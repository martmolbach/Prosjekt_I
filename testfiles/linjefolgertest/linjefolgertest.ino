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
bool hasRightCrossBeenDetected = false;
bool hasLeftCrossBeenDetected = false;
bool hasCrossBeenDetected = false;
bool deadEnd = false;

unsigned long timeForZeroValues = 0;
unsigned long previousTimeForZeroValues = 0;


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

    // Serial.print("Sensor 1: ");
    // Serial.print(lineSensorValues[0]);
    // Serial.print("\t");
    // Serial.print("Sensor 2: ");
    // Serial.print(lineSensorValues[1]);
    // Serial.print("\t");
    // Serial.print("Sensor 3: ");
    // Serial.print(lineSensorValues[2]);
    // Serial.print("\t");
    // Serial.print("Sensor 4: ");
    // Serial.print(lineSensorValues[3]);
    // Serial.print("\t");
    // Serial.print("Sensor 5: ");
    // Serial.print(lineSensorValues[4]);
    Serial.print("\t");
    Serial.print("Total: ");
    Serial.print(lineSensors.readLine(lineSensorValues));
    Serial.print("\t");
    Serial.print("Right: ");
    Serial.print(hasRightCrossBeenDetected);
    Serial.print("\t");
    Serial.print("Left: ");
    Serial.print(hasLeftCrossBeenDetected);
    Serial.print("\t");
    Serial.print("Cross: ");
    Serial.println(hasCrossBeenDetected);

    // hasRightCrossBeenDetected = false;
    // hasLeftCrossBeenDetected = false;
    // hasCrossBeenDetected = false;


    if(lineSensorValues[4] > 900 && lineSensorValues[0] < 300 && hasRightCrossBeenDetected == false){
        display.clear();
        display.print("RIGHT CROSS");
        hasLeftCrossBeenDetected = false;
        hasCrossBeenDetected = false;
        hasRightCrossBeenDetected = true;
    }

    if(lineSensorValues[4] < 300 && lineSensorValues[0] > 900 && hasLeftCrossBeenDetected == false){
        display.clear();
        display.print("LEFT CROSS");
        hasLeftCrossBeenDetected = true;
        hasCrossBeenDetected = false;
        hasRightCrossBeenDetected = false;
    }

    if(lineSensorValues[0] > 900 && lineSensorValues[1] > 900 && lineSensorValues[2] > 900 && lineSensorValues[3] > 900 && lineSensorValues[4] > 900 && hasCrossBeenDetected == false){
        display.clear();
        display.print("CROSS");
        hasLeftCrossBeenDetected = false;
        hasCrossBeenDetected = true;
        hasRightCrossBeenDetected = false;
    }

    if(lineSensorValues == 0 && (hasRightCrossBeenDetected == true || hasLeftCrossBeenDetected == true || hasCrossBeenDetected == true)){
        previousTimeForZeroValues = millis();
        Serial.println("NEGER");
        if(millis() - previousTimeForZeroValues >= 1000){
            deadEnd = true;
            Serial.println("DeadEnd Niggaballs");
            previousTimeForZeroValues = timeForZeroValues;
        }
        if (deadEnd == true){
            Serial.println("BALLESTEIN");
            // Reverse
        }
    }



    // Linjefølger del
    int error = position - 2000;

    int16_t speedDifference = error / 4 + 6 * (error - lastError);

    lastError = error;

    int16_t leftSpeed = maxSpeed + speedDifference;
    int16_t rightSpeed = maxSpeed - speedDifference;

    leftSpeed = constrain(leftSpeed, 0, maxSpeed);
    rightSpeed = constrain(rightSpeed, 0, maxSpeed);

    //motors.setSpeeds(leftSpeed, rightSpeed);
}