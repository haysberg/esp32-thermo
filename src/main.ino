#include "OneWire.h"
#include "DallasTemperature.h"
#include "ArduinoJson.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include "vars.h"
#include "OTA.h"

extern char *identifier_esp;
extern PubSubClient client;

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

  pinMode(2, OUTPUT);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  connect_wifi();

  server_init();
  
  // set server of our client
  client.setServer(mqtt_server, 1883);
  // char* monitor_topic = (identifier_esp).c_str();
  //mqtt_pubcallback(identifier_esp);
  client.connect("esp32", "user", "chiasse");
  client.setBufferSize(2048);
  //mqtt_mysubscribe((char *)(TOPIC_LED));
  client.subscribe(TOPIC_LED);
  // set callback when publishes arrive for the subscribed topic
  client.setCallback(mqtt_pubcallback); 
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

  serializeJson(jdoc, payload);
  if(MONITOR){
    Serial.println(payload);
  }
  
  
  /*--- subscribe to TOPIC_LED if not yet ! */
  if (!client.connected()) { 
    mqtt_mysubscribe((char *)(TOPIC_LED));
  }

  String tempString = get_temperature();
  // Serial info
  Serial.print("Published Temperature : "); Serial.println(tempString);
  // MQTT Publish
  //client.publish(TOPIC_TEMP, tempString.c_str());
  client.publish(TOPIC_JSON, String(payload).c_str());


  /* Process MQTT ... une fois par loop() ! */
  client.loop();
  sleep(MONITOR_SP);
}