#include <Zumo32U4.h>
#include <EEPROM.h>
#include <Wire.h>
#include "timer.h"
#include "lineSensorSetup.h"
#include "speedometer.h"
#include "gyro.h"
#include "enums.h"
#include "eepromSetup.h"
#include "displaySettings.h"
#include "drivingPatterns.h"
#include "buttonFunctions.h"
#include "waitForButton.h"

Zumo32U4Motors motors;
Zumo32U4IMU imu;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
EepromSetup eepromSetup;
Timer drivingPatternTimer1;
Timer drivingPatternTimer2;
DrivingPatterns drivingPatterns;
DisplaySettings displaySettings;
Zumo32U4Encoders encoder;
Gyro gyro;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
WaitForButton waitForButton;
LineSensorSetup lineSensorSetup;

void setup()
{
    Serial.begin(115200);
    displaySettings.setupFunction(imu, buzzer, display, gyro);
    Wire.begin();
    gyro.gyroSetup(imu);
    encoder.init();
    display.clear();
    display.setLayout21x8();
    waitForButton.waitForAorB(eepromSetup, motors, imu, drivingPatternTimer2, drivingPatternTimer1, buttonC, buttonA, buttonB, drivingPatterns, displaySettings, encoder, gyro, display);
}

void loop()
{
    Serial.println("hello!");
}