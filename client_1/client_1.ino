#include <ESP8266WiFi.h>
#include <EEPROM.h>
#define sensor A0
#define flag 12

const char* ssid = "ESP32-AP";
const char* password = "12345678";

const char* server_ip = "192.168.4.1";
const uint16_t server_port = 80;

int x;
int sid;
long timer;
WiFiClient client;

String sendInfo(){

  EEPROM.get(0,sid);
  Serial.print("SID ANTES: ");
  Serial.println(sid);
  if(client.connect(server_ip, server_port)){
    
    Serial.println("Conected to ESP32 Server");
    
    x = analogRead(sensor);

      /*int tmp; 
      EEPROM.get(0, tmp);
      Serial.print("TMP: ");
      Serial.println(tmp);*/

    if(sid == -1){
      client.println(String(0) + " " + String(x));
      delay(1000);
      String string_id = client.readStringUntil('\n');
      Serial.print("SID RECEBIDO: ");
      Serial.println(string_id);
      sid = string_id.toInt();
      EEPROM.put(0, sid);
      EEPROM.commit();
      Serial.print("SID: ");
      Serial.println(sid);
      delay(500);
      String msg = client.readStringUntil('\n');
      client.stop();
      Serial.println("Disconnected");
      return msg;
    }
    else{
      //sid = tmp;
      client.println(String(sid) + " " + String(x));
      String msg = client.readStringUntil('\n');
      client.stop();
      Serial.println("Disconnected");
      return msg;
    }
  }
  else return "F";

}

void setup() {
  Serial.begin(115200);
  delay(1000);

  EEPROM.begin(1024);

  pinMode(sensor, INPUT);
  pinMode(flag, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.println("Conecting ao ESP32-AP");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConected");

  String f = "";
  while(f != "T"){
    f = sendInfo();
  }
  digitalWrite(flag, HIGH);
  
}


void loop() {

}