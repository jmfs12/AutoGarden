#include <WiFi.h>
#include <EEPROM.h>
#include <map>

const char* ssid_sta = "jmfs";
const char* password_sta = "jmfs13052022";

const char* ssid_ap = "ESP32-AP";
const char* password_ap = "12345678";

std::map<int,int> dict;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  EEPROM.begin(512);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid_sta, password_sta);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  bool ap = WiFi.softAP(ssid_ap,password_ap);

  delay(1000);

  server.begin();
  Serial.println("Server criado");

}

void loop() {

  WiFiClient client = server.available();

  if(client){

    while(client.connected()){

      if(client.available()){
        Serial.println("Client conectado");

        String id = client.readStringUntil(' ');
        String value = client.readStringUntil('\n');
        value.trim();

        int sid = id.toInt();
        int val = value.toInt();

        if(sid == 0){

          int i = 300;
          int tmp;

          do{
            EEPROM.get(i, tmp);
            i++;
          }while(tmp != -1 && i < 512);

          EEPROM.put(i, i);
          EEPROM.commit();

          sid = i;
          client.println(sid);

          if(value.length()>0 && val == 0 && value != "0"){

            Serial.println("enviando resposta");
            client.println("T");
            dict[sid] = val;

          } else{

            client.println("F");
          }

        }

        else{

          int s; EEPROM.get(sid, s);

          if(s == sid){

            if(value.length()>0 && val == 0 && value != "0"){

              Serial.println("enviando resposta");
              client.println("T");
              dict[sid] = val;

            } else{

              client.println("F");
            }

          }

        }

      }

    }
    client.stop();
  }

}