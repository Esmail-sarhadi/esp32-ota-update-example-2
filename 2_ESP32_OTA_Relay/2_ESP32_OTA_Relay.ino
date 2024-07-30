#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "HUAWEI";
const char* password = "sam6612599";

const int pinRelay = 5;
unsigned long previousMillis = 0;
const long interval = 1000;
int relayState = LOW;
//***************************************************************************************************
void setup() {
  Serial.begin(115200);
  pinMode(pinRelay, OUTPUT);

  Serial.println(">> Booting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(">> Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  //Port defaults to 3232
  //ArduinoOTA.setPort(3232);

  //Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("MyESP32");

  //No authentication by default
  ArduinoOTA.setPassword("admin");

  //Password can be set with it's md5 value as well
  //MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else                                            //U_SPIFFS
      type = "filesystem";

    //NOTE: If updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println(">> Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\n >> End.");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf(">> Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf(">> Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(">> Auth Failed.");
    else if (error == OTA_BEGIN_ERROR) Serial.println(">> Begin Failed.");
    else if (error == OTA_CONNECT_ERROR) Serial.println(">> Connect Failed.");
    else if (error == OTA_RECEIVE_ERROR) Serial.println(">> Receive Failed.");
    else if (error == OTA_END_ERROR) Serial.println(">> End Failed.");
  });

  ArduinoOTA.begin();

  Serial.println(">> Ready.");
  Serial.print(">> IP address: ");
  Serial.println(WiFi.localIP());
}
//***************************************************************************************************
void loop() {
  ArduinoOTA.handle();

  //In this program, we have not used delay() for changing relay mode, because ESP32 pauses your program during the delay().
  //If next OTA request is generated while Arduino is paused waiting for the delay() to pass, your program will miss that request.

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    relayState = not(relayState);
    digitalWrite(pinRelay,  relayState);
  }
}
//***************************************************************************************************
