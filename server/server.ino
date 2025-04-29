#include <WiFi.h>
#include <EEPROM.h>
#include <map>

const char* ssid_sta = "jmfs";
const char* password_sta = "jmfs13052022";

const char* ssid_ap = "ESP32-AP";
const char* password_ap = "12345678";

std::map<int,int> dict;

WiFiServer server(80);
unsigned long start;

void setup() {

  EEPROM.begin(512);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid_sta, password_sta);

  start = millis();
  while(WiFi.status() != WL_CONNECTED){
    
    while(millis() - start < 500){}

  }

  bool ap = WiFi.softAP(ssid_ap,password_ap);
  
  while(millis() - start < 1000){}

  server.begin();

}

void loop() {
  
  WiFiClient client = server.available();

  if(client){

    while(client.connected()){

      if(client.available()){
        String id = client.readStringUntil(' ');
        String cmd = client.readStringUntil('\n');
        if(cmd.charAt(0) != 'R'){
          cmd.trim();
          int sid = id.toInt();
          int val = cmd.toInt();

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

            if(cmd.length()>0 && val == 0 && cmd != "0"){

              client.println("T");
              dict[sid] = val;
            
            } else{

              client.println("F");
            }

          }

          else{

            int s; EEPROM.get(sid, s);

            if(s == sid){

              if(cmd.length()>0 && val == 0 && cmd != "0"){

                client.println("T");
                dict[sid] = val;

              } else{

                client.println("F");
              }

            }

          }
        }
        else {
          int sid = id.toInt();
          EEPROM.put(sid, -1);
          EEPROM.commit();
        }

      }

    }
    client.stop();
  }
  
}
