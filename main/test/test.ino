///////////////////////////////////////////////////////////
///// Fungerende kode for speedometer og linjefølger //////
///////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>
#include <EEPROM.h>

Zumo32U4Encoders encoder;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4ButtonB buttonB;

unsigned long previousMillis = 0;
unsigned long previousNumber = 0;
unsigned long previousWaitTimeForNext = 0;
unsigned long lastHealthChange = 0;

unsigned long waitTimeForNext = 0;

unsigned long allowHealthChangeMillis = 0;
unsigned long healthChangeMillis = 0;

const int NUM_SENSORS = 5;
unsigned int lineSensorValues[NUM_SENSORS];


int batteryLevel = 0;
int batteryHealth = 0;

const int batteryHealthAddress = 0;
const int batteryLevelAddress = 1;


bool allowHealthChange = false;
bool hasHealthChanged = false;

int randomNumber1 = 0;
int randomNumber2 = 0;
int randomNumber3 = 0;
int randomNumber4 = 0;


// void batteryPercentage(){
//     if (cmPerSecond > 65*0.7){
//         batteryOver70 = true;
//     }
//     else{
//         batteryOver70 = false;
//     }
// }


void setup() 
{
    display.init();
    display.clear();
    display.gotoXY(2,4);
    display.setLayout21x8();
    display.print("Trykk A for start");
    display.gotoXY(2,5);
    display.print("Trykk B for reset");
    waitForAorB();
    lineSensors.initFiveSensors();
    encoder.init();
    display.clear();
}

void waitForAorB(){
    while(true){
        if (buttonA.isPressed()){
            break;
        }
        else if (buttonB.isPressed()){
            EEPROM.write(batteryHealthAddress, 100);
            EEPROM.write(batteryLevelAddress, 100);
            break;
        }
    }
}

void displayValues() 
{
    display.gotoXY(0,0);
    display.print("Battery Health:");
    display.gotoXY(0,1);
    display.print(batteryHealth);
}

void loop() {
    
    batteryHealth = EEPROM.read(batteryHealthAddress);
    Serial.println(lineSensors.readLine(lineSensorValues));
    
    // Random tidspunkt i ny og ned
    randomNumber1 = random(0, 10000); // Change to 100000
    randomNumber2 = random(0, 10000);
    randomNumber3 = random(0, 10000);
    randomNumber4 = random(0, 10000);

    //Serial.print("Bool: ");
    //Serial.println(hasHealthChanged);
    displayValues();
    //Serial.println(batteryHealth);

    unsigned long start = millis();
    if ((randomNumber1 == randomNumber2 == randomNumber3 == randomNumber4) && hasHealthChanged == false){
        // Reduce battery 50%
        batteryHealth = batteryHealth / 2;
        EEPROM.update(batteryHealthAddress,batteryHealth);
        display.clear();
        lastHealthChange = start;
        start = start - previousNumber;
        previousNumber = start;
        hasHealthChanged = true;
    }

    if (millis() - lastHealthChange >= 240000 && hasHealthChanged == true){
        hasHealthChanged = false;
        previousWaitTimeForNext = waitTimeForNext;
    }



    // Serial.println(healthChangeMillis);
    // EEPROM.update(batteryHealthAddress, EEPROM.read(batteryHealthAddress) / 2);
}