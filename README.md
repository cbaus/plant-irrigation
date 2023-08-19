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

* In Arduino IDE `-->` Preferences `-->` Addiontal boards maanager URLS `-->` add
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
* `-->` Boards `Node MCU 1.0 (ESP 12E)`

* Edit `credentials.h`
* Deploy code (Crtl + u)

## Web server
* You will need to find the IP address (it will print it in the Serial Monitor of the Arduino IDE or check the router)
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
* these pumps are not self-priming. They need to be fully submerged in water or at least water pressure on the inlet side (pre-fill hose)
* https://www.youtube.com/watch?v=FWvEEtrTGRQ
### Full parts list
* Chip and sensor (¥1100): https://www.amazon.co.jp/gp/product/B0B57H957B/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&psc=1
* Power (¥1399): https://www.amazon.co.jp/gp/product/B09R1NFVT1/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1
* Power 12V to 5V (¥871): https://www.amazon.co.jp/gp/product/B07MSBBZP9/ref=ppx_yo_dt_b_asin_title_o00_s01?ie=UTF8&psc=1
* Pump (¥1499): https://www.amazon.co.jp/gp/product/B07D29YT2C/ref=ppx_yo_dt_b_asin_title_o08_s00?ie=UTF8&th=1
* Breadboard (¥980): https://www.amazon.co.jp/gp/product/B081RG5P3S/ref=ppx_yo_dt_b_asin_title_o07_s00?ie=UTF8&psc=1
* Cables (¥949): https://www.amazon.co.jp/gp/product/B07MNSRX78/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&th=1
* Hose 8mm (¥1011): https://www.amazon.co.jp/gp/product/B01B2ORREO/ref=ppx_yo_dt_b_asin_title_o08_s00?ie=UTF8&th=1
* Y-connector - better get different ones (¥874): https://www.amazon.co.jp/gp/product/B097BBC57S/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1
* Elbow connector x 4 (¥700): https://www.amazon.co.jp/gp/product/B0BS614FVM/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1
* Manual valve (¥527): https://www.amazon.co.jp/gp/product/B019I059XK/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1
* 20 L tank (¥954): https://www.amazon.co.jp/gp/product/B099ZRSN6M/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1

**Total: ¥10,864**