#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>

#include <string.h> 


#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Nome do ESP na rede
const char* host = "ESP-10.0.0.109";
IPAddress local_IP(10, 0, 0, 109);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

// mqtt --------------------------------
/*WiFiClient espClient;                                 //Instância do WiFiClient
PubSubClient client(espClient);                       //Passando a instância do WiFiClient para a instância do PubSubClient

#define servidor_mqtt             ""  //URL do servidor MQTT
#define servidor_mqtt_porta       ""  //Porta do servidor (a mesma deve ser informada na variável abaixo)
#define servidor_mqtt_usuario     ""  //Usuário
#define servidor_mqtt_senha       ""  //Senha
#define mqtt_topico_pub           "esp8266/pincmd"    //Tópico para publicar o comando de inverter o pino do outro ESP8266*/

// --------------------------------

// Definições do servidor MQTT
const char* BROKER_MQTT = "10.0.0.101";  //URL do broker MQTT 
int BROKER_PORT = 1883;  

// Definições do ID
#define ID_MQTT "ESP"            // ID desta nodeMCU
WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);   // Instancia o Cliente MQTT passando o objeto espClient

// Topicos a serem subescritos
#define SBC "SBC";

// Topicos a serem publicados
#define SENSOR_ANALOG "ANALOG_SENSOR";
#define SENSOR_DIGITAL "DIGITAL_SENSOR";
#define LED "LED";
#define STATUS "STATUS";

/**
 * Configura a comunicacao com o nodemcu via WIFI
*/
void config_connect(){
  Serial.begin(115200);
  Serial.println("Booting");

  // Configuração do IP fixo no roteador, se não conectado, imprime mensagem de falha
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * Recebe as mensagens via mqtt 
 * @param topic - Topico que enviou a mensagem
 * @param payload - Mensagem recebida
 * @param length - Tamanho da mensagem
*/
void on_message(char* topic, byte* payload, unsigned int length){
    char* msg;
    for(int i = 0; i < length; i++) {
      char c = (char)payload[i];
      msg += c;
    }

    if(msg[0] == '3'){
      MQTT.publish(STATUS, "00");
    }
    else if(msg[0] == '4'){
      char analog[3];
      sprintf(analog,"%f",analogRead(A0));
      MQTT.publish(SENSOR_ANALOG, analog);
    }
    else if(msg[0] == '5'){
      int d;
      switch(msg[1]){
        case '1':
          d = D0;
          break;
        case '2':
          d = D1;
          break;
        case '3':
          d = D2;
          break;
        case '4':
          d = D3;
          break;
        case '5':
          d = D4;
          break;
        case '6':
          d = D5;
          break;
        case '7':
          d = D6;
          break;
        case '8':
          d = D7;
          break;
      }
      char digital[1];
      sprintf(digital,"%f",digitalRead(A0));
      MQTT.publish(SENSOR_DIGITAL, digital);
    }
    else if(msg[0] == '6'){
      if(digitalRead(LED_BUILTIN) == HIGH){
        digitalWrite(LED_BUILTIN, LOW);
        MQTT.publish(LED, "1");
      }
      else{
        digitalWrite(LED_BUILTIN, HIGH);
        MQTT.publish(LED, "0");
      }
    }
    else{
      // envia a mensagem de erro
      MQTT.publish(STATUS, "1F");
      // pisco o led caso a mensagem nao seja reconhecida
      for(int i=0; i<5; i++){
        digitalWrite(LED_BUILTIN,LOW);
        delay(400);
        digitalWrite(LED_BUILTIN,HIGH);
        delay(400);
      }
    }
}

void setup() {
  // realiza a configuracao inicial para conexao via wifi com nodemcu
  config_connect();
  Serial.begin(9600);

  // definicao dos pinos
  pinMode(LED_BUILTIN, OUTPUT);  

  MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
  MQTT.setCallback(on_message);

  // pisca o led do nodemcu no momento da execucao
  for(int i=0; i<10; i++){
    digitalWrite(LED_BUILTIN,LOW);
    delay(50);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(50);
  }

  MQTT.subscribe(SBC);
}

void loop() {
  ArduinoOTA.handle();
  
  MQTT.loop();
}
