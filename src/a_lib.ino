#include "WString.h"
#include "DallasTemperature.h"
#include <lwip/ip_addr.h>
#include "ArduinoJson.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>

//Les variables sont importées depuis vars.h
extern bool GREEN_LED_STATE;
extern bool RED_LED_STATE;
extern DallasTemperature tempSensor;
extern const int SEUIL_HAUT_JOUR;
extern const int SEUIL_BAS_JOUR;
extern const int SEUIL_HAUT_NUIT;
extern const int SEUIL_BAS_NUIT;
extern int SEUIL_LUMINOSITE;
extern char payload[512];
extern StaticJsonDocument<512> jdoc;
extern bool MONITOR;
extern int LUMINOSITE;
extern String MONITOR_IP;
extern String MONITOR_PORT;
extern int MONITOR_SP;

extern const int SEUIL_INCENDIE;
extern bool incendie;

WiFiMulti wifiMulti;
IPAddress ip;

PubSubClient client(espClient); // MQTT client

//Fonction pour renvoyer un string avec la température
String get_temperature()
{
  tempSensor.requestTemperaturesByIndex(0);
  return String(tempSensor.getTempCByIndex(0));
}

//Fonction pour récupérer la valeur actuelle de la luminosité
String get_light()
{
  //Parfois, la valeur donnée par le capture de luminosité est null
  //Pour éviter ça, on redonne la valeur précédente si nécessaire
  if (analogRead(A0) == NULL)
  {
    return String(LUMINOSITE);
  }
  LUMINOSITE = analogRead(A0);
  return String(LUMINOSITE);
}

//On renvoie l'adresse MAC
String get_mac()
{
  return String(WiFi.macAddress());
}

//Fonction pour renvoyée l'IP sous forme de string formatée
String get_ip()
{
  ip = WiFi.localIP();
  return String(ip[0]) + String(".") +
         String(ip[1]) + String(".") +
         String(ip[2]) + String(".") +
         String(ip[3]);
}

//On renvoie le SSID
String get_ssid()
{
  return String(WiFi.SSID());
}

//Si le ventilateur est allumé, on renvoie ON, sinon OFF
String get_cooler()
{
  if (GREEN_LED_STATE)
  {
    return String("ON");
  }
  return String("OFF");
}

//Si le radiateur est allumé, on renvoie ON, sinon OFF
String get_heater()
{
  if (RED_LED_STATE)
  {
    return String("ON");
  }
  return String("OFF");
}

//Renvoie seuil_bas_jour
String get_sbj()
{
  Serial.println(SEUIL_BAS_JOUR);
  return String(SEUIL_BAS_JOUR);
}

//Renvoie seuil_haut_jour
String get_shj()
{
  return String(SEUIL_HAUT_JOUR);
}

//Renvoie seuil_bas_nuit
String get_sbn()
{
  return String(SEUIL_BAS_NUIT);
}

//Renvoie seuil_haut_nuit
String get_shn()
{
  return String(SEUIL_HAUT_NUIT);
}

//Renvoie le seuil de luminosité. J'ai laissé le nom get_lt pour la compatibilité
String get_lt()
{
  return String(SEUIL_LUMINOSITE);
}

//Cette fonction permet de changer les variables globales qui dirigent
//l'état des LEDs. Cela permet de séparer l'édition des valeurs de l'ESP
//de la réaction de l'ESP dans la vraie vie.
void led_state(bool green, bool red)
{
  GREEN_LED_STATE = green;
  RED_LED_STATE = red;
}

//La fonction renvoie "OFF" si on ne monitor pas l'ESP
String get_monitor()
{
  if (MONITOR)
  {
    return String("ON");
  }
  return String("OFF");
}

//Cette fonction permet de mettre à jour le doc json
//en fonction des valeurs à l'intérieur de l'ESP
void generate_state_json()
{
  jdoc["mac"] = get_mac();
  jdoc["ip"] = get_ip();
  jdoc["ssid"] = get_ssid();
  jdoc["greenled_pin"] = GREENLED_PIN;
  jdoc["redled_pin"] = REDLED_PIN;
  jdoc["seuil_haut_jour"] = SEUIL_HAUT_JOUR;
  jdoc["seuil_bas_jour"] = SEUIL_BAS_JOUR;
  jdoc["seuil_haut_nuit"] = SEUIL_HAUT_NUIT;
  jdoc["seuil_bas_nuit"] = SEUIL_BAS_NUIT;
  jdoc["seuil_luminosite"] = SEUIL_LUMINOSITE;
  jdoc["cooler"] = get_cooler();
  jdoc["heater"] = get_heater();
  jdoc["monitor"] = get_monitor();
  jdoc["temperature"] = get_temperature();
  jdoc["light"] = get_light();
  jdoc["monitor_ip"] = MONITOR_IP;
  jdoc["monitor_port"] = MONITOR_PORT;
  jdoc["monitor_sp"] = MONITOR_SP;
}

