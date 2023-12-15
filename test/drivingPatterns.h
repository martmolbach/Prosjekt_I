#ifndef DRIVINGPATTERNS_H
#define DRIVINGPATTERNS_H
#include "gyro.h"
#include "enums.h"
#include "timer.h"
#include <Zumo32U4Motors.h>
#include <Zumo32U4Encoders.h>
struct DrivingPatterns
{
    const int stopDuration = 2000;
    int forwardCounter = 0;
    const float countPerRotation = 75.81 * 12;
    const float wheelDiameter = 3.75;
    State currentState = moveForward;

    bool countDistance(float targetDistance, Zumo32U4Encoders &encoder)
    {
        static float totalDistance = 0;

        int leftEncoder = encoder.getCountsLeft();
        int rightEncoder = encoder.getCountsRight();
        float encoderCount = (abs(leftEncoder) + abs(rightEncoder)) / 2.0;
        float rotationCounter = encoderCount / countPerRotation;
        float currentDistance = rotationCounter * (PI * wheelDiameter);

        totalDistance += currentDistance;

        if (totalDistance >= targetDistance)
        {
            totalDistance = 0;
            return true;
        }

        return false;
    }

    void circleMode(Zumo32U4Motors &motors)
    {
        motors.setSpeeds(100,200);
        
    }

    void forwardsBackwards(Zumo32U4Motors &motors, Gyro &gyro, Timer &drivingPatternTimer1, Timer &drivingPatternTimer2, Zumo32U4Encoders &encoder)
    {
        float targetDistance = 50;
        float turnDistance = 30;
        int distance = 1;

        switch (currentState)
        {

        case moveForward:
            motors.setSpeeds(200, 200);
            if (countDistance(targetDistance, encoder))
            {
                currentState = stopForward;
            }
            break;

        case stopForward:
            motors.setSpeeds(0, 0);
            if (drivingPatternTimer1.isFinished(stopDuration))
            {
                drivingPatternTimer1.reset();
                currentState = turnAround;
                gyro.angle = 0;
            }
            break;

        case turnAround:
            motors.setSpeeds(-100, 100);
            if (gyro.angle >= 180)
            {
                gyro.angle = 0;
                currentState = stopTurn;
            }
            break;

        case stopTurn:
            motors.setSpeeds(0, 0);
            if (drivingPatternTimer1.isFinished(stopDuration))
            {
                drivingPatternTimer1.reset();
                currentState = moveForward2;
            }
            break;

        case moveForward2:
            motors.setSpeeds(200, 200);
            if (countDistance(targetDistance, encoder))
                currentState = stop;
            break;

        case stop:
            motors.setSpeeds(0, 0);
            if (drivingPatternTimer1.isFinished(stopDuration))
            {
                drivingPatternTimer1.reset();
                currentState = finish;
            }
            break;

        case finish:
            break;

        default:
            break;
        }
    }

    void squareMode(Zumo32U4Motors &motors, Gyro &gyro, Timer &drivingPatternTimer1, Timer &drivingPatternTimer2, Zumo32U4Encoders &encoder)
    {

        unsigned long currentTime = millis();
        float targetDistance = 0.4;
        switch (currentState)
        {
        case moveForward:
            motors.setSpeeds(200, 200);
            if (countDistance(targetDistance, encoder))
            {
                currentState = stopForward;
            }
            break;

        case stopForward:
            gyro.angle = 0;
            motors.setSpeeds(0, 0);
            if (drivingPatternTimer1.isFinished(stopDuration))
            {
                drivingPatternTimer1.reset();
                currentState = turnRight;
            }
            break;

        case turnRight:
            motors.setSpeeds(-100, 100);
            if (gyro.angle >= 90)
            {
                currentState = stopTurn;
                gyro.angle = 0;
            }
            break;

        case stopTurn:
            motors.setSpeeds(0, 0);

            if (drivingPatternTimer2.isFinished(stopDuration))
            {
                forwardCounter = forwardCounter + 1;
                drivingPatternTimer2.reset();
                currentState = moveForward;
            }
            break;

        default:
            break;
        }
    }
};

#endif