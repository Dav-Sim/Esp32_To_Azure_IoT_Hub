#ifndef Store_h
#define Store_h

#include <Arduino.h>
#include <EEPROM.h>

class Store
{
public:
    uint32_t getValueFromEEPROM(uint32_t defaultValue = 10000);
    void setValueToEEPROM(uint32_t value);

private:
    bool isInitialized = false;
    void init();
};

#endif