


// 1. Calibrate sensor
// 

#include <Wire.h>
#include <Zumo32U4.h>


Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;


const int maxSpeed = 400;
const int calibrationTime = 3500;
const int NUM_SENSORS = 5;

int16_t lastError = 0;

unsigned int lineSensorValues[NUM_SENSORS];


void calibrateSensors(unsigned long startTime) {
    while(millis() - startTime < calibrationTime){
        lineSensors.calibrate();
        motors.setSpeeds(200,-200);
    }
    motors.setSpeeds(0,0);
}

void setup() {
    display.init();
    display.clear();
    display.print("Press A");
    display.gotoXY(0,1);
    display.print("4 calib");
    lineSensors.initFiveSensors();
    buttonA.waitForButton();
    display.clear();
    unsigned long startTime = millis();
    display.print("Calib-");
    display.gotoXY(0,1);
    display.print("rating");
    calibrateSensors(startTime);
    display.clear();
    display.print("Calib");
    display.gotoXY(0,1);
    display.print("done");
    delay(2000);
    display.clear();
}

void loop(){
    int16_t position = lineSensors.readLine(lineSensorValues);

    int error = position - 2000;

    int16_t speedDifference = error / 4 + 6 * (error - lastError);

    lastError = error;

    int16_t leftSpeed = maxSpeed + speedDifference;
    int16_t rightSpeed = maxSpeed - speedDifference;

    leftSpeed = constrain(leftSpeed, 0, maxSpeed);
    rightSpeed = constrain(rightSpeed, 0, maxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
}