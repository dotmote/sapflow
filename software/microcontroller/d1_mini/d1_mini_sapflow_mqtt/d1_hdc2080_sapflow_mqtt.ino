// SHT30 temp/humidity sensor
#include <WEMOS_SHT3X.h>
SHT3X sht30(0x45);

#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

// HDC2080 temperature/humidity sensor
#include <HDC2080.h>

//needed for WiFiManager library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <ArduinoJson.h>  // Use version 6.0+

// Node Configuration Data

// NOTE: make sure that each clientID is unique. Your MQTT broker likely won't allow multiple clients
// to connect using the same clientID.
const char *clientID = "1";
const char *mqtt_server = "192.168.1.1"; // IP address of MQTT server/broker
const char *mqtt_topic = "sapflow";      // topic for MQTT messages
//const char* mqtt_username = "user"; // username for MQTT server/broker
//const char* mqtt_password = "pass"; //password for MQTT server/broker

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker
// NOTE: PubSubClient by default allows for a max packet size of 128 bytes. Edit PubSubClient.h to increase the max to 512.

#define ADDR 0x40
#define ADDR2 0x41
HDC2080 sensor(ADDR);
HDC2080 sensor2(ADDR2);

#define HEAT_PIN_SWITCH D8

// Soil moisture sensor
#define SOIL_MOISTURE_PIN A0

unsigned long tempSensorTimer = 0;

//Using millis instead of delay
unsigned long currentMillis;
unsigned long previousMillis = 0;

//Counter for millis since heat pulse was fired
unsigned long millisSinceHeatPulse = 0;

//Internal counter for starting/stopping heat pulse
unsigned long millisStartHeatPulse = 0;

//Millis value for last time heater was turned on
unsigned long previousHeaterOnTime = 0;

//Counter for millis since we started tracking reference temperatures
unsigned long millisStartReferenceTemp = 0;

unsigned long millisSinceReferenceTemp = 0;

boolean referenceTempRecorded = false;

float temp1;
float temp2;
float outsideTemp = -1.0;
float outsideHumidity = -1.0;
int soilMoisture = -1;

long lastMqttReconnectAttempt = 0;

boolean reconnect()
{
  if (client.connect(mqtt_topic))
  {
    Serial.println(F("Reconnected to MQTT broker!"));
  }
  return client.connected();
}

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;

  //WiFiManager
  //Local initialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //NOTE: Activate when connecting to new WiFi network. Upload to microcontroller, connect through AutoConnectAP on phone,
  // then re-comment out and re-upload to microcontroller.
  //wifiManager.resetSettings();

  //set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  //if you get here you have connected to the WiFi
  Serial.println("connected to wifi");

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID))
  {
    Serial.println("Connected to MQTT Broker!");
  }
  else
  {
    Serial.println("Connection to MQTT Broker failed...");
  }

  // Set heat pin low to begin with
  pinMode(HEAT_PIN_SWITCH, OUTPUT);
  digitalWrite(HEAT_PIN_SWITCH, LOW);

  //Enable onboard LED
  pinMode(2, OUTPUT);

  // Initialize I2C communication
  sensor.begin();
  sensor2.begin();

  // Begin with a device reset
  sensor.reset();
  sensor2.reset();

  // Configure Measurements
  sensor.setMeasurementMode(TEMP_AND_HUMID); // Set measurements to temperature and humidity
  sensor2.setMeasurementMode(TEMP_AND_HUMID);
  sensor.setRate(ONE_HZ); // Set measurement frequency to 1 Hz
  sensor2.setRate(ONE_HZ);
  sensor.setTempRes(FOURTEEN_BIT);
  sensor2.setTempRes(FOURTEEN_BIT);
  sensor.setHumidRes(FOURTEEN_BIT);
  sensor2.setHumidRes(FOURTEEN_BIT);

  //begin measuring
  sensor.triggerMeasurement();
  sensor2.triggerMeasurement();
}