//Cette fonction envoie une requête POST
void send_post_request()
{
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(MONITOR_IP, MONITOR_PORT.toInt(), String("/esp"));

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Data to send with HTTP POST
  String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";
  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  // If you need an HTTP request with a content type: application/json, use the following:
  http.addHeader("Content-Type", "application/json");
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  http.end();
}

//React to the global variables changed in act()
void react(void)
{
  if (GREEN_LED_STATE)
  {
    digitalWrite(0, 4095);
  }
  else
  {
    digitalWrite(0, 0);
  }

  if (RED_LED_STATE)
  {
    digitalWrite(1, 4095);
  }
  else
  {
    digitalWrite(1, 0);
  }

  sendData(payload);

  if (incendie)
  {
    LED_INCENDIE = !LED_INCENDIE;
  }else{
    LED_INCENDIE = false;
  }

  if (LED_INCENDIE)
  {
    digitalWrite(2, LOW);
  }
  else
  {
    digitalWrite(2, HIGH);
  }
}

//Change the state of the global variables
void act(void)
{
  if (MONITOR)
  {
    if (analogRead(A0) > SEUIL_LUMINOSITE)
    {
      if (tempSensor.getTempCByIndex(0) < SEUIL_BAS_JOUR)
      {
        led_state(false, true);
      }
      else if (tempSensor.getTempCByIndex(0) > SEUIL_BAS_JOUR && tempSensor.getTempCByIndex(0) < SEUIL_HAUT_JOUR)
      {
        led_state(false, false);
      }
      else if (tempSensor.getTempCByIndex(0) > SEUIL_HAUT_JOUR)
      {
        led_state(true, false);
      }
    }
    else
    {
      if (tempSensor.getTempCByIndex(0) < SEUIL_BAS_NUIT)
      {
        led_state(false, true);
      }
      else if (tempSensor.getTempCByIndex(0) > SEUIL_BAS_NUIT && tempSensor.getTempCByIndex(0) < SEUIL_HAUT_NUIT)
      {
        led_state(false, false);
      }
      else if (tempSensor.getTempCByIndex(0) > SEUIL_HAUT_NUIT)
      {
        led_state(true, false);
      }
    }
  }

  generate_state_json();

  if (tempSensor.getTempCByIndex(0) > SEUIL_INCENDIE)
  {
    incendie = true;
  }
}

/*============== CALLBACK ===================*/
void mqtt_pubcallback(char *topic,
                      byte *message,
                      unsigned int length)
{
  /* 
   * Callback if a message is published on this topic.
   */
  Serial.print("Message arrived on topic : ");
  Serial.println(topic);
  Serial.print("=> ");

  // Byte list to String and print to Serial
  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic,
  // you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == TOPIC_LED)
  {
    if (messageTemp == "ON")
    {
      incendie = true;
      LED_INCENDIE = true;
    }
    else if (messageTemp == "OFF")
    {
      incendie = false;
      LED_INCENDIE = false;
    }
  }
}

/*============= SUBSCRIBE =====================*/
void mqtt_mysubscribe(char *topic)
{
  /*
   * Subscribe to a MQTT topic 
   */
  while (!client.connected())
  { // Loop until we're reconnected

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect => https://pubsubclient.knolleary.net/api
    if (client.connect("esp32", /* Client Id when connecting to the server */
                       "user",  /* No credential */
                       "chiasse"))
    {
      Serial.println("connected");
      // then Subscribe topic
      client.subscribe(topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());

      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

/*------------------------*/
void connect_wifi()
{
  //  Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  // and disconnect from an AP if
  // it was previously connected
  WiFi.disconnect();
  delay(100); // ms

  Serial.println(String("\nAttempting to connect to SSIDs : "));
  wifiMulti.addAP("HUAWEI-6EC2", "FGY9MLBL");
  wifiMulti.addAP("HUAWEI-553A", "QTM06RTT");
  wifiMulti.addAP("NIQUE TA MERE RIOT GAMES", "petitesalope");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  if (wifiMulti.run() == WL_CONNECTED)
  {
    Serial.print("\nWiFi connected : \n");
  }
}

void callback_fn()
{
}