/////////////////////////////////////////////////////////////
///// Fungerende kode for speedometer og avstand kjørt //////
/////////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>
#include "speedometerVariables.h"
#include "speedometerFunctions.h"

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;

void setup()
{
    encoder.init();
    display.init();
    display.clear();
    display.setLayout21x8();
}

void displayValues()
{
    display.gotoXY(0, 0);
    display.print("Speed:");
    display.gotoXY(0, 1);
    display.print(cmPerSecond);
    display.print("cm/s");
    display.gotoXY(0, 6);
    display.print("Distance:");
    display.gotoXY(0, 7);
    display.print(distanceTraveled); // 3.75 er dm til hjulet
    display.print("m");
    display.gotoXY(16, 0);
    display.print("Money");
    display.gotoXY(11, 1);
    display.print("Used:");
    display.gotoXY(17, 1);
    display.print("4kr");
    display.gotoXY(9, 2);
    display.print("Earned:");
    display.gotoXY(17, 2);
    display.print("2kr");
    display.gotoXY(10, 3);
    display.print("Total:");
    display.gotoXY(17, 3);
    display.print("7kr");
    display.gotoXY(16, 6);
    display.print("Time:");
    display.gotoXY(16, 7);
    display.print(timePassedForValues);
    display.print("sec");
}

void displayAverages()
{

    display.gotoXY(0, 0);
    display.print("Average speed:");
    display.gotoXY(0, 1);
    display.print(averageSpeed); // averageSpeed
    display.print("cm/s");

    display.gotoXY(0, 3);
    display.print("Highest speed:");
    display.gotoXY(0, 4);
    display.print(highestSpeed); // highestSpeed
    display.print("cm/s");

    display.gotoXY(0, 6);
    display.print("Over 70% max:");
    display.gotoXY(0, 7);
    display.print(secondsOverPercentage); // secondsOverPercentage
    display.print("sec");

    display.gotoXY(16, 6);
    display.print("Time: ");
    display.gotoXY(16, 7);
    display.print(timePassedForAverage);
    display.print("sec");
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
        display.clear();
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
            display.clear();
            highestSpeed = 0;
            secondsOverPercentage = 0;
            averageSpeed = 0;
            showingValues = true;
            previousMillis2 = currentMillis;
        }
    }
}