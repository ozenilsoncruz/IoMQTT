#ifndef MQTT_SBC_H
#define MQTT_SBC_H

#include "display.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>

// Configuracoes mqtt
#define CLIENTID   "sbc"
//#define BROKER     "mqtt://broker.emqx.io:1883"
#define BROKER     "tcp://10.0.0.101:1883"
#define USERNAME   "aluno"
#define PASSWORD   "@luno*123"

// Topicos a serem publicados
#define SBC_ESP "sbc/esp"
#define SBC_IHM "sbc/ihm"

// Topicos a se inscrever
#define SENSORES_D        "esp/sensores_digitais"
#define SENSORES_A        "esp/sensores_analogicos"
#define SENSOR_ANALOG     "esp/analog_sensor"
#define SENSOR_DIGITAL    "esp/digital_sensor"
#define STATUS            "esp/status"
#define LED               "esp/led"

#define IHM_TIME          "imh/tempo"

/*char dig_history[10][9] =   {"00000000", 
                            "00000000", 
                            "00000000", 
                            "00000000",
                            "00000000",
                            "00000000", 
                            "00000000",
                            "00000000",
                            "00000000", 
                            "00000000"};

char analog_history[10][5] = {"0000", 
                            "0000", 
                            "0000", 
                            "0000",
                            "0000",
                            "0000", 
                            "0000",
                            "0000",
                            "0000", 
                            "0000"};*/

// variavel cliente MQTT
MQTTClient client;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/**
 * Recebe as mensagens dos topicos inscritos
 * @param context - 
 * @param topicName - Nome do topico que mando a mensagem 
 * @param topicLen  - Tamamnho do nome do topico
 * @param mensagem - Mensagem recebida
 */
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    
    printf("\n\nMensagem recebida! \n\rTopico: %s Mensagem: %s\n\n\n", topicName, payload);

    if(strcmp(topicName, LED) == 0){
        if(strcmp(payload, "1")){
            write_textLCD("      MQTT     ", "LED: ON");
        }
        else if(strcmp(payload, "0")){
            write_textLCD("      MQTT     ", "LED: OFF");
        }
	}
    else if(strcmp(topicName, STATUS) == 0){
        if(strcmp(payload, "00")){
            write_textLCD("      MQTT     ", "Status: OK");
        }
        else if(strcmp(payload, "1F")){
            write_textLCD("      MQTT     ", "Status: ERROR");
        }
	}
    else if(strcmp(topicName, SENSOR_ANALOG) == 0){
        char texto[30];
        sprintf(texto, "A1: %s", payload);
        write_textLCD("Leitura Analogica", texto);
    }
    else if(strcmp(topicName, SENSOR_DIGITAL) == 0){
        char texto[30];
        sprintf(texto, "D%c: %c", payload[0], payload[1]);
        write_textLCD("Leitura Digital", texto);
    }
    else if(strcmp(topicName, SENSORES_A) == 0){
        publicar(SBC_IHM, analog_history); // envia o comando e o sensor indicado
    }
    else if(strcmp(topicName, SENSORES_D) == 0){
        publicar(SBC_IHM, dig_history); // envia o comando e o sensor indicado
    }
    else if(strcmp(topicName, IHM_TIME) == 0){
        publicar(SBC_ESP, payload);// envia o tempo recebido para nodemcu
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

/**
 * Realiza as configurações iniciais para a comunicacao mqtt
*/
void mqtt_config(){
    while(!MQTTClient_isConnected(client)){
        int rc;
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        conn_opts.username = USERNAME;
        conn_opts.password = PASSWORD;

        //Inicializacao do MQTT (conexao & subscribe)
        MQTTClient_create(&client, BROKER, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;

        // Adiciona msgs 
        MQTTClient_setCallbacks(client, NULL, mqtt_config, on_message, NULL);

        // Verifica se o cliente mqtt esta conectado
        rc = MQTTClient_connect(client, &conn_opts);
        if (rc != MQTTCLIENT_SUCCESS) {
            printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
            exit(-1);
        }
    } 
}

/**
 * Publica uma mensagem num dado topico
 * @param cliente - cliente MQTT que publicara o topico
 * @param topico - topico MQTT a ser publicado
 * @param payload - conteudo da mensagem
*/
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

/**
 * Inscreve-se em um topico mqtt
 * @param topic - topico que deseja se inscrever
*/
void increver(char* topic){
    MQTTClient_subscribe(client, topic, 0);
}

/**
 * Publica uma mensagem em um dado topico
 * @param topic - Topico da mensagem a ser publicada
 * @param payload - Mensagem a ser enviada
 */
void publicar(char* topic, char* payload){
   publish(client, topic, payload);
}

#endif