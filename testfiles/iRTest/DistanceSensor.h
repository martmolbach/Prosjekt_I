#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

struct DistanceSensor
{
    const int echoPin = 12;    // Define pin for echo
    const int triggerPin = 11; // Define pin for trigger
    long duration;             // Duration variable holder
    long distanceInCm;         // Distance variable holder

    const long pulseInterval = 15;

    // Function for a simple pulse, only use this for testing
    // (DO NOT USE WILL BLOCK CODE)
    void simplePulse()
    {
        digitalWrite(triggerPin, LOW);
        delayMicroseconds(5);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin, LOW);
    }

    // Calculates the duration of the pulse
    void calculateDuration()
    {
        duration = pulseIn(echoPin, HIGH);
    }
    // Calculates the distance to the object
    void calculateDistance()
    {
        distanceInCm = (duration / 2) / 29.1;
    }
    // Setup function for the pinModes
    distanceSensorSetup()
    {
        pinMode(triggerPin, INPUT);
        pinMode(echoPin, OUTPUT);
    }
};

#endif