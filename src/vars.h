#include "OneWire.h"
#include "DallasTemperature.h"
#include "ArduinoJson.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <PubSubClient.h>

OneWire oneWire(23);
DallasTemperature tempSensor(&oneWire);
const int GREENLED_PIN = 19;
const int REDLED_PIN = 18;
const int SEUIL_HAUT_JOUR = 28;
const int SEUIL_BAS_JOUR = 26;
const int SEUIL_HAUT_NUIT = SEUIL_HAUT_JOUR - 2;
const int SEUIL_BAS_NUIT = SEUIL_BAS_JOUR - 2;
int SEUIL_LUMINOSITE = 1000;
int LUMINOSITE = 0;
const int SEUIL_INCENDIE = 30;
bool incendie = false;
bool LED_INCENDIE = false;

char payload[512];
StaticJsonDocument<512> jdoc;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
bool MONITOR = true;

const char *ssid = "NIQUE TA MERE RIOT GAMES";
const char *password = "petitesalope";

String MONITOR_IP = "";
String MONITOR_PORT = "1880";
int MONITOR_SP = 1;

AsyncWebServer server(80);

File file;
char *identifier_esp = "esp_n1";


/*===== MQTT broker/server ========*/
const char* mqtt_server = "home.haysberg.io"; 

/*===== MQTT TOPICS ===============*/
#define TOPIC_TEMP "temperature"
#define TOPIC_LED  "control"
#define TOPIC_JSON "json"

/*===== ESP is MQTT Client =======*/
WiFiClient espClient;           // Wifi 
