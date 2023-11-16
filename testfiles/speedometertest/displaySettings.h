#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H
#include "speedometerVariables.h"
#include <Zumo32U4OLED.h>
Zumo32U4OLED display;

void clearDisplay()
{
    display.clear();
}

void setupDisplay()
{
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
#endif