void loop()
{

  currentMillis = millis();

  //check for MQTT connection and reconnect, if needed
  if (!client.connected())
  {
    if (currentMillis - lastMqttReconnectAttempt > 5000)
    {
      lastMqttReconnectAttempt = currentMillis;
      // Attempt to reconnect
      if (reconnect())
      {
        lastMqttReconnectAttempt = 0;
      }
    }
  }
  else
  {
    // MQTT client connected
    client.loop();
  }

  //start reading temperature for baseline reference check
  if (digitalRead(HEAT_PIN_SWITCH) == LOW && millisStartReferenceTemp == 0)
  {
    Serial.println(F("Starting to read reference temperatures..."));
    millisStartReferenceTemp = currentMillis;
  }

  if (digitalRead(HEAT_PIN_SWITCH) == LOW && currentMillis - millisStartReferenceTemp >= 10000 && previousHeaterOnTime == 0)
  {
    previousMillis = currentMillis;
    millisStartHeatPulse = currentMillis;
    Serial.println(F("Done reading reference temperatures. Turning heating element on."));
    digitalWrite(HEAT_PIN_SWITCH, HIGH);
    Serial.println(F("Heater ON"));
    previousHeaterOnTime = currentMillis;
  }
  //turn on heating element every 30 minutes
  else if (digitalRead(HEAT_PIN_SWITCH) == LOW && currentMillis - millisStartReferenceTemp >= 1800000)
  {
    previousMillis = 0;
    previousHeaterOnTime = 0;
    millisStartReferenceTemp = 0;
  }

  //turn off heating element after it has been on for >= 20 seconds
  if (digitalRead(HEAT_PIN_SWITCH) == HIGH && currentMillis - millisStartHeatPulse >= 20000)
  {
    digitalWrite(HEAT_PIN_SWITCH, LOW);
    Serial.println(F("Heater OFF"));
  }

  millisSinceHeatPulse = currentMillis - previousHeaterOnTime;
  millisSinceReferenceTemp = currentMillis - millisStartReferenceTemp;

  if (millis() - tempSensorTimer >= 1000)
  {
    tempSensorTimer = millis();
    temp1 = sensor.readTemp();
    Serial.print("Sensor 1 Temperature (C): ");
    Serial.print(temp1);
    temp2 = sensor2.readTemp();
    Serial.print(" Sensor 2 Temperature (C): ");
    Serial.print(temp2);
    Serial.print(F(" clientID: "));
    Serial.println(clientID);
    Serial.print(F("millisSinceHeatPulse: "));
    Serial.println(millisSinceHeatPulse);
    soilMoisture = analogRead(SOIL_MOISTURE_PIN);
    Serial.print(F("Soil Moisture: "));
    Serial.println(soilMoisture);
    if (sht30.get() == 0)
    {
      outsideTemp = sht30.cTemp;
      outsideHumidity = sht30.humidity;
      Serial.print("Outside Temperature in Celsius : ");
      Serial.println(outsideTemp);
      Serial.print("Relative Humidity : ");
      Serial.println(outsideHumidity);
      Serial.println();
    }
    else
    {
      Serial.println("Error retrieving ambient temp/humidity readings from SHT30");
    }
    sendTx(temp1, temp2, millisSinceHeatPulse, outsideTemp, outsideHumidity, soilMoisture, millisSinceReferenceTemp);
  }
}

void sendTx(float temp1, float temp2, unsigned long millisSinceHeatPulse, float outsideTemp, float outsideHumidity, int soilMoisture, unsigned long millisSinceReferenceTemp)
{
  StaticJsonDocument<512> doc;

  doc["id"] = clientID;
  doc["temp1"] = temp1;
  doc["temp2"] = temp2;
  doc["millisSinceHeatPulse"] = millisSinceHeatPulse;
  doc["soilMoisture"] = soilMoisture;
  doc["outsideTemp"] = outsideTemp;
  doc["outsideHumidity"] = outsideHumidity;
  doc["millisSinceReferenceTemp"] = millisSinceReferenceTemp;

  char buffer[512];

  size_t n = serializeJson(doc, buffer);
  client.publish(mqtt_topic, buffer, n);
  Serial.print("Sent mqtt message: ");
  Serial.println(buffer);
  Serial.println("To topic: ");
  Serial.println(mqtt_topic);
}
