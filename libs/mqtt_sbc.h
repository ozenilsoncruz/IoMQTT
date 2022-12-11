#ifndef MQTT_SBC_H
#define MQTT_SBC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>

// Configuracoes mqtt
#define CLIENTID   "sbc"
#define BROKER     "tcp://10.0.0.101:1883"
#define USERNAME   "aluno"
#define PASSWORD   "@luno*123"

// variavel cliente MQTT
MQTTClient client;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);


int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;

    
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

    /*
    if(strcmp(topicName,NODE_CONNECTION_STATUS) == 0){
    	if(strcmp(msg,"0x200") == 0){
		send(REQUEST,GET_LED_VALUE);
	}
    }else if(strcmp(topicName,RESPONSE) == 0){
    	if(strcmp(msg,"0x03") == 0){
		ledState = 1;
	}else if(strcmp(msg,"0x04") == 0){
		ledState = 0;
	}
    }else if(strcmp(topicName,DIGITAL_SENSOR) == 0){
	char buf[strlen(msg)];
    	strcpy(buf,msg);
    	setDigitalValueSensors(buf);
	updateHistoryList(historyList[nextHistory]);
    }else if(strcmp(topicName,ANALOG_SENSOR) == 0){
    	analogValue = msg;
    }*/

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

/**
 * Realiza as configurações iniciais para a comunicacao mqtt
*/
void mqtt_config(){
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    //Inicializacao do MQTT (conexao & subscribe)
    MQTTClient_create(&client, BROKER, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    // Adiciona msgs 
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    // Verifica se o cliente mqtt esta conectado
    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        exit(-1);
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