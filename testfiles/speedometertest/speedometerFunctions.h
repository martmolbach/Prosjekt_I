#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "speedometerVariables.h"
#include <Zumo32U4Encoders.h>
Zumo32U4Encoders encoder;

int calculateEncoderCount()
{
    int leftEncoder = encoder.getCountsAndResetLeft();
    int rightEncoder = encoder.getCountsAndResetRight();
    int result = (abs(leftEncoder) + abs(rightEncoder)) / 2;
    return result;
}

void resetAverageValues()
{
    highestSpeed = 0;
    secondsOverPercentage = 0;
    averageSpeed = 0;
}

float calculateRotation()
{
    float rotationCounter = encoderCount / countPerRotation;
    return rotationCounter;
}

float calculateRotationPerSecond()
{
    return rotationCount - previousRotationCount;
}

float calculateCmPerSecond()
{
    return 2 * PI * (wheelDiameter / 2) * rotationsPerSecond;
}

float calculateDistanceTraveled()
{
    return rotationCount * (PI * wheelDiameter) / 100;
}

float calculateAverageSpeed()
{
    return (distanceTraveledLast60Seconds * 100) / 60;
}

float updateHighestSpeed()
{
    if (highestSpeed < cmPerSecond)
    {
        return cmPerSecond;
    }
}

int calculatePercentageOfMaxSpeed()
{
    if (cmPerSecond > 0.7 * maxSpeed)
    {
        return 1;
    }
    return 0;
}
#endif