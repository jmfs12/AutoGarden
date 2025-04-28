#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define sensor A0
#define flag 12
#define button 15
#define led 18

const char* ssid = "ESP32-AP";
const char* password = "12345678";

const char* server_ip = "192.168.4.1";
const uint16_t server_port = 80;

unsigned long start;
unsigned long now;
int x;
int sid;
WiFiClient client;

String sendInfo(){

  EEPROM.get(0,sid);

  if(client.connect(server_ip, server_port)){
    
    x = analogRead(sensor);

    if(sid == -1){

      client.println(String(0) + " " + String(x));
      start = millis();
      while(millis() - start < 1000){}
      
      String string_id = client.readStringUntil('\n');
      sid = string_id.toInt();

      EEPROM.put(0, sid);
      EEPROM.commit();
      while(millis() - start < 500){}

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

    client.println(sid + "R");

    EEPROM.put(0, -1);
    EEPROM.commit();



  }
}

void setup() {

  EEPROM.begin(1024);

  pinMode(sensor, INPUT);
  pinMode(flag, OUTPUT);
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);

  WiFi.begin(ssid, password);

  start = millis();
  while(WiFi.status() != WL_CONNECTED){

    while(millis() - start < 500){}

  }

  String f = "";
  start = millis();
  while(f.charAt(0) != 'T' && millis() - start < 10000){

    f = sendInfo();

  }

  digitalWrite(flag, HIGH);

  now = millis();
  
}

void loop() {
  int bt = digitalRead(button);

  if (bt == LOW && !waiting) { // Detectou botão pressionado
    waiting = true;
    now = millis();
  }

  if (waiting) {
    // Se passaram 2 segundos e botão ainda pressionado
    if ((millis() - now >= 2000) && (digitalRead(button) == LOW)) {
      digitalWrite(led, HIGH);
      resetId();
      waiting = false; // Reset
    }
    // Se botão soltar antes dos 2 segundos
    else if (digitalRead(button) == HIGH) {
      waiting = false; // Cancela
    }
  }
}