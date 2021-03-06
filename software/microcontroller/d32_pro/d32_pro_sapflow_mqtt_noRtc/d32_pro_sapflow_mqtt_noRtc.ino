uint32_t rtcUnixTimestamp;

// Deep sleep
#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 1800     /* Time ESP32 will go to sleep (in seconds). 1800 seconds = 30 minutes */
#include "driver/adc.h"

// Wifi
#include <WiFi.h>

const char *ssid = "wifi name";
const char *password = "wifi password";

WiFiClient wifiClient;

// MQTT
// NOTE: PubSubClient by default allows for a max packet size of 128 bytes. Edit PubSubClient.h to increase the max to 512.
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <ArduinoJson.h>

const char *mqtt_server = "192.168.1.1"; // IP address of MQTT server/broker
const char *mqtt_topic = "sapflow";      // topic for MQTT messages
//const char* mqtt_username = "user"; // username for MQTT server/broker
//const char* mqtt_password = "pass"; //password for MQTT server/broker

PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

// NOTE: make sure that each clientID is unique. Your MQTT broker likely won't allow multiple clients
// to connect using the same clientID.
const char *clientID = "1";

// E-Paper Display
#include <LOLIN_EPD.h>
#include <Adafruit_GFX.h>

#define EPD_CS 14
#define EPD_DC 27
#define EPD_RST 33                                             // can set to -1 and share with microcontroller Reset! Must be set to pin 33 to work with lolin cable
#define EPD_BUSY -1                                            // can set to -1 to not use a pin (will wait a fixed delay)
LOLIN_IL3897 EPD(250, 122, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY); //hardware SPI

// temperature sensors
#include <HDC2080.h>

#define ADDR 0x40
#define ADDR2 0x41
HDC2080 sensor(ADDR);
HDC2080 sensor2(ADDR2);

#define HEAT_PIN_SWITCH 26

// SHT30 temp/humidity sensor
#include <WEMOS_SHT3X.h>

SHT3X sht30(0x45);

// Soil moisture sensor
#define SOIL_MOISTURE_PIN 34

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
float outsideTemp;
float outsideHumidity;
int soilMoisture;

long lastMqttReconnectAttempt = 0;
long lastWifiReconnectAttempt = 0;

boolean displayedWifiStatus = false;

boolean reconnect()
{
  if (client.connect(mqtt_topic))
  {
    Serial.println(F("Reconnected to MQTT broker!"));
    EPD.clearBuffer();
    EPD.print("Reconnected to MQTT broker on topic: ");
    EPD.println(mqtt_topic);
    EPD.display();
  }
  return client.connected();
}

