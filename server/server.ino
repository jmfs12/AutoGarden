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

  Serial.println("");

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid_sta, password_sta);
  Serial.println("Conecting to extern wifi");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConected. IP: " + WiFi.localIP().toString());

  bool ap = WiFi.softAP(ssid_ap,password_ap);
  if(ap){
    Serial.println("Access point initiated.");
  }
  else{
    Serial.println("Fail to init AP");
  }
  
  delay(1000);

  server.begin();
}

void loop() {
  
  WiFiClient client = server.available();

  if(client){
    Serial.println("Client connected");

    while(client.connected()){

      if(client.available()){
        String id = client.readStringUntil(' ');
        String value = client.readStringUntil('\n');
        value.trim();

        Serial.println("sid antes: " + id);
        int sid = id.toInt();
        int val = value.toInt();

        if(sid == 0){
          int i = 0;
          int tmp;
          do{
            EEPROM.get(i, tmp);
            i++;
          }while(tmp != -1 && i < 512);
          EEPROM.put(i, i);
          EEPROM.commit();

          sid = i;
          Serial.print("sid depois: ");
          Serial.println(sid);
          client.println(sid);

          if(value.length()>0){
            if(val == 0 && value != "0"){
              Serial.println("Invalid value");
            }
            else{
              Serial.print("Value: ");
              Serial.println(val);
              client.println("Value received succesfully");
              dict[sid] = val;
            }
          }
          else{
            Serial.println("Empty value");
          }
        }
        else{
          int s; EEPROM.get(sid, s);
          Serial.print("ID: ");
          Serial.println(s);
          if(s == sid){
            if(value.length()>0){
              if(val == 0 && value != "0"){
                Serial.println("Invalid value");
              }
              else{
                Serial.print("Value: ");
                Serial.println(val);
                client.println("Value received succesfully");
                dict[sid] = val;
              }
            }
            else{
              Serial.println("Empty value");
            }
          }
        }

      }
    }
    //client.stop();
    //Serial.println("Client disconnected");
  }
  
}
