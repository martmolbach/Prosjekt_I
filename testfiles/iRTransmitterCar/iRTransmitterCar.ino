#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4IRPulses iRPulse;
Zumo32U4ProximitySensors proximitySensors;


void setup()
{
    proximitySensors.initThreeSensors();
    
}

void loop()
{
    proximitySensors.read();
}