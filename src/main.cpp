#include <WiFi.h>
#include "secrets.h"
#include <PubSubClient.h>
#include <ModbusMaster.h>

// RS485 Pins
#define MAX485_DE 2
#define MAX485_RE_NEG 4
#define RX_PIN 16 // RX2
#define TX_PIN 17 // TX2

#define Slave_ID 1

// WiFi and MQTT Configuration
char ssid[] = SECRET_SSID; // Network SSID
char pass[] = SECRET_PASS; // Network password
WiFiClient espClient;
PubSubClient client(espClient);
const char *mqtt_server = SECRET_MQTT_SERVER; // MQTT broker address
const int mqtt_port = 1883;                   // MQTT port
const char *mqtt_user = SECRET_MQTT_USER;     // MQTT username
const char *mqtt_pass = SECRET_MQTT_PASS;     // MQTT password

// MQTT Topics
const char *mqtt_topic_status = "inverter/status";

// Define a structure for sensors
struct Sensor
{
  const char *name;
  const char *topic;
  uint16_t registerAddress;
  uint8_t functionCode;
  float scale;
  const char *unit;
};

// Source: https://github.com/hvoerman/solis2mqtt/blob/master/solis_modbus.yaml
Sensor sensors[] = {
    {"active_power", "inverter/active_power", 3004, 4, 1.0, "W"},
    {"inverter_temp", "inverter/inverter_temp", 3041, 4, 0.1, "°C"},
    {"total_power", "inverter/total_power", 3008, 4, 1.0, "kWh"},
    {"generation_today", "inverter/generation_today", 3014, 4, 0.1, "kWh"},
    {"generation_yesterday", "inverter/generation_yesterday", 3015, 4, 0.1, "kWh"},
    {"total_dc_output_power", "inverter/total_dc_output_power", 3006, 4, 1.0, "W"},
    {"energy_this_month", "inverter/energy_this_month", 3010, 4, 1.0, "kWh"},
    {"generation_last_month", "inverter/generation_last_month", 3012, 4, 1.0, "kWh"},
    {"generation_this_year", "inverter/generation_this_year", 3016, 4, 1.0, "kWh"},
    {"generation_last_year", "inverter/generation_last_year", 3018, 4, 1.0, "kWh"},
    {"v_dc1", "inverter/v_dc1", 3021, 4, 0.1, "V"},
    {"i_dc1", "inverter/i_dc1", 3022, 4, 0.1, "A"},
    {"v_ac", "inverter/v_ac", 3035, 4, 0.1, "V"},
    {"i_ac", "inverter/i_ac", 3038, 4, 0.1, "A"},
    {"grid_frequency", "inverter/grid_frequency", 3042, 4, 0.01, "Hz"}};

// Modbus instance
ModbusMaster modbus;

// Function prototypes
void connectWiFi();
void reconnectMQTT();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void preTransmission();
void postTransmission();
bool getResultMsg(ModbusMaster *node, uint8_t result);

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);

  Serial.begin(9600);
  Serial.println("Initializing...");

  // WiFi setup
  WiFi.mode(WIFI_STA);
  connectWiFi();

  // Modbus and MQTT setup
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  modbus.begin(Slave_ID, Serial2);
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop()
{
  // Reconnect WiFi or MQTT if disconnected
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }
  if (!client.connected())
  {
    reconnectMQTT();
  }
  client.loop();

  bool got_errors = false;

  // Iterate over sensors and read/publish data
  for (Sensor &sensor : sensors)
  {
    uint8_t result = modbus.readInputRegisters(sensor.registerAddress, 1);
    if (result == modbus.ku8MBSuccess)
    {
      uint16_t rawValue = modbus.getResponseBuffer(0);
      float scaledValue = rawValue * sensor.scale;

      char valueStr[10];
      dtostrf(scaledValue, 6, 2, valueStr);

      // Print to serial
      Serial.print(sensor.name);
      Serial.print(": ");
      Serial.print(valueStr);
      Serial.print(" ");
      Serial.println(sensor.unit);

      // Publish to MQTT
      client.publish(sensor.topic, valueStr);
    }
    else
    {
      got_errors = true;
      Serial.print("Error reading ");
      Serial.println(sensor.name);
      break;
    }
    delay(100); // Small delay between reads
  }

  if (got_errors)
  {
    client.publish("inverter/status", "err");
  }
  else
  {

    client.publish("inverter/status", "ok");
  }

  delay(2000); // Wait before the next loop
}

// RS485 control functions
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
  delay(15);
}

void postTransmission()
{
  delay(15);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void connectWiFi()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.println("WiFi connected.");
}

void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass))
    {
      Serial.println("MQTT connected.");
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  // Handle incoming MQTT messages if needed
}
