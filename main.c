  GNU nano 6.2                                    main.c                                             
#include "mqtt_sbc.h"
#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>


// Botões 
#define botao_1 19
#define botao_2 23
#define botao_3 25
  
// Topicos
#define MQTT_PUBLISH_TOPIC     "sbc/sensores"
#define MQTT_SUBSCRIBE_TOPIC   "node/+/sensores/#"

int main() {
    wiringPiSetup();
    //mqtt_config();  // configura o mqtt
    initDisplay();  // inicializa o display lcd

    // define os botoes como modo de entrada
    pinMode(botao_1, INPUT);
    pinMode(botao_2, INPUT);
    pinMode(botao_3, INPUT);

    write_textLCD("   Problema 3    ", "      MQTT     ");
   
    while(1){
       if(digitalRead(botao_1) == 0){
          printf("Hello, World!");
       }
    }

    return 0;
}
