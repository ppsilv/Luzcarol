
#include "ESP8266mDNS.h"
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
 
//const char* ssid = "OpenSoftware2";
//const char* password = "";
const int ESP_BUILTIN_LED = 1; 
const int timeout=1000;
unsigned long timer;
int led_status;




void setupOta() {
  //Serial.println("Booting");
  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //  Serial.println("Connection Failed! Rebooting...");
  //  delay(5000);
  //  ESP.restart();
  //}
 
  ArduinoOTA.setHostname("espota");
  // No authentication by default
  // ArduinoOTA.setPassword("admin");
 
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
 
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready OTA ESP8266 - for Open Software Ltda(C)2018");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(ESP_BUILTIN_LED, OUTPUT);
  timer=millis();
  led_status=LOW;
}


 
void loopOta() {
  ArduinoOTA.handle();

  if ( (timer+timeout) < millis() ){
     timer=millis();
     if ( led_status == LOW  ){
        led_status = HIGH;
     }else{
        led_status = LOW;
     }
  }
  digitalWrite(ESP_BUILTIN_LED, led_status);

}
