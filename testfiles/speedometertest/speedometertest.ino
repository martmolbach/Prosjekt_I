/////////////////////////////////////////////////////////////
///// Fungerende kode for speedometer og avstand kjørt //////
/////////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>
#include "speedometerVariables.h"
#include "speedometerFunctions.h"
#include "displaySettings.h"

Zumo32U4Motors motors;

displayState currentDisplayState = Normal;

void setup()
{
    encoder.init();
    setupDisplay();
}

void loop()
{
    motors.setSpeeds(400, 400);
    encoderCount += calculateEncoderCount();
    rotationCount = calculateRotation();
    cmPerSecond = calculateCmPerSecond();
    distanceTraveled = calculateDistanceTraveled();
    highestSpeed = updateHighestSpeed();
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 100)
    {
        previousMillis = currentMillis;
        rotationsPerSecond = calculateRotationPerSecond() * 10;
        previousRotationCount = rotationCount;
        percentageOfMaxSpeed += calculatePercentageOfMaxSpeed();
    }

    switch (currentDisplayState)
    {
    case Normal:
        displayValues();
        timePassedForValues = (currentMillis - previousMillis2) / 1000;
        if (currentMillis - previousMillis3 > 60000)
        {
            previousMillis2 = currentMillis;
            distanceTraveledLast60Seconds = distanceTraveled - distanceTraveledLast60Seconds;
            secondsOverPercentage = percentageOfMaxSpeed / 10;
            averageSpeed = calculateAverageSpeed();
            clearDisplay();
            currentDisplayState = Average;
        }
        break;

    case Average:
        displayAverages();
        timePassedForAverage = (currentMillis - previousMillis2) / 1000;
        if (currentMillis - previousMillis2 > 3000)
        {
            previousMillis3 = currentMillis;
            resetAverageValues();
            clearDisplay();
            timePassedForValues = 0;
            currentDisplayState = Normal;
        }
        break;
    }
}