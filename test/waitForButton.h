#ifndef WAITFORBUTTON_H
#define WAITFORBUTTON_H

#include <Zumo32U4OLED.h>
#include <Zumo32U4Buttons.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4Encoders.h>
#include <Zumo32U4IMU.h>
#include "timer.h"
#include "eepromSetup.h"
#include "gyro.h"
#include "displaySettings.h"
#include "drivingPatterns.h"

struct WaitForButton
{
    void buttonChoiceDisplay(Zumo32U4OLED &display)
    {
        
        display.clear();
        display.gotoXY(2, 1);
        display.print("Press A for start");
        display.gotoXY(2, 3);
        display.print("Press B for reset");
        display.gotoXY(2, 5);
        display.print("Press C for patterns");
    }

    void buttonCPress(Zumo32U4Motors &motors, Zumo32U4IMU &imu, Timer &drivingPatternTimer2, Timer &drivingPatternTimer1, Zumo32U4ButtonC &buttonC, Zumo32U4ButtonA &buttonA, Zumo32U4ButtonB &buttonB, DrivingPatterns &drivingPatterns, DisplaySettings &displaySettings, Zumo32U4Encoders &encoder, Gyro &gyro, Zumo32U4OLED &display)
    {
        display.clear();
        display.print(F("Gyroscope calibrating"));
        delay(500);

        for (uint16_t i = 0; i < 1024; i++)
        {
            while (!imu.gyroDataReady())
            {
            }
            imu.readGyro();
            gyro.updateGyroOffset(imu);
        }

        gyro.gyroOffsetZ /= 1024;
        display.clear();
        display.print("Press A to continue");
        while (!buttonA.getSingleDebouncedRelease())
        {
            gyro.updateAngleGyro(imu);
            static uint8_t lastCorrectionTime = 0;
            uint8_t m = millis();
            if ((uint8_t)(m - lastCorrectionTime) >= 20)
            {
                lastCorrectionTime = m;
                displaySettings.printAngles(gyro, display);
            }
        }
        delay(500);
        while (true)
        {
            displaySettings.displayChoiceOfDriving(display);
            if (drivingStyleChoice(motors, imu, drivingPatternTimer2, drivingPatternTimer1, buttonC, buttonA, buttonB, drivingPatterns, displaySettings, encoder, gyro, display))
            {
                break;
            }
        }
    }

    bool checkButtonPress(EepromSetup &eepromSetup, Zumo32U4Motors &motors, Zumo32U4IMU &imu, Timer &drivingPatternTimer2, Timer &drivingPatternTimer1, Zumo32U4ButtonC &buttonC, Zumo32U4ButtonA &buttonA, Zumo32U4ButtonB &buttonB, DrivingPatterns &drivingPatterns, DisplaySettings &displaySettings, Zumo32U4Encoders &encoder, Gyro &gyro, Zumo32U4OLED &display)
    {
        if (buttonA.getSingleDebouncedRelease())
        {
            display.clear();
            return true;
        }
        else if (buttonB.getSingleDebouncedRelease())
        {
            eepromSetup.eepromValueSetup();
            display.clear();
            return true;
        }
        else if (buttonC.getSingleDebouncedRelease())
        {
            buttonCPress(motors, imu, drivingPatternTimer2, drivingPatternTimer1, buttonC, buttonA, buttonB, drivingPatterns, displaySettings, encoder, gyro, display);
        }
    }

    void waitForAorB(EepromSetup &eepromSetup, Zumo32U4Motors &motors, Zumo32U4IMU &imu, Timer &drivingPatternTimer2, Timer &drivingPatternTimer1, Zumo32U4ButtonC &buttonC, Zumo32U4ButtonA &buttonA, Zumo32U4ButtonB &buttonB, DrivingPatterns &drivingPatterns, DisplaySettings &displaySettings, Zumo32U4Encoders &encoder, Gyro &gyro, Zumo32U4OLED &display)
    {
        while (true)
        {
            buttonChoiceDisplay(display);
            if (checkButtonPress(eepromSetup, motors, imu, drivingPatternTimer2, drivingPatternTimer1, buttonC, buttonA, buttonB, drivingPatterns, displaySettings, encoder, gyro, display))
            {
                break;
            }
        }
    }

