#ifndef TIMER_H
#define TIMER_H

struct Timer
{
    unsigned long previousMillis = 0;
    int timePassedForValues = 0;
    int timePassedForAverage = 0;

    unsigned long startTime;
    
    void reset() {
        previousMillis = millis();
    }

    bool isFinished(unsigned long duration){
        return (millis() - previousMillis) > duration;
    }
};

#endif