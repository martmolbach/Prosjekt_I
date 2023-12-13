#ifndef TIMER_H
#define TIMER_H

struct Timer
{
    // Variable to store the previous milliseconds
    unsigned long previousMillis = 0;

    //Variable to store the previous microseconds
    unsigned long previousMicros = 0;

    // Reset previousMillis 
    void reset()
    {
        previousMillis = millis();
    }
    // Reset previousMicros
    void resetMicros()
    {
        previousMicros = micros();
    }

    // Checks if the time passed is longer than desired duration (millis)
    bool isFinished(unsigned long duration)
    {
        return (millis() - previousMillis) > duration;
    }

    // Checks if the time passed is longer than desired duration (micros)
    bool isFinishedMicros(unsigned long duration)
    {
        return (micros() - previousMicros) > duration;
    }
};

#endif