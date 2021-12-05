#include "OneWire.h"
#include "DallasTemperature.h"
#include "ArduinoJson.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include "vars.h"
#include "OTA.h"

void setup(void)
{
  Serial.begin(9600);
  // Install OTA
  //setupOTA("ESP OTA Ready");

  tempSensor.begin();
  ledcAttachPin(GREENLED_PIN, 0);
  ledcSetup(0, 5000, 12);

  ledcAttachPin(REDLED_PIN, 1);
  ledcSetup(1, 5000, 12);


  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  server_init();
}

void loop(void)
{
  act();
  react();

  while (Serial.available() > 0)
  {
    String receivedStr = Serial.readStringUntil('\n');
    if (receivedStr.indexOf("stop") >= 0)
    {
      MONITOR = false;
    }
    else if (receivedStr.indexOf("start") >= 0)
    {
      MONITOR = true;
    }
  }

  if(MONITOR){
    serializeJson(jdoc, payload);
    Serial.println(payload);
  }
  sleep(MONITOR_SP);
}