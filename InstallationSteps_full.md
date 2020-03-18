# Sensor Installation

## Materials: 
* Sensor
* Tape
* 2 Twist-ties
* Bubble wrap (~ 5 x 12 cm)
* Aluminum foil (~ 5 x 12 cm)

<img src="/docs/InstallationMaterials.png" width=600>

## Directions:
1. Connect sensor to microcontroller (D32 Pro or D1 Mini).
2. Position sensor on stem (width < 1 cm). On each plant, choose locations of similar height and stem width. Install each sensor in the same orientation (an “E” shape) and facing the same direction.
3. Use tape to secure the sensor’s straight edge on the stem. The nodes should press firmly against the stem’s surface. 
4. Use 2 twist-ties to tighten and secure the sensor’s hold.
5. When secure in place, wrap piece of bubble wrap around the stem and tape in place. This helps buffer air temperature.
6. Wrap aluminum foil around the bubble wrap and tape in place. This helps buffer light; ensure there are no holes or openings where light can enter. 

<img src="/docs/InstallationSensor1.png" width=200> <img src="/docs/InstallationSensor2.png" width=200> <img src="/docs/InstallationSensor3.png" width=200> <img src="/docs/InstallationSensor4.png" width=200>

# Checklist for D32 Pro

## Board: [Arduino-ESP32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)
- [ ] Connect microcontroller via USB.
- [ ] In Arduino, open Preferences and copy release link to Additional Boards Manager URLs. Separate multiple URLs with commas. (Files > Preferences)
- [ ] Open Boards Manager, search "esp32" and install. (Tools > Board > Boards Manager)
- [ ] Switch board to LOLIN D32 PRO. (Tools > Board)

## Dependencies:
- [ ] In Library Manager, search and install the following libraries (Tools > Manage Libraries):
    - [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
    - [ArduinoJSON](https://arduinojson.org/) by Benoit Blanchon
    - [HDC2080 Arduino Library](https://github.com/lime-labs/HDC2080-Arduino) by Lime Labs
    - [PubSubClient](https://pubsubclient.knolleary.net/) by Nick O'Leary
    - [RTC by Makuna](https://github.com/Makuna/Rtc/wiki) by Michael C. Miller
- [ ] These optional libraries can be downloaded online and copied to Arduino Libraries folder in Finder. Comment out applicable lines in code if unused. (In Finder, Arduino > libraries)
    - [LOLIN E-Paper Display Library](https://github.com/wemos/LOLIN_EPD_Library)
    - [WEMOS SHT3x Arduino Library](https://github.com/wemos/WEMOS_SHT3x_Arduino_Library)
- [ ] NOTE: For PubSubClient, edit PubSubClient.h increase max packet size from 128b to 512b.
(In Finder, Arduino > libraries > PubSubClient > src > PubSubClient.h > Edit “#define MQTT_MAX_PACKET_SIZE 512”)

## Network settings:
- [ ] Open [D32 Pro code](https://github.com/dotmote/sapflow/blob/master/software/microcontroller/d32_pro/d32_pro_sapflow_mqtt_sd/d32_pro_sapflow_mqtt_sd.ino) in Arduino.
- [ ] ClientID: change `“clientID”`
- [ ] Server/broker IP address: change `“mqtt_server”`
- [ ] Connect to WiFi: enter `“wifi name”` and `“wifi password”`
- [ ] Verify/Compile and Upload to microcontroller via USB

## Check:
- [ ] Open Serial Monitor and check that data is incoming. (Tools > Serial Monitor)
- [ ] Check that Temperatures are not reading negative. If yes, try pressing Reset button on microcontroller several times.
- [ ] Check that data is also transmitting on Raspberry Pi.





# Checklist for D1 Mini

## Board: [Arduino-ESP8266](https://github.com/esp8266/Arduino)
- [ ] Connect microcontroller via USB.
- [ ] In Arduino, open Preferences and copy release link (https://arduino.esp8266.com/stable/package_esp8266com_index.json) to Additional Boards Manager URLs. Separate multiple URLs with commas. (Files > Preferences)
- [ ] Open Boards Manager, search "esp8266" and install. (Tools > Board > Boards Manager)
- [ ] Switch board to LOLIN (WEMOS) D1 mini Lite. (Tools > Board)

## Dependencies:
- [ ] In Library Manager, search and install the following libraries (Tools > Manage Libraries):
    - [ArduinoJSON](https://arduinojson.org/) by Benoit Blanchon
    - [HDC2080 Arduino Library](https://github.com/lime-labs/HDC2080-Arduino) by Lime Labs
    - [PubSubClient](https://pubsubclient.knolleary.net/) by Nick O'Leary
    - [WiFi Manager](https://github.com/tzapu/WiFiManager) by Tzapu
- [ ] This optional library can be downloaded online and copied to Arduino Libraries folder in Finder. Comment out applicable lines in code if unused. (In Finder, Arduino > libraries)
    - [WEMOS SHT3x Arduino Library](https://github.com/wemos/WEMOS_SHT3x_Arduino_Library)
- [ ] NOTE: For PubSubClient, edit PubSubClient.h increase max packet size from 128b to 512b.
(In Finder, Arduino > libraries > PubSubClient > src > PubSubClient.h > Edit “#define MQTT_MAX_PACKET_SIZE 512”)

## Network settings:
- [ ] Open [D1 Mini code](https://github.com/dotmote/sapflow/blob/master/software/microcontroller/d1_mini/d1_mini_sapflow_mqtt/d1_hdc2080_sapflow_mqtt.ino) in Arduino. 
- [ ] ClientID: change `“clientID”`
- [ ] Server/broker IP address: change `“mqtt_server”`
- [ ] Verify/Compile and Upload to microcontroller via USB
- [ ] Connect to WiFi: 
    - On phone, select WiFi network “AutoConnectAP”. 
    - On next screen, select “Configure WiFi”.
    - Select WiFi network, enter password, and select "Save".
    - Wait to connect. “AutoConnectAP” will disappear from network list when complete.
- [ ] NOTE: If "AutoConnectAP" network does not appear:
    - Press and hold Reset button on microcontroller for 5 seconds. 
    - In code, activate `//wifiManager.resetSettings();` to `wifiManager.resetSettings();`. Upload to microcontroller.
    - "AutoConnectAP" network should now appear. Connect through phone.
    - In code, re-comment out `wifiManager.resetSettings();` to `//wifiManager.resetSettings();`. Re-upload to microcontroller.

## Check:
- [ ] Open Serial Monitor and check that data is incoming. (Tools > Serial Monitor)
- [ ] Check that Temperatures are not reading negative. If yes, try pressing Reset button on microcontroller several times.
- [ ] Check that data is also transmitting on Raspberry Pi.
