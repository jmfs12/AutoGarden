#include <EEPROM.h>
#include <WiFi.h>

const char* ssid_sta = "jmfs";
const char* password_sta = "jmfs13052022";
const char* ssid_ap = "ESP32-AP";
const char* password_ap = "12345678";

WiFiServer server(80);
WiFiClient client;

void sendId(){
  int id = client.readStringUntil(' ').toInt();
  String str_value = client.readStringUntil('\n');
  str_value.trim();
  int value = str_value.toInt();

  if (id == 0){

    int i = 300;
    int tmp;
    do{
      EEPROM.get(i, tmp);
      i++;
    }while(tmp != -1 && i < 512);

    id = i;
    EEPROM.put(id,id);
    EEPROM.commit();

    if (value <= 0)
      client.println(String(id) + " F");
    else 
      client.println(String(id) + " T");

  }
  else if(id > 0 && id < 512){
    
    if (value <= 0)
      client.println("F");
    else 
      client.println("T");

  }

}
 
void resetId(){

  int id = client.readStringUntil('\n').toInt();
  int sid;
  EEPROM.get(id, sid);
  if( sid == id ){
    EEPROM.put(id, -1);
    EEPROM.commit();
    client.println("R");
  }

  else{
    client.println("N");
  }

}

void setFunction(){
  
  String function = client.readStringUntil('\n');
  if(function.equals("reset")){
    resetId();
  }
  else if(function.equals("send")){
    sendId();
  }

}

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
  client = server.available();
  if(client){

    while(client.connected()){

      if(client.available()){

        setFunction();

      }

    }
    client.stop();
  }

}
