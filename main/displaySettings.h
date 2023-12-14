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
    void displaySettings(Zumo32U4IMU &imu, Zumo32U4Buzzer &buzzer, Zumo32U4OLED &display,)
    {
        display.init();
        display.clear();
        display.print("Welcome");
        buzzer.play(">g32>>c32");
        delay(1000);
        Wire.begin();
        imu.init();
        imu.enableDefault();
        imu.configureForBalancing();

    }
};

#endif