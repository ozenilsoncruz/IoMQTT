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

// variaveis para o debounce
int buttonState;                       
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/**
 * Implementa um debounce para verificar se um botao foi ou nao precionado
 * @param buttonPin - Botao a ser verificado
 */
void btn_press(int buttonPin){
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
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
  
  // teste de botao com debounce
  while(1){
    btn_press(botao_1);
  }

  /*int opcao = 0;
  do{
    char* menu[] = {"Situacao NodeMCU", 
                    "Entrada analogica", 
                    "Entrada digital", 
                    "Acender/Apagar LED",
                    "Sair"};
    
    if(btn_press(botao_1) == 0  && opcao < 5){
        opcao = opcao + 1;
    }

    if(btn_press(botao_1) == 0 && opcao > 0){
        opcao = opcao - 1;
    }
    
    // se o botao de enter for pressionado, seleciona a opcao
    if(btn_press(botao_2) == 0){
      switch(opcao){
        case 0:
          printf("Situacao NodeMCU");
          break;
        case 1:
          printf("Entrada analogica");
          break;
        case 2:
          printf("Entrada digital");
          break;
        case 3:
          printf("Acender/Apagar LED");
          break;
        case 4:
          break;
        default:
          printf("\n\n\tOpcao invalida!\n\n");
      }
    }
  } while(opcao != 4);*/

  return 0;
}
