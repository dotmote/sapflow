# Sensor Installation

## Materials: 
* Sensor
* Tape
* 2 Twist-ties
* Bubble wrap (~ 5 x 12 cm)
* Aluminum foil (~ 5 x 12 cm)

## Directions:
1. Connect sensor to board (D32 Pro or D1 Mini).
2. Position sensor on stem (width < 1 cm). On each plant, choose locations of similar height and stem width. Install each sensor in the same orientation (an “E” shape) and facing the same direction.
3. Use tape to secure the sensor’s straight edge on the stem. The nodes should press firmly against the stem’s surface. 
4. Use 2 twist-ties to tighten and secure the sensor’s hold.
5. When secure in place, wrap piece of bubble wrap around the stem and tape in place. This helps buffer air temperature.
6. Wrap aluminum foil around the bubble wrap and tape in place. This helps buffer light; ensure there are no holes or openings where light can enter. 

<img src="/docs/SensorInstall1.png" width=400> <img src="/docs/SensorInstall2.png" width=400> 
<img src="/docs/SensorInstall3.png" width=400> <img src="/docs/SensorInstall4.png" width=400>

# Checklist for D32 Pro

## Board:
- [ ] Connect microcontroller via USB.
- [ ] Install [Arduino-ESP32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md). 
- [ ] In Arduino, open Preferences and copy release link to Additional Boards Manager URLs.
- [ ] Switch board to LOLIN D32 PRO. (Tools > Board)

## Dependencies:
- [ ] Install [dependencies](https://github.com/dotmote/sapflow/blob/master/software/microcontroller/d32_pro/d32.md): online or search in Library Manager. (Tools > Manage Libraries)
- [ ] For PubSubClient, increase max packet size from 128b to 512b
(In Finder, Arduino > libraries > PubSubClient > src > PubSubClient.h > Edit “#define MQTT_MAX_PACKET_SIZE 512”)

## Network settings:
- [ ] Open [D32 Pro code](https://github.com/dotmote/sapflow/blob/master/software/microcontroller/d32_pro/d32_pro_sapflow_mqtt_sd/d32_pro_sapflow_mqtt_sd.ino) in Arduino.
- [ ] ClientID: change “clientID”
- [ ] Server/broker IP address: change “mqtt_server”
- [ ] Connect to WiFi: enter “wifi name” and “wifi password”
- [ ] Verify/Compile
- [ ] Upload to microcontroller via USB


## Check:
- [ ] Open Serial Monitor and check that data is incoming. (Tools > Serial Monitor)
- [ ] Check that Temperatures are not reading negative. If yes, try pressing Reset button on microcontroller several times.
- [ ] Check that data is also transmitting on Raspberry Pi.





# Checklist for D1 Mini

## Board:
- [ ] Connect microcontroller via USB.
- [ ] Install [Arduino-ESP8266](https://github.com/esp8266/Arduino). 
- [ ] In Arduino, open Preferences and copy release link to Additional Boards Manager URLs.
- [ ] Switch board to LOLIN (WEMOS) D1 mini Lite. (Tools > Board)

## Dependencies:
- [ ] Install [dependencies](https://github.com/dotmote/sapflow/blob/master/software/microcontroller/d1_mini/d1.md): online or search in Library Manager (Tools > Manage Libraries)
- [ ] For PubSubClient, increase max packet size from 128b to 512b
(In Finder, Arduino > libraries > PubSubClient > src > PubSubClient.h > Edit “#define MQTT_MAX_PACKET_SIZE 512”)

## Network settings:
- [ ] Open [D1 Mini code](https://github.com/dotmote/sapflow/blob/master/software/microcontroller/d1_mini/d1_mini_sapflow_mqtt/d1_hdc2080_sapflow_mqtt.ino) in Arduino. 
- [ ] ClientID: change “clientID”
- [ ] Server/broker IP address: change “mqtt_server”
- [ ] Connect to WiFi: 
    - On phone, select WiFi network “AutoConnectAP”. 
    - On next screen, select “Configure WiFi”.
    - Select WiFi network and enter password.
    - Wait to connect. “AutoConnectAP” will disappear from network list.
- [ ] Verify/Compile
- [ ] Upload to microcontroller via USB

## Check:
- [ ] Open Serial Monitor and check that data is incoming. (Tools > Serial Monitor)
- [ ] Check that Temperatures are not reading negative. If yes, try pressing Reset button on microcontroller several times.
- [ ] Check that data is also transmitting on Raspberry Pi.
