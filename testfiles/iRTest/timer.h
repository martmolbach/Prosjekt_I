#ifndef TIMER_H
#define TIMER_H

struct Timer
{
    unsigned long previousMillis = 0;
    unsigned long previousMicros = 0;
    int timePassedForValues = 0;
    int timePassedForAverage = 0;

    void reset()
    {
        previousMillis = millis();
    }

    void resetMicros()
    {
        previousMicros = micros();
    }

    bool isFinished(unsigned long duration)
    {
        return (millis() - previousMillis) > duration;
    }

    bool isFinishedMicros(unsigned long duration)
    {
        return (micros() - previousMicros) > duration;
    }
};

#endif