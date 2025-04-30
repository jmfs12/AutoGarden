#include <EEPROM.h>
#include <WiFi.h>

const char* ssid_sta = "jmfs";
const char* password_sta = "jmfs13052022";
const char* ssid_ap = "ESP32-AP";
const char* password_ap = "12345678";

WiFiServer server(80);
WiFiClient client;

void sendId(){
  Serial.println("Função sendId chamada");
  
  int id = client.readStringUntil(' ').toInt();
  String str_value = client.readStringUntil('\n');
  str_value.trim();
  int value = str_value.toInt();

  Serial.print("ID recebido: ");
  Serial.println(id);
  Serial.print("Valor recebido: ");
  Serial.println(value);

  if (id == 0){
    Serial.println("ID é 0, procurando um ID livre na EEPROM...");

    int i = 300;
    int tmp;
    do {
      EEPROM.get(i, tmp);
      i++;
    } while(tmp != -1 && i < 512);

    id = i;
    EEPROM.put(id, id);
    EEPROM.commit();

    Serial.print("Novo ID atribuído: ");
    Serial.println(id);

    if (value <= 0)
      client.println(String(id) + " F");
    else 
      client.println(String(id) + " T");

    Serial.println("Resposta enviada ao cliente");
  }
  else if (id > 0 && id < 512){
    Serial.println("ID válido, verificando valor...");

    if (value <= 0)
      client.println("F");
    else 
      client.println("T");

    Serial.println("Resposta enviada ao cliente");
  }

}

void resetId(){
  Serial.println("Função resetId chamada");

  int id = client.readStringUntil('\n').toInt();
  int sid;
  EEPROM.get(id, sid);

  Serial.print("ID recebido para reset: ");
  Serial.println(id);
  Serial.print("ID armazenado na EEPROM: ");
  Serial.println(sid);

  if(sid == id){
    EEPROM.put(id, -1);
    EEPROM.commit();
    client.println("R");
    Serial.println("ID resetado com sucesso");
  }
  else {
    client.println("N");
    Serial.println("ID não encontrado, resposta N enviada");
  }
}

void setFunction(){
  String function = client.readStringUntil('\n');
  Serial.print("Função recebida: ");
  Serial.println(function);

  if(function.charAt(0) == 'r'){
    resetId();
  }
  else if(function.charAt(0) == 's'){
    sendId();
  }
  else {
    Serial.println("Função desconhecida");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  EEPROM.begin(512);

  Serial.println("Iniciando conexão Wi-Fi...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid_sta, password_sta);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("Conexão Wi-Fi estabelecida");
  bool ap = WiFi.softAP(ssid_ap, password_ap);
  if (ap) {
    Serial.println("Modo AP ativo");
  } else {
    Serial.println("Falha ao ativar o AP");
  }

  delay(1000);

  server.begin();
  Serial.println("Servidor iniciado, aguardando conexões...");
}

void loop() {
  client = server.available();
  if(client){
    Serial.println("Cliente conectado!");

    while(client.connected()){
      if(client.available()){
        setFunction();
      }
    }

    Serial.println("Cliente desconectado");
    client.stop();
  }
}
