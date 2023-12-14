#ifndef LINESENSORSETUP_H
#define LINESENSORSETUP_H
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4OLED.h>
#include "timer.h"

struct LineSensorSetup
{
    const int calibrationTime = 3500;

    void calibrateSensors(Zumo32U4LineSensors &lineSensors, Zumo32U4Motors &motors, Timer &timer1)
    {

        while (millis() - timer1.startTime < calibrationTime)
        {
            lineSensors.calibrate();
            motors.setSpeeds(200, -200);
        }
        motors.setSpeeds(0, 0);
    }

    void lineSensorSetup1(Zumo32U4LineSensors &lineSensors, Zumo32U4OLED &display, Timer &timer1)
    {
        lineSensors.initFiveSensors();
        display.clear();
        display.gotoXY(4, 3);
        display.print("Calibrating...");
        timer1.startTime = millis();
    }
    void lineSensorSetup2(Zumo32U4LineSensors &lineSensors, Zumo32U4Motors &motors, Zumo32U4OLED &display, Timer &timer1)
    {
        calibrateSensors(lineSensors, motors, timer1);
        display.clear();
        display.gotoXY(5, 3);
        display.print("Calibration");
        display.gotoXY(9, 4);
        display.print("Done");
        display.clear();
    }
};

#endif
