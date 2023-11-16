/////////////////////////////////////////////////////////////
///// Fungerende kode for speedometer og avstand kjørt //////
/////////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>
#include "speedometerVariables.h"
#include "speedometerFunctions.h"
#include "displaySettings.h"

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;

void setup()
{
    encoder.init();
    setupDisplay();
}

void loop()
{
    motors.setSpeeds(400, 400);
    int holderEncoderLeft = encoder.getCountsAndResetLeft();
    int holderEncoderRight = encoder.getCountsAndResetRight();
    leftEncoderCount += (abs(holderEncoderLeft) + abs(holderEncoderRight)) / 2;
    rotationCount = calculateRotation();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 100)
    {
        previousMillis = currentMillis;
        rotationsPerSecond = calculateRotationPerSecond() * 10;
        previousRotationCount = rotationCount;
        percentageOfMaxSpeed += calculatePercentageOfMaxSpeed();
    }

    if (currentMillis - previousMillis3 > 60000)
    {
        distanceTraveledLast60Seconds = distanceTraveled - distanceTraveledLast60Seconds;
        previousMillis3 = currentMillis;
        secondsOverPercentage = percentageOfMaxSpeed / 10;
        averageSpeed = calculateAverageSpeed();
    }

    cmPerSecond = calculateCmPerSecond();
    highestSpeed = updateHighestSpeed();
    distanceTraveled = calculateDistanceTraveled();

    unsigned long currentMillis2 = millis();

    timePassedForValues = (currentMillis - previousMillis2) / 1000;

    // Viser values i 60 sekunder
    if (showingValues == true)
    {
        Serial.print("Viser vanlige verdier i 10 sek: ");
        Serial.println(currentMillis - previousMillis2);
        displayValues();
    }

    // Clearer Display for å vise average, etter 60 sek
    if (showingValues == true && currentMillis - previousMillis2 >= 60000)
    {
        clearDisplay();
        showingValues = false;
        previousMillis2 = currentMillis;
    }

    if (showingValues == false)
    {
        // Viser Average i 3 sekunder
        Serial.print("Viser Average i neste 3 sek: ");
        displayAverages();
        Serial.println(currentMillis - previousMillis2);
        timePassedForAverage = (currentMillis - previousMillis2) / 1000;

        // Clearer Display for neste iterasjon, etter 3 sek
        if (showingValues == false && currentMillis - previousMillis2 >= 3000)
        {
            clearDisplay();
            highestSpeed = 0;
            secondsOverPercentage = 0;
            averageSpeed = 0;
            showingValues = true;
            previousMillis2 = currentMillis;
        }
    }
}