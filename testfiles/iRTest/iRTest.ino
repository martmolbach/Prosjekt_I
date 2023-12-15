#define DECODE_NEC

// Include necessary libraries and header files
#include <Keypad.h>
#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>
#include "PressureSensor.h"
#include "timer.h"
#include "DistanceSensor.h"
#include "keyPadSetup.h"

// Declare instances of necessary objects
KeyPadSetup keyPadSetup;
Timer timer;
Timer timer2;
Timer timer3;
PressureSensor pressureSensor;
DistanceSensor distanceSensor;

// Define keypad parameters
const byte ROWS = 4;
const byte COLS = 4;

// Define the Keymap
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Define the keypads row and column pins
byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};

// Create a keypad instance
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Initialize variables for data timer and state changer (related to IR remote)
unsigned int readingTimer = 500;
unsigned int changeViewState = 0;
int serialPrintChanger = 1;

bool enterPasswordStart = false; // Variable to check if '*' is pressed
bool commitPassword = false;     // Variable to check if '#' is pressed
bool unlockDoor = false;         // Variable to check if password is correct
String passwordHolder;           // String to store password input from keypad

int pulseState = HIGH; // Pulse state for distance sensor

// Function to check if recieved IR protocol is unknown
bool checkIfUnknownProtocol()
{
  if (IrReceiver.decodedIRData.protocol == UNKNOWN)
  {
    return true;
  }
  return false;
}


void setup()
{
  // Set up serial communication at 115200 baud rate
  Serial.begin(115200);

  // Initialize distance sensor
  distanceSensor.distanceSensorSetup();

  // Initialize IR reciever
  IrReceiver.begin(IR_RECEIVE_PIN, enableLEDFeedback);

  // Print available IR protocol
  Serial.print(F("Ready to recieve IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

// Main loop function
void loop()
{

  ////////////////////////////////////////////
  ////////////////////////////////////////////
  //////////////IR RECIEVER///////////////////
  ////////////////////////////////////////////
  ////////////////////////////////////////////

  // Check if an IR signal is recieved
  if (IrReceiver.decode())
  {
    // PRINT IR RESULT (UNCOMMENT AND UPLOAD TO SEE PRINT FROM IR RECIEVER TO ADD MORE BUTTONS ON CONTROLLER)
    // IrReceiver.printIRResultShort(&Serial);
    // IrReceiver.printIRSendUsage(&Serial);

    // Check if the protocol is unknown
    if (checkIfUnknownProtocol())
    {
      // Print unknown protocol information to serial communication
      Serial.println("Recieved unknown protocol.");
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    // Resume IR reciever
    IrReceiver.resume();

    // Adjust variables based on the recieved IR information
    if (IrReceiver.decodedIRData.command == 0x46)
    {
      readingTimer = readingTimer + 50; // Adjust readingTimer based on button input
    }
    else if (IrReceiver.decodedIRData.command == 0x15)
    {

      // If readingTimer within given bounds change value accordingly
      if (readingTimer <= 50)
      {
        readingTimer = readingTimer - 5;
      }
      else if (readingTimer < 10)
      {
        readingTimer = readingTimer;
      }
      else if (readingTimer >= 100)
      {
        readingTimer = readingTimer - 50;
      }
    }

    else if (IrReceiver.decodedIRData.command == 0x43)
    {
      if (serialPrintChanger <= 4)
      {
        serialPrintChanger = serialPrintChanger + 1; // Change what information is printed to serial communication
      }
    }
    else if (IrReceiver.decodedIRData.command == 0x44)
    {
      if (serialPrintChanger >= 1)
      {
        serialPrintChanger = serialPrintChanger - 1; // Change what information is printed to serial communication
        Serial.println(serialPrintChanger);
      }
    }
  }

  ////////////////////////////////////////////
  ////////////////////////////////////////////
  //////////////PRESSURE SENSOR///////////////
  ////////////////////////////////////////////
  ////////////////////////////////////////////

  // Update the pressure sensor value at given intervals based on readingTimer
  if (timer.isFinished(readingTimer))
  {
    timer.reset(); // Reset the timer
    pressureSensor.pressureSensorValue = pressureSensor.updatePressureValue();
  }
  pressureSensor.pressureSensorThresholdString();

  ////////////////////////////////////////////
  ////////////////////////////////////////////
  /////////////////KEYPAD/////////////////////
  ////////////////////////////////////////////
  ////////////////////////////////////////////

  char keyPress = keypad.getKey();          // Update keyPress variable based on the input from keypad
  keyPadSetup.enterPassword(keyPress);      // Commit to entering a password if '*' is pressed
  keyPadSetup.storePasswordInput(keyPress); // Store password input in a String (passwordHolder)
  keyPadSetup.passwordCommit(keyPress);     // Commit to comparing passwordHolder to correct password
  keyPadSetup.passwordCompare();            // Compare passwordHolder to correct password (Open Door / Wrong password)

  ////////////////////////////////////////////
  ////////////////////////////////////////////
  //////////////DISTANCE SENSOR///////////////
  ////////////////////////////////////////////
  ////////////////////////////////////////////

  // Changes the pulseState variable from HIGH to LOW every 15 micro seconds
  if (timer3.isFinishedMicros(15))
  {
    timer3.resetMicros(); // reset the timer
    pulseState = !pulseState;
    digitalWrite(distanceSensor.triggerPin, pulseState);
  }
  distanceSensor.calculateDuration(); // Calculates the duration of the sound wave
  distanceSensor.calculateDistance(); // calculates the distance to the object

  ////////////////////////////////////////////
  ////////////////////////////////////////////
  ////////////SERIAL PRINT CHANGER////////////
  ////////////////////////////////////////////
  ////////////////////////////////////////////

  // Switch case to change which prints to view
  switch (serialPrintChanger)
  {
  case 1: // PressureSensor View
    if (timer2.isFinished(1000))
    {
      timer2.reset();
      pressureSensor.pressureSensorSerialPrint();
      Serial.println();
    }
    break;

  case 2: // Keypad View
    keyPadSetup.printKeyPress(keyPress);
    break;

  case 3: // Distance Sensor View
    if (timer3.isFinished(1000))
    {
      timer3.reset();
      Serial.print("Distance sensor on keypad value: ");
      Serial.print(distanceSensor.distanceInCm);
      Serial.println();
    }
    break;
  }
}