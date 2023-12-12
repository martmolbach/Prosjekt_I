#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

struct DistanceSensor
{
    const int echoPin = 12;
    const int triggerPin = 11;
    long duration;
    long distanceInCm;

    const long pulseInterval = 15;

    void simplePulse()
    {
        digitalWrite(triggerPin, LOW);
        delayMicroseconds(5);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin, LOW);
    }

    void calculateDuration()
    {
        duration = pulseIn(echoPin, HIGH);
    }

    void calculateDistance()
    {
        distanceInCm = (duration / 2) / 29.1;
    }

    distanceSensorSetup()
    {
        pinMode(triggerPin, INPUT);
        pinMode(echoPin, OUTPUT);
    }
};

#endif