#ifndef LINESENSOR_H
#define LINESENSOR_H
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4OLED.h>

struct LineSensorSetup
{
    unsigned long startTime;
    const int calibrationTime = 3500;

    void lineSensorSetup(Zumo32U4LineSensors &lineSensors, Zumo32U4OLED &display)
    {
        lineSensors.initFiveSensors();
        display.clear();
        display.gotoXY(4, 3);
        display.print("Calibrating...");
        startTime = millis();
        calibrateSensors(startTime);
        display.clear();
        display.gotoXY(5, 3);
        display.print("Calibration");
        display.gotoXY(9, 4);
        display.print("Done");
        display.clear();
    }

    void calibrateSensors(Zumo32U4LineSensors &LineSensors, Zumo32U4Motors &motors)
    {

        while (millis() - startTime < calibrationTime)
        {
            LineSensor.calibrate();
            motors.setSpeeds(200, -200);
        }
        motors.setSpeeds(0, 0);
    }
};

#endif
