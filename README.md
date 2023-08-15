# IoT Plant Irrigation
Code for ESP8266 irrigation system

## Setup
* Please use Arduino IDE
* Install driver
```
git clone https://github.com/juliagoda/CH341SER.git
make
sudo make load
```

* In Arduino IDE --> Preferences --> Addiontal boards maanager URLS --> add
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
--> Boards `Node MCU 1.0 (ESP 12)`

* Edit `credentials.h`
* Deploy code (Crtl + u)

## Web server
* You will need to find the IP (it will print it in the Serial Monitor of the Arduino IDE or check the router)
* https://lastminuteengineers.com/creating-esp8266-web-server-arduino-ide/
* Go to http://192.168.0.2 or whatever IP your device got assigned

## Components
### Chip
* ESP8266-CH340
* https://www.instructables.com/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/
### Soil moisture sensor
* Soil and humidity sensor (FC-28) x 1 
* https://jp-phoenix.blogspot.com/2022/06/blog-post_27.html
### Pump
* Type: LEDGLE Submersible Pump, DC 12 V
* top plug is outlet
* needs to be drained
* https://www.youtube.com/watch?v=FWvEEtrTGRQ