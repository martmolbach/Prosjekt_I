#ifndef SPEEDOMETERVARIABLES_H
#define SPEEDOMETERVARIABLES_H

const float countPerRotation = 75.81 * 12;
const float wheelDiameter = 3.75;
const float maxSpeed = 64;

unsigned long encoderCount = 0;
float rotationCount = 0;
float previousRotationCount = 0;

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

int timePassedForValues = 0;
int timePassedForAverage = 0;

bool showingValues = true;

float cmPerSecond = 0;
float rotationsPerSecond = 0;
float distanceTraveled = 0;
float highestSpeed = 0;
int percentageOfMaxSpeed = 0;
int secondsOverPercentage = 0;
float averageSpeed = 0;
unsigned long previousMillis3 = 0;
float distanceTraveledLast60Seconds = 0;

enum displayState
{
    Normal,
    Average
};

#endif