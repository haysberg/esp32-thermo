
#include <WiFi.h>
#include <ESPmDNS.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// gestion du temps pour calcul de la durée de la MaJ
unsigned long otamillis;

void setupOTA(const char* nameprefix) {
  const int maxlen = 40;
  char fullhostname[maxlen];
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
  

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);
  
  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");
  snprintf(fullhostname, maxlen, "%s-%02x%02x%02x", nameprefix, mac[3], mac[4], mac[5]);
  ArduinoOTA.setHostname(fullhostname);
  
  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
  .onStart([]() {// lancé au début de la MaJ
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    otamillis=millis();
  })
  .onEnd([]() {// lancé en fin MaJ
    Serial.println("\nEnd");
    Serial.print((millis()-otamillis)/1000.0);
    Serial.println(" secondes");
  })
  .onProgress([](unsigned int progress, unsigned int total) {// lancé lors de la progression de la MaJ
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {// En cas d'erreur
    Serial.printf("Error[%u]: ", error);
    switch(error) {
      // erreur d'authentification, mauvais mot de passe OTA
      case OTA_AUTH_ERROR:     Serial.println("OTA_AUTH_ERROR: Auth Failed");
                               break;
      // erreur lors du démarrage de la MaJ (flash insuffisante)
      case OTA_BEGIN_ERROR:    Serial.println("OTA_BEGIN_ERROR: Begin Failed");
                               break;
      // impossible de se connecter à l'IDE Arduino
      case OTA_CONNECT_ERROR:  Serial.println("OTA_CONNECT_ERROR: Connect Failed");
                               break;
      // Erreur de réception des données
      case OTA_RECEIVE_ERROR:  Serial.println("OTA_RECEIVE_ERROR: Receive Failed");
                               break;
      // Erreur lors de la confirmation de MaJ
      case OTA_END_ERROR:      Serial.println("OTA_END_ERROR: Receive Failed");
                               break;
      // Erreur inconnue
      default:                 Serial.println("Erreur inconnue");
    }
  });

  // Activation fonctionnalité OTA
  ArduinoOTA.begin();

  Serial.println("OTA Initialized");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
