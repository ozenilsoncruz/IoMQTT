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


// Variables will change:
int buttonState;                       
int lastButtonState = 1;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


void btn_press(int buttonPin){
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
       printf("Pressionado");
      }
    }
  }
  lastButtonState = reading;
}


int main() {
    wiringPiSetup();
    mqtt_config();  // configura o mqtt
    initDisplay();  // inicializa o display lcd

    // define os botoes como modo de entrada
    pinMode(botao_1, INPUT);
    pinMode(botao_2, INPUT);
    pinMode(botao_3, INPUT);

    write_textLCD("   Problema 3    ", "      MQTT     ");
   
    while(1){
       btn_press(botao_1);
       btn_press(botao_2);
       btn_press(botao_3);
    }

    return 0;
}
