#include "OneWire.h"
#include "DallasTemperature.h"
#include "ArduinoJson.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>

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

char payload[512];
StaticJsonDocument<512> jdoc;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
bool MONITOR = true;

//wifiMulti.addAP("HUAWEI-6EC2", "FGY9MLBL");
 //wifiMulti.addAP("HUAWEI-553A", "QTM06RTT");
const char *ssid = "Baguette";
const char *password = "petitpoucet";

String MONITOR_IP = "";
String MONITOR_PORT = "1880";
int MONITOR_SP = 2;

AsyncWebServer server(80);

File file;