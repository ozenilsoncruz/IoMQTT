#ifndef MQTT_SBC_H
#define MQTT_SBC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>

/*https://embarcados.com.br/paho-mqtt-em-c-no-linux-embarcado/*/

// variavel cliente MQTT
MQTTClient client;

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/**
 * Realiza as configurações iniciais para a comunicacao mqtt
*/
void mqtt_config(){
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    /* Inicializacao do MQTT (conexao & subscribe) */
    MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS) {
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
void subscribe(char* topic){
    MQTTClient_subscribe(client, topic, 0);
}

/**
 * Recebe uma mensagem de um topico MQTT
 * 
 * @return payload - Mensagem recebida pelo topico
*/
char* MQTT_receive(){
    /**
     * Recebe as mensagens de num dado topico
     * @param contexto ponteiro para nome do topico da mensagem recebida, 
     * @param tamanho do nome do topico
     * @param mensagem - mensagem recebida
     * 
     * @return 1 sucesso
    */
    static char* payload = "";
    int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
        payload = message->payload;

        /* Mostra a mensagem recebida */
        printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);

        return 1;
    }
    return payload;
}

#endif
