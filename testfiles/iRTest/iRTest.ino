#define DECODE_NEC

#include <Keypad.h>
#include <Arduino.h>

#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>
#include "PressureSensor.h"
#include "timer.h"
#include "DistanceSensor.h"
#include "keyPadSetup.h"

KeyPadSetup keyPadSetup;
Timer timer;
Timer timer2;
Timer timer3;
PressureSensor pressureSensor;
DistanceSensor distanceSensor;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned int readingTimer = 500;
unsigned int changeViewState = 0;
int serialPrintChanger = 1;

bool enterPasswordStart = false;
bool commitPassword = false;
bool unlockDoor = false;
int pulseState = HIGH;
String passwordHolder;

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
  Serial.begin(115200);

  distanceSensor.distanceSensorSetup();
  IrReceiver.begin(IR_RECEIVE_PIN, enableLEDFeedback);
  Serial.print(F("Ready to recieve IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

void loop()
{
  if (IrReceiver.decode())
  {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);

    if (checkIfUnknownProtocol())
    {
      Serial.println("Recieved unknown protocol.");
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    //  Serial.println();

    IrReceiver.resume();

    if (IrReceiver.decodedIRData.command == 0x46)
    {
      readingTimer = readingTimer + 50; // Change the speed that the data updates
                                        //    Serial.println(changeSomeValue);
    }
    else if (IrReceiver.decodedIRData.command == 0x15)
    {
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
        readingTimer = readingTimer - 50; // Change the speed that the data updates
      }
      //  Serial.println(changeSomeValue);
    }

    else if (IrReceiver.decodedIRData.command == 0x43)
    {
      if (serialPrintChanger <= 4)
      {
        serialPrintChanger = serialPrintChanger + 1; // Change the speed that the data updates
      }
    }
    else if (IrReceiver.decodedIRData.command == 0x44)
    {
      if (serialPrintChanger >= 1)
      {
        serialPrintChanger = serialPrintChanger - 1; // Change the speed that the data updates
        Serial.println(serialPrintChanger);
      }
    }
  }

  if (timer.isFinished(readingTimer))
  {
    timer.reset();
    pressureSensor.pressureSensorValue = pressureSensor.updatePressureValue();
    //    Serial.println(pressureSensor.pressureSensorValue);
  }

  pressureSensor.pressureSensorThresholdString();

  char keyPress = keypad.getKey();
  keyPadSetup.passwordCommit(keyPress);
  keyPadSetup.enterPassword(keyPress);
  keyPadSetup.storePasswordInput(keyPress);
  keyPadSetup.passwordCommit();

  if (timer3.isFinishedMicros(15))
  {
    timer3.resetMicros();
    pulseState = !pulseState;
    digitalWrite(distanceSensor.triggerPin, pulseState);
  }
  distanceSensor.calculateDuration();
  distanceSensor.calculateDistance();

  switch (serialPrintChanger)
  {
  case 1:
    if (timer2.isFinished(1000))
    {
      timer2.reset();
      pressureSensor.pressureSensorSerialPrint();
      Serial.println();
    }
    break;

  case 2:
    keyPadSetup.printKeyPress(keyPress);
    break;

  case 3:
    if(timer3.isFinished(1000))
    {
      timer3.reset();
      Serial.print("Distance sensor on keypad value: ");
      Serial.print(distanceSensor.distanceInCm);
      Serial.println();
    }
    break;
  }
}