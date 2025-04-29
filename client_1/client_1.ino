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

WiFiClient client;

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

void setup() {
  Serial.begin(115200);
  delay(1000);

  EEPROM.begin(1024);

  pinMode(sensor, INPUT);
  pinMode(flag, OUTPUT);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){

    delay(500);

  }


  String f = "";
  while(f.charAt(0) != 'T'){

    Serial.println(f);
    f = sendInfo();

  }

  digitalWrite(flag, HIGH);

}


void loop() {}
