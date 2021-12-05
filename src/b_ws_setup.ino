#include "ArduinoJson.h"
#include <HTTPClient.h>

HTTPClient http;

extern StaticJsonDocument<512> jdoc;
extern char payload[512];

//Cette fonction renvoie un 404 tout simple en cas de pépin
void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

//Cette fonction envoie les données POST au cas où on a renseigné une IP
void sendData(String payload)
{
    if (MONITOR_IP != "")
    {
        http.begin("http://" + MONITOR_IP + ":" + MONITOR_PORT + "/esp"); //On envoi vers l'ip target à l'url /esp
        http.addHeader("Content-Type", "application/json");               //On précise le type
        int httpResponseCode = http.POST(payload);                        //On envoi le json
        http.end();
    }
}


//Fonction de processing de la template HTML
String processor(const String &var)
{
    if (var == "TEMPERATURE")
    {
        return get_temperature();
    }
    else if (var == "LIGHT")
    {
        Serial.println(get_light());
        return get_light();
    }
    else if (var == "SSID")
    {
        return get_ssid();
    }
    else if (var == "MAC")
    {
        return get_mac();
    }
    else if (var == "IP")
    {
        return get_ip();
    }
    else if (var == "COOLER")
    {
        return get_cooler();
    }
    else if (var == "HEATER")
    {
        return get_heater();
    }
    else if (var == "HEATER")
    {
        return get_heater();
    }
    else if (var == "LT")
    {
        return get_lt();
    }
    else if (var == "SBJ")
    {
        return get_sbj();
    }
    else if (var == "SHJ")
    {
        return get_shj();
    }
    else if (var == "SBN")
    {
        return get_sbn();
    }
    else if (var == "SHN")
    {
        return get_shn();
    }

    return String();
}

//Cette fonction initie tout le serveur web
void server_init()
{
    // Declaring root handler, and action to be taken when root is requested
    auto root_handler = server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                                  {
                                      /* This handler will download statut.html (SPIFFS file) and will send it back */
                                      request->send(SPIFFS, "/statut.html", String(), false, processor);
                                  });

    //Cette fonction gère le path /target.
    //Si on lui fournit un paramètre, il le change, sinon on désactive le monitoring
    server.on("/target", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  bool change = false;
                  if (request->hasArg("ip"))
                  {
                      change = true;
                      MONITOR_IP = request->getParam("ip")->value();
                  }
                  if (request->hasArg("port"))
                  {
                      change = true;
                      MONITOR_PORT = request->getParam("port")->value();
                  }
                  if (request->hasArg("sp"))
                  {
                      change = true;
                      MONITOR_SP = request->getParam("sp")->value().toInt();
                  }

                  MONITOR = change;

                  request->send(200, "text/json", payload);
              });

    //Si on veut récupérer une valeur en particulier, on fait une requete GET
    //Par ex : pour récupérer la température, on fait une requête sur /value/temperature
    server.on("^\\/value\\/([a-z_-]+)$", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  String value = request->pathArg(0);
                  if (jdoc[value])
                  {
                      StaticJsonDocument<512> tmp_jdoc;
                      char tmp_payload[512];
                      tmp_jdoc[value] = jdoc[value];
                      serializeJson(tmp_jdoc, tmp_payload);
                      request->send(200, "text/json", tmp_payload);
                  }
                  else
                  {
                      request->send(404, "text/plain", "Not found");
                  }
              });

    // Le path /set permet de modifier certaines variables de l'ESP directement
    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  /* A route with a side effect : this get request has a param and should     
     *  set a new light_threshold ... used for regulation !
     */
                  if (request->hasArg("seuil_luminosite"))
                  {
                      SEUIL_LUMINOSITE = atoi(request->arg("seuil_luminosite").c_str());
                      request->send_P(200, "text/plain", "Threshold Set !");
                  }
                  else
                  {
                      request->send_P(404, "text/plain", "Argument not found !");
                  }
              });

    server.onNotFound(notFound);
    server.begin();
}