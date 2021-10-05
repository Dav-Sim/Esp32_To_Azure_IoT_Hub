#include <Arduino.h>
#include <EEPROM.h>
#include "Store.h"

void Store::init()
{
    EEPROM.begin(5);
    isInitialized = true;
    Serial.println("EEPROM is initialized");
}

uint32_t Store::getValueFromEEPROM(uint32_t defaultValue)
{
    if (isInitialized == false)
        init();

    uint8_t check = EEPROM.readByte(0);
    uint32_t value = EEPROM.readUInt(1);

    Serial.println("EEPROM value: " + String(value) + " Flag :" + String(check));

    if (check != 1)
    {
        EEPROM.writeUInt(1, defaultValue);
        EEPROM.writeByte(0, 1);
        EEPROM.commit();
        Serial.println("EEPROM is empty, setting default value: " + String(defaultValue));
        value = defaultValue;
    }

    return value;
}

void Store::setValueToEEPROM(uint32_t value)
{
    if (isInitialized == false)
        init();

    //Read before write to minimize rewrite cycles
    uint8_t check = EEPROM.readByte(0);
    uint32_t currentValue = EEPROM.readUInt(1);

    if (check != 1 || currentValue != value)
    {
        EEPROM.writeByte(0, 1);
        EEPROM.writeUInt(1, value);
        EEPROM.commit();
        Serial.println("Setting value to EEPROM: " + String(value));
    }
    else
    {
        Serial.println("Value already saved in EEPROM: " + String(value));
    }
}
