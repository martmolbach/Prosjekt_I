#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

struct PressureSensor
{
    const int pressureSensorPin = A0;
    int pressureSensorValue;

    const int pressureThreshold0 = 500;


    int updatePressureValue()
    {
        int result = analogRead(pressureSensorPin);
        return result;
    }

    String pressureSensorThresholdString()
    {
        if (pressureSensorValue < pressureThreshold0)
        {
            String printString = "Nobody is standing on the doormat :)";
            return printString;
        }
        else if (pressureSensorValue > pressureThreshold0)
        {
            String printString1 = "Someone is standing on the doormat :)";
            return printString1;
        }
    }

    void pressureSensorSerialPrint()
    {
        Serial.print("Current sensor value : - ");
        Serial.println(pressureSensorValue);
        Serial.print(pressureSensorThresholdString());
    }
};

#endif