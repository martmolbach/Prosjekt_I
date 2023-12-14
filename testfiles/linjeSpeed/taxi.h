#ifndef TAXI_H
#define TAXI_H

#include <Arduino.h>

struct Taxi
{
    // PSUEDO CODE

    void startTaxi()
    {
        if (batteryLevel == randomInt(30, 100)) // IF THE BATTERY IS EQUAL TO THE RANDOM INT WITHIN BATTERY RANGE
        {
            driveState = TAXIMODE;
        }
    }

    ved neste kryss stop plukk opp
        ved neste neste kryss stop sett av

        void
        taxiMode()
    {
        if (rightCrossDetected)
        {
            setOffPassenger == true;
        }
    }
};

#endif