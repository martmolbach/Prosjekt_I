/////////////////////////////////////////////////////////////
///// Fungerende kode for speedometer og avstand kjørt //////
/////////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>
#include "speedometer.h"
#include "displaySettings.h"
#include "timer.h"

Zumo32U4Motors motors;
Speedometer speedometer;
DisplaySettings displaySettings;
Zumo32U4Encoders encoder;
Zumo32U4OLED display;
Timer timer1;
Timer timer2;
Timer timer3;
DisplayState currentDisplayState = Normal;
Zumo32U4ButtonA buttonA;

void setup()
{
    encoder.init();
    displaySettings.setupDisplay(display);
    if (buttonA.waitForRelease())
    {
        
    }
}

void loop()
{
    motors.setSpeeds(400, 400);
    speedometer.encoderCount += speedometer.calculateEncoderCount(encoder);
    speedometer.rotationCount = speedometer.calculateRotation();
    speedometer.cmPerSecond = speedometer.calculateCmPerSecond();
    speedometer.distanceTraveled = speedometer.calculateDistanceTraveled();
    speedometer.highestSpeed = speedometer.updateHighestSpeed();
    unsigned long currentMillis = millis();

    if (timer1.isFinished(100))
    {
        timer1.reset();
        speedometer.rotationsPerSecond = speedometer.calculateRotationPerSecond() * 10;
        speedometer.previousRotationCount = speedometer.rotationCount;
        speedometer.percentageOfMaxSpeed += speedometer.calculatePercentageOfMaxSpeed();
    }

    switch (currentDisplayState)
    {
    case Normal:
        timer1.timePassedForValues = (currentMillis - timer3.previousMillis) / 1000;
        displaySettings.displayValues(speedometer, display, timer1);
        if (timer3.isFinished(60000))
        {
            timer2.previousMillis = currentMillis;
            speedometer.distanceTraveledLast60Seconds = speedometer.distanceTraveled - speedometer.distanceTraveledLast60Seconds;
            speedometer.secondsOverPercentage = speedometer.percentageOfMaxSpeed / 10;
            speedometer.averageSpeed = speedometer.calculateAverageSpeed();
            timer1.timePassedForAverage = 0;
            displaySettings.clearDisplay(display);
            currentDisplayState = Average;
        }
        break;

    case Average:
        timer1.timePassedForAverage = (currentMillis - timer2.previousMillis) / 1000;
        displaySettings.displayAverages(speedometer, display, timer1);
        if (timer2.isFinished(5000))
        {
            timer3.previousMillis = currentMillis;
            speedometer.resetAverageValues();
            displaySettings.clearDisplay(display);
            timer1.timePassedForValues = 0;
            currentDisplayState = Normal;
        }
        break;
    }
}