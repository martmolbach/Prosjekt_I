#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

struct PressureSensor
{
    // Defines a pin for the pressuresensor;
    const int pressureSensorPin = A0;

    // Defines a holder variable for the value of the pressure sensor
    int pressureSensorValue;

    // Defines a threshold for the pressure sensor
    const int pressureThreshold = 500;

    // Analog reads the pressure sensor and returns the value
    int updatePressureValue()
    {
        int result = analogRead(pressureSensorPin);
        return result;
    }
    // Informs the user if the doormat if someone is standing on the doormat or not
    String pressureSensorThresholdString()
    {
        if (pressureSensorValue < pressureThreshold)
        {
            String printString = "Nobody is standing on the doormat :)";
            return printString;
        }
        else if (pressureSensorValue > pressureThreshold)
        {
            String printString1 = "Someone is standing on the doormat :)";
            return printString1;
        }
    }
    //Serial print function
    void pressureSensorSerialPrint()
    {
        Serial.print("Current sensor value : - ");
        Serial.println(pressureSensorValue);
        Serial.print(pressureSensorThresholdString());
    }
};

#endif