#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "speedometerVariables.h"

float calculateRotation()
{
    float rotationCounter = leftEncoderCount / countPerRotation;
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