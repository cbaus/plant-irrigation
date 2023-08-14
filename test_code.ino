#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "credentials.h"

ESP8266WebServer server(80);
String header;

bool paused = true;
bool pumpRunState = false;
const int waterPumpPort = D0;
const int moisturePort = D1;
unsigned long runTime = 0;  //overflow after ~50 days, ignored here
unsigned long stateChangeTime = 0;
unsigned long totalPumpTime_ms = 0;
unsigned long pumpUntil_ms = -1;
double availableWaterInL = 10;
const double L_per_s = 0.052;
unsigned long lastWatering_ms = -1;
const unsigned long wateringInterval_h = 3;  //3h
const double wateringAmountPerDay_L = 1;
const double wateringAmountPerTime_L = wateringAmountPerDay_L / 24 * double(wateringInterval_h) * 2;  //at max can do twice the amount each time
double wateringAmountDoneToday_L = 0;
const unsigned long msInDay = 1000 * 60 * 60 * 24;
int currentDay = 0;

String logFilePath = "/log_1.txt";

String getTimeStr(unsigned long runTime) {
  char buffer[11];
  //Serial.println("asd " + String(runTime) + " " + String( runTime%3600000));
  unsigned long runTime_d = runTime / (3600000 * 24);
  unsigned long runTime_h = (runTime % (3600000 * 24)) / 3600000;
  unsigned long runTime_min = (runTime % 3600000) / 60000;
  unsigned long runTime_s = (runTime % 60000) / 1000;
  sprintf(buffer, "%02d:%02d:%02d:%02d", runTime_d, runTime_h, runTime_min, runTime_s);
  return String(buffer);
}

void log(const String& logText) {
  File file = SPIFFS.open(logFilePath, "a");
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  if (file.size() > 128 * 1024 * 1024) {
    Serial.println("Switching to second log file and truncating");
    file.close();
    logFilePath = logFilePath == "/log_1.txt" ? "/log_2.txt" : "/log_1.txt";
    File file = SPIFFS.open(logFilePath, "w");
    if (!file) {
      Serial.println("Error opening file for writing");
      return;
    }
  }
  auto runTime_str = getTimeStr(runTime);
  file.println(runTime_str + logText);
  file.close();
}

void pumpOn() {
    Serial.println("#Turning pump on");
    digitalWrite(waterPumpPort, HIGH);
    pumpRunState = true;
    stateChangeTime = runTime;
    double maxPumpTime_s = min(min(availableWaterInL / L_per_s, wateringAmountPerTime_L / L_per_s), (wateringAmountPerDay_L - wateringAmountDoneToday_L) / L_per_s);
    Serial.println("Pump time: " + String(maxPumpTime_s) + " s");
    pumpUntil_ms = stateChangeTime + long(maxPumpTime_s * 1000);
    Serial.println("Pump until time: " + String(pumpUntil_ms) + " ms");
}

void pumpOff() {
    digitalWrite(waterPumpPort, LOW);
    pumpRunState = false;
    unsigned long pumpTime_ms = runTime - stateChangeTime;
    totalPumpTime_ms += pumpTime_ms;
    auto pumpTime_str = String(pumpTime_ms / 1000);
    auto totalPumpTime_str = String(totalPumpTime_ms / 1000);
    availableWaterInL -= double(pumpTime_ms) * L_per_s / 1000.;
    wateringAmountDoneToday_L += double(pumpTime_ms) * L_per_s / 1000.;
    lastWatering_ms = runTime;
    log("Turning pump off after " + pumpTime_str + "s (total: " + totalPumpTime_str + "s)");
    Serial.println("#Turning pump off after " + pumpTime_str + "s (total: " + totalPumpTime_str + "s)");
}

void setup() {
  Serial.begin(115200);
  pinMode(waterPumpPort, OUTPUT);

  bool success = SPIFFS.begin();
  if (success) {
    Serial.println("File system mounted with success");
  } else {
    Serial.println("Error mounting the file system");
    return;
  }

  Serial.print("Connecting to " + String(WIFI_SSID) + " .");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handle_OnConnect);
  server.on("/download", handle_Download);
  server.on("/pause", handle_Pause);
  //server.on("/someaction", handle_action);
  server.onNotFound(handle_NotFound);
  server.begin();

  log("Settings: " + String(wateringAmountPerDay_L) + " | " + String(wateringAmountPerTime_L) + " | " + String(L_per_s) + " | " + " | " + String(availableWaterInL) + " | ");
}

void sendHTML() {
  int waterLevelVal = 1024 - analogRead(A0);
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 Irrigation System</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP8266 Irrigation System</h1>";
  ptr += "<p>Day: " + String(currentDay) + "</p>";
  ptr += "<p>Time: " + getTimeStr(runTime) + "</p>";
  ptr += "<p>Time (last irrigation): " + getTimeStr(lastWatering_ms) + "</p>";
  ptr += "<p>Available water: " + String(availableWaterInL) + " L</p>";
  ptr += "<p>Watering amount done today: " + String(wateringAmountDoneToday_L) + " L / " + String(wateringAmountPerDay_L) + " L</p>";
  ptr += "<p>Water level value: " + String(waterLevelVal) + "</p>";
  ptr += "<p>Pump status: " + String(pumpRunState ? "on" : "off") + "</p>";
  ptr +="<a class=\"button button-" + String(paused?"on":"off") + "\" href=\"/pause\">" + String(paused?"START":"PAUSE") + "</a>\n";
  ptr +="<a class=\"button\" href=\"/download\">Download Log File</a>\n";
  ptr += "</body></html>";
  server.send(200, "text/html", ptr);
}

void handle_OnConnect() {
  sendHTML();
}

void handle_Pause() {
  paused = !paused;
  if(pumpRunState) {
    pumpOff();
  }
  sendHTML();
}

void handle_Download() {
  File download = SPIFFS.open(logFilePath, "r");
  if (download) {
    server.sendHeader("Content-Type", "text/text");
    server.sendHeader("Content-Disposition", "attachment; filename=log.txt");
    server.sendHeader("Connection", "close");
    server.streamFile(download, "application/octet-stream");
    download.close();
  } else {
    server.send(400, "text/plain", "Log file not found");
  }
}
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void loop() {
  runTime = millis();
  int day = runTime / msInDay;
  if (day > currentDay) {
    currentDay = day;
    log("Today is Day " + String(day));
    Serial.println("Today is Day " + String(day));
    wateringAmountDoneToday_L = 0;
  }

  server.handleClient();

  if(paused) {
    delay(1000);
    return;
  }

  int waterLevelVal = 1024 - analogRead(A0);
  int moistureVal = !digitalRead(moisturePort);
  Serial.println("WaterLevel:" + String(waterLevelVal));
  Serial.println("Moisture:" + String(moistureVal));
  Serial.println("AvailableWater:" + String(availableWaterInL));


  //turn pump on
  if (
    !pumpRunState
    && availableWaterInL > 0
    && waterLevelVal < 50
    && wateringAmountDoneToday_L < wateringAmountPerDay_L
    && (lastWatering_ms == -1 || runTime > lastWatering_ms + wateringInterval_h * 3600000)) {
    log("Turning pump on. WaterLevel:" + String(waterLevelVal) + " Moisture:" + String(moistureVal) + " AvailableWater:" + String(availableWaterInL));
    pumpOn();
  }

  //turn pump off
  if (pumpRunState && (runTime > pumpUntil_ms || waterLevelVal > 100)) {
    pumpOff();
  }
  delay(1000);
}