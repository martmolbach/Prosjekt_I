#ifndef EEPROMSETUP_H
#define EEPROMSETUP_H

#include <EEPROM.h>

struct EepromSetup
{
    const int batteryHealthAddress = 0;
    const int batteryLevelAddress = 1;
    const int moneyAddress = 2;

    int batteryLevel = 0;
    int batteryHealth = 0;
    int money = 0;

    void eepromReader()
    {
        batteryHealth = EEPROM.read(batteryHealthAddress);
        batteryLevel = EEPROM.read(batteryLevelAddress);
        money = EEPROM.read(moneyAddress);
    }

    void eepromValueSetup()
    {
        EEPROM.write(batteryHealthAddress, 100);
        EEPROM.write(batteryLevelAddress, 100);
        EEPROM.write(moneyAddress, 70);
    }
};

#endif