    bool drivingStyleChoice(Zumo32U4Motors &motors, Zumo32U4IMU &imu, Timer &drivingPatternTimer2, Timer &drivingPatternTimer1, Zumo32U4ButtonC &buttonC, Zumo32U4ButtonA &buttonA, Zumo32U4ButtonB &buttonB, DrivingPatterns &drivingPatterns, DisplaySettings &displaySettings, Zumo32U4Encoders &encoder, Gyro &gyro, Zumo32U4OLED &display)
    {
        if (buttonA.getSingleDebouncedRelease())
        {
            display.clear();
            while (true)
            {
                if (drivingStyleA(display, displaySettings, drivingPatterns, encoder, motors, gyro, drivingPatternTimer1, drivingPatternTimer2, imu))
                {
                    break;
                }
            }
            display.clear();
        }
        else if (buttonB.getSingleDebouncedRelease())
        {
            display.clear();
            while (true)
            {
                if (drivingStyleB(displaySettings, encoder, drivingPatterns, motors, gyro, drivingPatternTimer1, drivingPatternTimer2, imu, display))
                {
                    break;
                }
            }
        }
        else if (buttonC.getSingleDebouncedRelease())
        {
            drivingStyleC(imu, encoder, gyro, displaySettings, display, drivingPatterns, motors);
        }

        else if ((buttonA.isPressed()) && (buttonB.isPressed()))
        {
            display.clear();
            delay(1000);
            return true;
        }
    }

    bool drivingStyleA(Zumo32U4OLED &display, DisplaySettings &displaySettings, DrivingPatterns &drivingPatterns, Zumo32U4Encoders &encoder, Zumo32U4Motors &motors, Gyro &gyro, Timer &drivingPatternTimer1, Timer &drivingPatternTimer2, Zumo32U4IMU &imu)
    {
        drivingPatterns.forwardsBackwards(motors, gyro, drivingPatternTimer1, drivingPatternTimer2, encoder);
        static byte lastCorrectionTime = 0;
        byte m = millis();
        if ((byte)(m - lastCorrectionTime) >= 20)
        {
            lastCorrectionTime = m;
            displaySettings.printAngles(gyro, display);
        }
        if (drivingPatterns.currentState == finish)
        {
            drivingPatterns.currentState = moveForward;
            return true;
        }
        int holderEncoderLeft = encoder.getCountsAndResetLeft();
        int holderEncoderRight = encoder.getCountsAndResetRight();
        gyro.updateAngleGyro(imu);
    }

    bool drivingStyleB(DisplaySettings &displaySettings, Zumo32U4Encoders &encoder, DrivingPatterns &drivingPattern, Zumo32U4Motors &motors, Gyro &gyro, Timer &drivingPatternTimer1, Timer &drivingPatternTimer2, Zumo32U4IMU &imu, Zumo32U4OLED &display)
    {
        display.clear();
        while (true)
        {
            drivingPattern.squareMode(motors, gyro, drivingPatternTimer1, drivingPatternTimer2, encoder);
            gyro.updateAngleGyro(imu);
            static byte lastCorrectionTime = 0;
            byte m = millis();
            if ((byte)(m - lastCorrectionTime) >= 20)
            {
                lastCorrectionTime = m;
                displaySettings.printAngles(gyro, display);
            }
            if (drivingPattern.forwardCounter == 4)
            {
                break;
            }
            int holderEncoderLeft = encoder.getCountsAndResetLeft();
            int holderEncoderRight = encoder.getCountsAndResetRight();
        }
    }

    void drivingStyleC(Zumo32U4IMU &imu, Zumo32U4Encoders &encoder, Gyro &gyro, DisplaySettings &displaySettings, Zumo32U4OLED &display, DrivingPatterns &drivingPatterns, Zumo32U4Motors &motors)
    {
        display.clear();
        while (true)
        {
            drivingPatterns.circleMode(motors);
            static byte lastCorrectionTime = 0;
            byte m = millis();
            if ((byte)(m - lastCorrectionTime) >= 20)
            {
                lastCorrectionTime = m;
                displaySettings.printAngles(gyro, display);
            }
            if (gyro.angle >= 360)
            {
                gyro.angle = 0;
                motors.setSpeeds(0, 0);
                break;
            }
            int holderEncoderLeft = encoder.getCountsAndResetLeft();
            int holderEncoderRight = encoder.getCountsAndResetRight();
            gyro.updateAngleGyro(imu);
        }
        display.clear();
    }
};

#endif