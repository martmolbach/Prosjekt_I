#include <Zumo32U4.h>

Zumo32U4Encoders encoder;

float encoderCount = 0;
float countPerRotation = 909.7;
float cmPerSecond = 0;
float rotationsPerSecond = 0;
const float wheelDiameter = 3.75;
unsigned long previousMillis = 0;
float rotationCount = 0;
float previousRotationCount = 0;
int absLeftEncoder = 0;
int absRightEncoder = 0;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    Serial.println(cmPerSecond);
    unsigned long currentMillis = millis();
    int leftEncoder = encoder.getCountsAndResetLeft();
    int rightEncoder = encoder.getCountsAndResetRight();
    absLeftEncoder += abs(leftEncoder);
    absRightEncoder += abs(rightEncoder);
    // encoderCount global variabel
    encoderCount = (absLeftEncoder + absRightEncoder) / 2;

    // rotationCounter global variabel
    rotationCount = encoderCount / countPerRotation;
    // cmPerSecond, wheelDiameter, rotation
    cmPerSecond = 2 * PI * (wheelDiameter / 2) * rotationsPerSecond;

    if (currentMillis - previousMillis >= 100)
    {
        // previousMillis global variabel
        previousMillis = currentMillis;
        // rotationsPerSecond global
        rotationsPerSecond = (rotationCount - previousRotationCount) * 10;
        // previousRotationCount global
        previousRotationCount = rotationCount;
    }
}