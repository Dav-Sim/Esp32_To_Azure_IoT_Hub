# Esp32 to Azure IoT Hub
ESP32 source code to send telemetric data to Azure IoT Hub and receive commands.

### Description
The following example shows an easy way to send telemetry data from ESP32 to the Azure IoT Hub and receiving commands from the Azure IoT Hub.

In main.cpp, there are two methods, setup and loop.

### Setup method
In setup method:
- Serial communication initialization
- Read configuration from EEPROM
- WiFi connection
- Connect to Azure IoT hub

### Loop method
In loop methodx, periodically:
- Compiles a report
- Sends to Azure IoT Hub
- Delay according to configuration

While the esp is waiting (delay in loop), it also is ready to receive a message from the Azure IoT Hub. If one arrives, it is processed by the method: **messageCallback (const char * payLoad, int size)**
in which the incoming message is parsed and evaluated.

### Message from the device
In our concept, the JSON message is in the form **{"temperature": value}**

### Message to device (configuration)
A message in the form "**CONFIG:123**" is expected where 123 is the value of the data transmission period in seconds.

### Configuration
The configuration is stored in the EEPROM of the device. The EEPROM.h library is used for this, for which a wrapper is written in the lib/Store/Store.h and .cpp

### MQTT connection to Azure
The connection to the Azure IoT hub is realized using the library: https://github.com/VSChina/ESP32_AzureIoT_Arduino 
