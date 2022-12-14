#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Timer.h>
#include <WiFiUdp.h>

#include <string.h> 


#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Nome do ESP na rede
const char* host = "ESP-10.0.0.108";
IPAddress local_IP(10, 0, 0, 108);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

// Definições do servidor MQTT
//const char* BROKER_MQTT = "broker.emqx.io";  // broker MQTT 
const char* BROKER_MQTT = "10.0.0.101";        // broker MQTT 
int BROKER_PORT = 1883;
              
// Definições do ID
#define ID_MQTT           "ESP"  // ID desta nodeMCU
#define USER "aluno"
#define PASSWORD "@luno*123"
#define QOS 2  
WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);   // Instancia o Cliente MQTT passando o objeto espClient

// Topicos a serem subescritos
#define SBC_ESP           "sbc/esp"

// Topicos a serem publicados
#define SENSORES_D        "esp/sensores_digitais"
#define SENSORES_A        "esp/sensores_analogicos"
#define SENSOR_ANALOG     "esp/analog_sensor"
#define SENSOR_DIGITAL    "esp/digital_sensor"
#define STATUS            "esp/status"
#define LED               "esp/led"

// tempo entre as medicoes altomaticas
int tempo_medicoes = 60;  // padrao 1 minuto
Timer t;

/**
 * Reconecta-se ao broker Tenta se conectar ao broker constantemente
 */
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT, USER, PASSWORD, SBC_ESP, QOS, false, "1F")){
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(SBC_ESP, 1); 
    } else{
      Serial.println("Falha ao se reconectar no broker!");
      Serial.println("\nTentando se conectar em 2s...\n");
      delay(2000);
    }
  }
}


/**
 * Caso a NodeMCU não esteja conectado ao WiFi, a conexão é restabelecida.
*/
void reconnectWiFi() {
  //se já está conectado a rede WI-FI, nada é feito. 
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;
        
  WiFi.begin(ssid, password); // Conecta na rede WI-FI
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}


/**
 * Verifica se o cliente está conectado ao broker MQTT e ao WiFi.
 * Em caso de desconexão, a conexão é restabelecida.
*/
void checkMQTTConnection(void) {
  reconnectWiFi();
  if (!MQTT.connected()){
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
  } 
}


/**
 * Realiza as medições dos sensores
*/
void medicoes(void){
  char digitais[10];
  sprintf(digitais, "%d%d%d%d%d%d%d%d", digitalRead(D0), digitalRead(D1), 
                                        digitalRead(D2), digitalRead(D3), 
                                        digitalRead(D4), digitalRead(D5),
                                        digitalRead(D6), digitalRead(D7));

  char analogicos[6];
  sprintf(analogicos, "%d", analogRead(A0)); 
  MQTT.publish(SENSORES_D, digitais);   // envia atualizacao para o topico dos sensores digitais
  MQTT.publish(SENSORES_A, analogicos); // envia atualizacao para o topico dos sensores analogicos
}


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
  String msg;
  if(length > 0){
    for(int i = 0; i < length; i++) {
      char c = (char)payload[i];
      msg += c;
    }
    if(msg[0] == '3'){
      MQTT.publish(STATUS, "00");
    }
    else if(msg[0] == '4'){
      char analog[] = "";
      sprintf(analog,"%d", analogRead(A0));
      MQTT.publish(SENSOR_ANALOG, analog);
    }
    else if(msg[0] == '5'){
      int d = D0;
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
      char digital[] = "";
      sprintf(digital,"%c%d", msg[1], digitalRead(d));
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
    else if(msg[0] == '7'){
      // tempo para enviar novas de mensagens
      tempo_medicoes = (msg[1] - 47) * 10; //Subtrai 48 para obter o valor inteiro e multiplica por 10 para ter o valor em segundos
      t.stop(0);
      t.every(tempo_medicoes * 1000, medicoes);
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
}


/**
 * Inicia as configuracoes no momento do upload 
 */
void setup() {
  // realiza a configuracao inicial para conexao via wifi com nodemcu
  config_connect();
  Serial.begin(9600);

  // definicao dos pinos
  pinMode(LED_BUILTIN, OUTPUT);  

  // inicia a comunicacao mqtt
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
  MQTT.setCallback(on_message);

  t.every(tempo_medicoes * 1000, medicoes); 

  // pisca o led do nodemcu no momento da execucao
  for(int i=0; i<10; i++){
    digitalWrite(LED_BUILTIN,LOW);
    delay(50);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(50);
  }
}


/**
 * Executa um loop 
 */
void loop() {
  ArduinoOTA.handle();
  
  checkMQTTConnection();

  MQTT.loop();

  t.update();
}
