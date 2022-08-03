#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"

#include <ModbusMaster.h>

#define MAX485_DE      2
#define MAX485_RE_NEG  4

#define Slave_ID       1
#define RX_PIN         16 //RX2 
#define TX_PIN         17  //TX2 

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// instantiate ModbusMaster object
ModbusMaster modbus;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
  delay(15); //delay to increase reliability
}

void postTransmission()
{
  delay(15); //delay to increase reliability
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);

  //Serial initialize
  Serial.begin(9600, SERIAL_8N1);
  Serial.println("Started");

  //Wifi setup
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  // Modbus communication runs at 9600 baud
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  modbus.begin(Slave_ID, Serial2);
  
  // Callbacks allow us to configure the RS485 transceiver correctly
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
}

bool state = true;

void loop()
{
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  uint8_t result = modbus.readInputRegisters(3021, 2); //test
  if (getResultMsg(&modbus, result)) 
    {
      Serial.println();
      float res_dbl = modbus.getResponseBuffer(0) / 10;
      String res = "DC Voltage: " + String(res_dbl) + " V\r\n";
      ThingSpeak.setField(1, res_dbl);
      res_dbl = modbus.getResponseBuffer(1) / 10;
      res += "DC Current: " + String(res_dbl) + "A";
      ThingSpeak.setField(2, res_dbl);
      Serial.println(res);
    }

  //ThingSpeak.setStatus(String("Status!")); //update status

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  delay(2000);
}

bool getResultMsg(ModbusMaster *node, uint8_t result) 
{
  String tmpstr2 = "\r\n";
  switch (result) 
  {
  case node->ku8MBSuccess:
    return true;
    break;
  case node->ku8MBIllegalFunction:
    tmpstr2 += "Illegal Function";
    break;
  case node->ku8MBIllegalDataAddress:
    tmpstr2 += "Illegal Data Address";
    break;
  case node->ku8MBIllegalDataValue:
    tmpstr2 += "Illegal Data Value";
    break;
  case node->ku8MBSlaveDeviceFailure:
    tmpstr2 += "Slave Device Failure";
    break;
  case node->ku8MBInvalidSlaveID:
    tmpstr2 += "Invalid Slave ID";
    break;
  case node->ku8MBInvalidFunction:
    tmpstr2 += "Invalid Function";
    break;
  case node->ku8MBResponseTimedOut:
    tmpstr2 += "Response Timed Out";
    break;
  case node->ku8MBInvalidCRC:
    tmpstr2 += "Invalid CRC";
    break;
  default:
    tmpstr2 += "Unknown error: " + String(result);
    break;
  }
  Serial.println(tmpstr2);
  return false;
}
