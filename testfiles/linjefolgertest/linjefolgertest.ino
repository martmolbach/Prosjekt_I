

// 1. Calibrate sensor
//

#include <Wire.h>
#include <Zumo32U4.h>


Zumo32U4IRPulses iRPulse;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4LineSensors deadEndSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;

const int maxSpeed = 300;
const int calibrationTime = 3500;
const int NUM_SENSORS = 5;
bool deadEnd = false;
int16_t lastError = 0;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

bool leftCross = false;
bool rightCross = false;
bool potentialDeadEnd = false;
bool doubleCross = false;

unsigned int lineSensorValues[NUM_SENSORS];

void calibrateSensors(unsigned long startTime)
{
    while (millis() - startTime < calibrationTime)
    {
        lineSensors.calibrate();
        deadEndSensors.calibrate();
        motors.setSpeeds(200, -200);
    }
    motors.setSpeeds(0, 0);
}

void normalDriving()
{
    motors.setSpeeds(200, 200);
}

void setup()
{
    Serial.begin(115200);
    display.init();
    display.clear();
    display.print("Press A");
    display.gotoXY(0, 1);
    display.print("4 calib");
    lineSensors.initFiveSensors();
    deadEndSensors.initThreeSensors();
    buttonA.waitForButton();
    display.clear();
    unsigned long startTime = millis();
    display.print("Calib-");
    display.gotoXY(0, 1);
    display.print("rating");
    calibrateSensors(startTime);
    display.clear();
    display.print("Calib");
    display.gotoXY(0, 1);
    display.print("done");
    delay(2000);
    display.clear();
}

void loop()
{
    unsigned long currentMillis = millis();
    int16_t position = lineSensors.readLine(lineSensorValues);
    int error = position - 2000;

    int16_t speedDifference = error / 4 + 6 * (error - lastError);

    lastError = error;

    int16_t leftSpeed = maxSpeed + speedDifference;
    int16_t rightSpeed = maxSpeed - speedDifference;

    leftSpeed = constrain(leftSpeed, 0, maxSpeed);
    rightSpeed = constrain(rightSpeed, 0, maxSpeed);



    if (position != 0 && deadEnd == false)
    {
        motors.setSpeeds(leftSpeed, rightSpeed);
        previousMillis = currentMillis;
        previousMillis2 = currentMillis;
    }

    if (lineSensorValues[0] > 900 && lineSensorValues[4] < 300)
    {
        leftCross = true;
    }
    else if (lineSensorValues[4] > 900 && lineSensorValues[0] < 300)
    {
        rightCross = true;
    }

    else if (position == 0)
    {
        potentialDeadEnd = true;
    }
    else if (position >= 4500)
    {
        doubleCross = true;
    }

    if (leftCross == true)
    {
        normalDriving();
    }

    if (rightCross == true)
    {
        normalDriving();
    }



    else if (position == 0 && deadEnd == false && currentMillis - previousMillis2 > 100)
    {

        motors.setSpeeds(100, 100);
        if (currentMillis - previousMillis >= 1000)
        {
            deadEnd = true;
        }
    }

    if (deadEnd == true)
    {
        motors.setSpeeds(-200, -200);
        if (lineSensorValues[4] == 1000)
        {
            Serial.println("leftTurn");
            deadEnd = false;
        }
        if (lineSensorValues[0] == 1000)
        {
            Serial.println("rightTurn");
            deadEnd = false;
        }
    }
}