#ifndef SPEEDOMETERVARIABLES_H
#define SPEEDOMETERVARIABLES_H
#include <Zumo32U4Encoders.h>

struct Speedometer
{

    const float countPerRotation = 75.81 * 12;
    const float wheelDiameter = 3.75;
    const float maxSpeed = 64;

    unsigned long encoderCount = 0;
    float rotationCount = 0;
    float previousRotationCount = 0;

    bool showingValues = true;

    float cmPerSecond = 0;
    float rotationsPerSecond = 0;
    float distanceTraveled = 0;
    float highestSpeed = 0;
    int percentageOfMaxSpeed = 0;
    int secondsOverPercentage = 0;
    float averageSpeed = 0;
    float distanceTraveledLast60Seconds = 0;

    int calculateEncoderCount(Zumo32U4Encoders &encoder)
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
};

enum DisplayState
{
    Normal,
    Average
};

#endif