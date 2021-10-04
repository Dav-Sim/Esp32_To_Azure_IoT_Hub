
// name: ***DEVICENAME***
// primary: HostName=***HUBNAME***.azure-devices.net;DeviceId=***DEVICENAME***;SharedAccessKey=***SASKEY***

// azure monitor
// az iot hub monitor-events --hub-name ***HUBNAME***

//Azure Lib
//https://github.com/VSChina/ESP32_AzureIoT_Arduino

//NOTE - REPLACE ALL ***...*** WITH YOUR DATA

//Include libs
#include <Arduino.h>
#include <WiFi.h>
#include "Esp32MQTTClient.h"
#include <Store.h>

//Declare constants
const char *AP_SSID = "***WIFIID***";
const char *AP_PASS = "***WIFIKEY***";
const char *connectionString = "HostName=***HUBNAME***.azure-devices.net;DeviceId=***DEVICENAME***;SharedAccessKey=***SASKEY***";

//Declare global variables
int testNumber = 0;
bool hasIoTHub = false;
Store EEPROM_Store;
uint32_t delayMs = 10000;
uint32_t timeNow = 0;

//Message from Azure
static void messageCallback(const char *payLoad, int size)
{
    Serial.println("Received message from Azure:");
    Serial.println(payLoad);
    //Expected "CONFIG:100000" //number is delay [seconds] between each meassures (valid values 1-100000 (from 1sec to more than 24hours))
    String s = String(payLoad);
    if (size > 7 && s.startsWith("CONFIG"))
    {
        String checkStr = s.substring(0, 6);
        String delayStr = s.substring(7, s.length());
        uint32_t newDelay = strtoul(delayStr.c_str(), NULL, 10) * 1000;
        if (newDelay >= 1000 && newDelay <= 100000000){
            Serial.println("Received new configuration with delay of " + String(newDelay) + " miliseconds");
            EEPROM_Store.setValueToEEPROM(newDelay);
            delayMs = newDelay;
        }
        else{
            Serial.println("Received dely in config is not valid");
        }
    }
    else
    {
        Serial.println("Received message from Azure is not in expected format!");
    }
}

//Confirmation from Azure
static void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
    switch (result)
    {
    case IOTHUB_CLIENT_CONFIRMATION_OK:
        Serial.println("Confirmation OK.");
        break;
    case IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY:
        Serial.println("Confirmation DESTROY.");
        break;
    case IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT:
        Serial.println("Confirmation TIMEOUT.");
        break;
    case IOTHUB_CLIENT_CONFIRMATION_ERROR:
        Serial.println("Confirmation ERROR.");
        break;
    default:
        Serial.println("Confirmation UNKNOWN.");
        break;
    }
}

//Setup
void setup()
{
    //Init Serial
    Serial.begin(9600);
    while (!Serial)
    {
        yield();
    }
    delay(2000);
    //Serial.setDebugOutput(true);

    //Read from EEPROM
    delayMs = EEPROM_Store.getValueFromEEPROM(delayMs);

    //Init WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Wifi connected");

    //Connect to Azure IoT Hub
    Serial.print("Connecting to Azure...");
    if (!Esp32MQTTClient_Init((const uint8_t *)connectionString))
    {
        hasIoTHub = false;
        Serial.println("Initializing IoT hub failed.");
        return;
    }
    Esp32MQTTClient_SetMessageCallback(messageCallback);
    Esp32MQTTClient_SetSendConfirmationCallback(sendConfirmationCallback);
    hasIoTHub = true;
    Serial.println("Initializing IoT hub success.");
}

//Loop
void loop()
{
    
    testNumber++;
    Serial.println("Loop " + String(testNumber) + "... Start sending event.");
    if (hasIoTHub)
    {
        char buff[128];
        String s = "{\"temperature\":\"" + String(testNumber) + "\"}";
        snprintf(buff, 128, s.c_str());
        //send data to Azure
        if (Esp32MQTTClient_SendEvent(buff))
        {
            Serial.println("Sending data succeed");
        }
        else
        {
            Serial.println("Sending data failed...");
        }
    }

    Serial.println("Going to sleep for " + String(delayMs) + " miliseconds....");
    timeNow = millis();
    while(millis() < timeNow + delayMs){
        delay(500);
        //check for incoming messages
        Esp32MQTTClient_Check();
    }
    Serial.println("Wake up....");
}