void enterDeepSleep()
{
  Serial.println(F("Entering deep sleep"));
  adc_power_off();
  esp_deep_sleep_start();
}

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  print_wakeup_reason();

  EPD.begin();
  EPD.clearBuffer();
  EPD.fillScreen(EPD_WHITE);

  EPD.setCursor(10, 10);
  EPD.setTextColor(EPD_BLACK);
  EPD.setTextWrap(true);
  EPD.print("Connecting to ");
  EPD.print(ssid);
  EPD.println("...");
  EPD.display();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  displayedWifiStatus = true;

  EPD.clearBuffer();
  EPD.println("Wifi connected");
  EPD.println("IP address: ");
  EPD.println(WiFi.localIP());
  EPD.display();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  EPD.clearBuffer();
  EPD.println("Connecting to MQTT broker...");
  EPD.display();

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password
  // if (client.connect(clientID, mqtt_username, mqtt_password)) {
  if (client.connect(clientID))
  {
    EPD.clearBuffer();
    EPD.print("Connected to MQTT broker on topic: ");
    EPD.println(mqtt_topic);
    EPD.display();
    Serial.println("Connected to MQTT Broker!");
  }
  else
  {
    Serial.println("Connection to MQTT Broker failed...");
  }

  // Set heat pin as an output
  pinMode(HEAT_PIN_SWITCH, OUTPUT);
  // Set heat pin low to begin with
  digitalWrite(HEAT_PIN_SWITCH, LOW);

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
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!displayedWifiStatus)
    {
      displayedWifiStatus = true;
      EPD.clearBuffer();
      EPD.print("Reconnected to wifi network: ");
      EPD.println(ssid);
      EPD.display();
    }

    // if wifi connected, then check for MQTT connection and reconnect, if needed
    if (!client.connected())
    {
      long now = millis();
      if (now - lastMqttReconnectAttempt > 5000)
      {
        lastMqttReconnectAttempt = now;
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
  }
  else
  {
    if (millis() - lastWifiReconnectAttempt > 5000)
    {
      lastWifiReconnectAttempt = millis();
      if (displayedWifiStatus != false)
      {
        displayedWifiStatus = false;
        EPD.clearBuffer();
        EPD.print("Wifi connection down. Attempting to reconnect to: ");
        EPD.println(ssid);
        EPD.display();
      }

      int WLcount = 0;
      int UpCount = 0;

      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED && WLcount < 200)
      {
        delay(100);
        Serial.printf(".");

        if (UpCount >= 60) // keep from scrolling sideways forever
        {
          UpCount = 0;
          Serial.printf("\n");
        }
        ++WLcount;
      }
    }
  }

  currentMillis = millis();

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

  // uncomment to enter deep sleep after 3 minutes of operation
  //  if (millisSinceHeatPulse > 180000) {
  //    enterDeepSleep();
  //  }

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
    //    EPD.clearBuffer();
    //    EPD.setCursor(10, 10);
    //    EPD.setTextColor(EPD_BLACK);
    //    EPD.setTextWrap(true);
    //    EPD.print("temp1: ");
    //    EPD.println(temp1);
    //    EPD.print(" temp2: ");
    //    EPD.println(temp2);
    if (sht30.get() == 0)
    {
      outsideTemp = sht30.cTemp;
      outsideHumidity = sht30.humidity;
      Serial.print("Outside Temperature in Celsius : ");
      Serial.println(outsideTemp);
      Serial.print("Relative Humidity : ");
      Serial.println(outsideHumidity);
      Serial.println();
      //      EPD.print("Outside Temp: ");
      //      EPD.println(outsideTemp);
      //      EPD.print("Outside Humidity: ");
      //      EPD.println(outsideHumidity);
    }
    else
    {
      Serial.println("Error reading SHT30 sensor");
    }
    //    EPD.display();
    rtcUnixTimestamp = millis();
    sendTx(temp1, temp2, millisSinceHeatPulse, outsideTemp, outsideHumidity, soilMoisture, rtcUnixTimestamp, millisSinceReferenceTemp);
  }
}

void sendTx(float temp1, float temp2, unsigned long millisSinceHeatPulse, float outsideTemp, float outsideHumidity, int soilMoisture, uint32_t rtcUnixTimestamp, unsigned long millisSinceReferenceTemp)
{
  StaticJsonDocument<512> doc;

  doc["id"] = clientID;
  doc["rtcUnixTimestamp"] = rtcUnixTimestamp;
  doc["temp1"] = temp1;
  doc["temp2"] = temp2;
  doc["millisSinceHeatPulse"] = millisSinceHeatPulse;
  doc["outsideTemp"] = outsideTemp;
  doc["outsideHumidity"] = outsideHumidity;
  doc["soilMoisture"] = soilMoisture;
  doc["millisSinceReferenceTemp"] = millisSinceReferenceTemp;

  char buffer[512];

  size_t n = serializeJson(doc, buffer);
  client.publish(mqtt_topic, buffer, n);
  Serial.print("Sent mqtt message: ");
  Serial.println(buffer);
  Serial.println("To topic: ");
  Serial.println(mqtt_topic);
}
