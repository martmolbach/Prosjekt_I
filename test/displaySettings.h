#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <Zumo32U4OLED.h>
#include <Zumo32U4Buzzer.h>
#include <Zumo32U4Encoders.h>
#include <Wire.h>
#include "gyro.h"
#include <Zumo32U4IMU.h>
struct DisplaySettings
{
    void setupFunction(Zumo32U4IMU &imu, Zumo32U4Buzzer &buzzer, Zumo32U4OLED &display, Gyro &gyro)
    {
        display.init();
        display.clear();
        display.print("Welcome");
        buzzer.play(">g32>>c32");
        delay(1000);
        Wire.begin();
        gyro.gyroSetup(imu);
    }

    void printAngles(Gyro &gyro, Zumo32U4OLED &display)
    {
        display.gotoXY(0, 5);
        display.print(gyro.angle);
        display.print(F("  "));
    }

    void displayChoiceOfDriving(Zumo32U4OLED &display)
    {
        display.gotoXY(2, 0);
        display.print("Press A for line");
        display.gotoXY(2, 2);
        display.print("Press B for square");
        display.gotoXY(2, 4);
        display.print("Press C for circle");
        display.gotoXY(2, 6);
        display.print("Press A+B for exit");
    }
};

#endif