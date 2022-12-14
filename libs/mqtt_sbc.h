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
    	write_textLCD_linha(1, "LED");
	}
    else if(strcmp(topicName, STATUS) == 0){
        write_textLCD_linha(1, "STATUS");
	}
    else if(strcmp(topicName, SENSOR_ANALOG) == 0){
        write_textLCD_linha(1, "Analog");
    }
    else if(strcmp(topicName, SENSOR_DIGITAL) == 0){
        write_textLCD_linha(1, "Digital");
    }
    else if(strcmp(topicName, SENSORES_A) == 0){
        write_textLCD_linha(1, "Analogicos");
    }
    else if(strcmp(topicName, SENSORES_D) == 0){
        write_textLCD_linha(1, "Digitais");
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
 * @param cleinte - cliente MQTT que publicara o topico
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