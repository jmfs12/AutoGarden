#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define sensor A0
#define flag 12
#define button 15
#define led 14

const char* ssid = "ESP32-AP";
const char* password = "12345678";

const char* server_ip = "192.168.4.1";
const uint16_t server_port = 80;

int x;
int sid;

WiFiClient client;
bool waiting = false;
bool ledActivated = false;
unsigned long now = 0;
unsigned long ledOnTime = 0;



String sendInfo(){

  EEPROM.get(0,sid);


  if(client.connect(server_ip, server_port)){

    Serial.println("Conectado ao ESP");

    x = analogRead(sensor);

    if(sid == -1){

      client.println(String(0) + " " + String(x));
      delay(1000);
      
      String string_id = client.readStringUntil('\n');


      sid = string_id.toInt();

      EEPROM.put(0, sid);
      EEPROM.commit();


      delay(500);

      String msg = client.readStringUntil('\n');
      client.stop();
      return msg;
    } else{

      client.println(String(sid) + " " + String(x));
      String msg = client.readStringUntil('\n');
      client.stop();
      return msg;

    }

  } else return "F";

}

void resetId(){
  EEPROM.get(0, sid);

  if(client.connect(server_ip, server_port)){
    client.println(String(sid) + "R");

    EEPROM.put(0, -1);
    EEPROM.commit();

    client.stop();
  }
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


  String f = "";
  /*while(f.charAt(0) != 'T'){

    Serial.println(f);
    f = sendInfo();

  }*/

  digitalWrite(flag, HIGH);

  now = millis();
}


void loop() {

  int bt = digitalRead(button);
  Serial.println(bt);

  if (bt == LOW && !waiting) {
    waiting = true;
    now = millis();
    ledActivated = false;  // nova flag
  }

  if (waiting) {
    if (digitalRead(button) == LOW && millis() - now >= 2000 && !ledActivated) {
      digitalWrite(led, HIGH);
      ledOnTime = millis();   // marca quando o LED foi ligado
      ledActivated = true;
    }

    if (ledActivated && millis() - ledOnTime >= 2000) {
      digitalWrite(led, LOW);
      resetId();
      waiting = false;
    }

    if (digitalRead(button) == HIGH && !ledActivated) {
      waiting = false;
    }
  }

}
