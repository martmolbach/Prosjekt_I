#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4IMU imu;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;

int16_t lastError = 0;
int maxSpeed = 400;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

const int threshold = 500; // Adjust threshold as needed
const int calibrationTime = 3500;


void setup() {
    Serial.begin(115200);    
    lineSensors.initFiveSensors();
    display.init();
    display.clear();
    buttonA.waitForButton();
    unsigned long startTime = millis();
    calibrateSensors(startTime);
}

void calibrateSensors(unsigned long startTime) {
    while(millis() - startTime < calibrationTime){
        lineSensors.calibrate();
        motors.setSpeeds(200,-200);
    }
    motors.setSpeeds(0,0);
}

void loop() {
    Serial.print(lineSensorValues[0]);
    Serial.print("\t");
    Serial.print(lineSensorValues[1]);
    Serial.print("\t");
    Serial.print(lineSensorValues[2]);
    Serial.print("\t");
    Serial.print(lineSensorValues[3]);
    Serial.print("\t");
    Serial.println(lineSensorValues[4]);
}