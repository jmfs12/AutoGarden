#include <EEPROM.h>
#include <ESP8266WiFi.h>

#define sensor A0
#define flag 12
#define button 15
#define led 14

const char* ssid = "ESP32-AP";
const char* password = "12345678";

const char* server_ip = "192.168.4.1";
const uint16_t server_port = 80;

int eid;
const int pos_id = 0;

WiFiClient client;
bool waiting = false;
bool ledActivated = false;
unsigned long now = 0;
unsigned long ledOnTime = 0;
String conf = ""; 

String sendId(){

  if(client.connect(server_ip, server_port)){
    client.println("send");
    EEPROM.get(pos_id, eid);

    int value = analogRead(sensor);

    if(eid == -1){
      client.println("0 " + String(value));
      delay(500);

      eid = client.readStringUntil(' ').toInt();
      String msg = client.readStringUntil('\n');
      msg.trim();

      if (eid >= 0 && eid < 511){
        EEPROM.put(0, eid);
        EEPROM.commit();
      }
      client.stop();
      return msg;
    }
    else if (eid >= 0 && eid < 511){
      client.println(String(eid) + " " + String(value));

      String msg = client.readStringUntil('\n');
      client.stop();
      return msg;
    }
    else return "F";

  }
  else return "F";

}

String resetId(){

  if (client.connect(server_ip, server_port)){

    client.println("reset");
    EEPROM.get(pos_id, eid);

    client.println(String(eid));

    EEPROM.put(pos_id, -1);
    EEPROM.commit();

    String msg = client.readStringUntil('\n');
    msg.trim();
    client.stop();
    return msg;

  }
  return "N";

}

void setup() {

  Serial.begin(115200);
  delay(1000);

  EEPROM.begin(1024);

  pinMode(sensor, INPUT);
  pinMode(flag, OUTPUT);
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){

    delay(500);

  }

  int i = 0;
  bool f = false;
  for(int i = 0; i < 10; i++){

    String msg = sendId();
    f = msg.equals("T");
    if (f){

      digitalWrite(flag, HIGH);
      delay(2000);
      digitalWrite(flag, LOW);
      break;

    }

  }

}

void loop() {

  int bt = digitalRead(button);

  if (bt == LOW && !waiting) {
    waiting = true;
    now = millis();
    ledActivated = false;
  }

  if (waiting) {
    if (digitalRead(button) == LOW && millis() - now >= 2000 && !ledActivated) {
      conf = resetId();  // chama aqui antes de acender o LED

      if (conf == "R") {
        digitalWrite(led, HIGH);
      }
      
      ledOnTime = millis();
      ledActivated = true;
    }

    if (ledActivated && millis() - ledOnTime >= 2000) {
      digitalWrite(led, LOW);
      Serial.println("Resetando");
      waiting = false;
    }

    if (digitalRead(button) == HIGH && !ledActivated) {
      waiting = false;
    }
  }


}
