# Solis2MQTT ESP32 Project

This project connects an ESP32 to a Solis inverter using Modbus RTU over RS485 and publishes the inverter data to an MQTT broker.

## Configuration
This project uses [PlatformIO](https://docs.platformio.org/en/latest/) and targets by default the 'DOIT ESP32 DEVKIT V1'. This is defined in the platformio.ini file.

## Dependencies

The project uses the following libraries:
- PubSubClient
- ModbusMaster

These dependencies are specified in the platformio.ini file and they are automatically downloaded.


### WiFi and MQTT Configuration

WiFi and MQTT credentials are stored in the [`secrets.h`](command:_github.copilot.openSymbolFromReferences?%5B%22%22%2C%5B%7B%22uri%22%3A%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Finclude%2Fsecrets_example.h%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22pos%22%3A%7B%22line%22%3A2%2C%22character%22%3A64%7D%7D%5D%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "Go to definition") file. To create this file, start from the 

secrets_example.h file:

1. Uncomment and fill in the following fields with your credentials:
    ```cpp
    #define SECRET_SSID "your-ssid"
    #define SECRET_PASS "your-password"
    #define SECRET_MQTT_SERVER "your-mqtt-server"
    #define SECRET_MQTT_USER "your-mqtt-username"
    #define SECRET_MQTT_PASS "your-mqtt-password"
    ```
2. Rename the file to [`secrets.h`](command:_github.copilot.openSymbolFromReferences?%5B%22%22%2C%5B%7B%22uri%22%3A%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Finclude%2Fsecrets_example.h%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22pos%22%3A%7B%22line%22%3A2%2C%22character%22%3A64%7D%7D%5D%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "Go to definition").

### RS485 Pins

RS485 control pins are defined in 

main.cpp

:

```cpp
#define MAX485_DE 2
#define MAX485_RE_NEG 4
#define RX_PIN 16 // RX2
#define TX_PIN 17 // TX2
```

### Connector Pinout Diagram

```
  /-----\
  | 2 3 |
  | 1 4 |
  \--^--/
```

Source: [https://github.com/incub77/solis2mqtt](https://github.com/incub77/solis2mqtt)

## Sensor Data

Sensor data is defined in the [`Sensor`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22Sensor%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp") structure and initialized in main.cpp:

```cpp
struct Sensor {
  const char *name;
  const char *topic;
  uint16_t registerAddress;
  uint8_t functionCode;
  float scale;
  const char *unit;
};

Sensor sensors[] = {
    {"active_power", "inverter/active_power", 3004, 4, 1.0, "W"},
    {"inverter_temp", "inverter/inverter_temp", 3041, 4, 0.1, "°C"},
    // ...
};
```

The full sensor and register mapping has been taken from [here](https://github.com/hvoerman/solis2mqtt).

## Functions

### WiFi and MQTT Functions

- [`void connectWiFi()`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22void%20connectWiFi\(\)%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp"): Connects to the WiFi network.
- [`void reconnectMQTT()`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22void%20reconnectMQTT\(\)%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp"): Reconnects to the MQTT broker.
- [`void mqttCallback(char *topic, byte *payload, unsigned int length)`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22void%20mqttCallback\(char%20*topic%2C%20byte%20*payload%2C%20unsigned%20int%20length\)%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp"): Handles incoming MQTT messages.

### RS485 Control Functions

- [`void preTransmission()`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22void%20preTransmission\(\)%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp"): Pre-transmission control for RS485.
- [`void postTransmission()`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22void%20postTransmission\(\)%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp"): Post-transmission control for RS485.

### Modbus Functions

- [`bool getResultMsg(ModbusMaster *node, uint8_t result)`](command:_github.copilot.openSymbolInFile?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fdennis%2FProjects%2Fsolis2mqtt-esp32%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22bool%20getResultMsg\(ModbusMaster%20*node%2C%20uint8_t%20result\)%22%2C%22e5f1ff8c-7197-4c15-bb7c-3f9c18fcb697%22%5D "/home/dennis/Projects/solis2mqtt-esp32/src/main.cpp"): Processes Modbus results.

## Building and Uploading

This project uses PlatformIO. For instructions on how to set it up, please visit the [PlatformIO documentation](https://docs.platformio.org/en/latest/).

To build and upload the project to the ESP32, use the following PlatformIO commands:

```sh
pio run
pio run --target upload
```

## Acknowledgments

This project is based on the work from:

- [hvoerman/solis2mqtt](https://github.com/hvoerman/solis2mqtt).
- [bram2202/Ginlong-Solis-mqtt](https://github.com/bram2202/Ginlong-Solis-mqtt)
- [incub77/solis2mqtt](https://github.com/incub77/solis2mqtt)