#include "display.h"
#include "mqtt_sbc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

// Botões
#define botao_1 19
#define botao_2 23
#define botao_3 25

/**
 * Implementa um debounce para verificar se um botao foi ou nao precionado
 * @param buttonPin - Botao a ser verificado
 */
int btn_press(int buttonPin){
  if(digitalRead(buttonPin) == 0){    // verifica se o botao foi pressionado
    delay(30);                        // aguarda um tempo
    if(digitalRead(buttonPin) == 0){  // verifica se o botao continua pressionado
      while(digitalRead(buttonPin) == 0);  // aguarda no loop ate que o botao pare de ser pressionado
      return 1;
    }
  }
  return 0;
}


int main() {
  wiringPiSetup();
  mqtt_config();  // configura o mqtt
  initDisplay();  // inicializa o display lcd

  // Se inscreve nos topicos princiapis
  increver(SENSOR_DIGITAL);
  increver(SENSOR_ANALOG);
  increver(SENSORES_D);
  increver(SENSORES_A);
  increver(STATUS);
  increver(LED);
  
  // define os botoes como modo de entrada
  pinMode(botao_1, INPUT);
  pinMode(botao_2, INPUT);
  pinMode(botao_3, INPUT);

  write_textLCD("   Problema 3    ", "      MQTT     ");
  while(btn_press(botao_2) == 0);

  int opcao = 0;
  int sensor;
  do{
    char menu[5][16] =  {"NodeMCU", 
                         "Analogica", 
                         "Digital", 
                         "LED",
                         "Sair"};
    
    if(btn_press(botao_1) == 1){
      opcao = opcao + 1;
      if(opcao > 4){
        opcao = 0;
      }
    }
    if(btn_press(botao_3) == 1){
      opcao = opcao - 1;
      if(opcao < 0){
        opcao = 4;
      }
    }
    write_textLCD_linha(1, menu[opcao]);
    // se o botao de enter for pressionado, seleciona a opcao
    if(btn_press(botao_2) == 1){
      switch(opcao){
        case 0:
          publicar(SBC_ESP, "30");
          break;
        case 1:
          publicar(SBC_ESP, "40");
          break;
        case 2:
          /*
          while(btn_press(botao_1) == 0){
            if(btn_press(botao_3) == 1){
              sensor = sensor - 1;
              if(sensor < 0){
                sensor = 4;
              }
            }
            if(btn_press(botao_2) == 1){
              char texto[5];
              sprintf(texto, "5%d", sensor);
              publicar(SBC_ESP, texto); // envia o comando e o sensor indicado
            }
          }
          */
          break;
        case 3:
          publicar(SBC_ESP, "60");
          break;
        case 4:
          // tempo
          break;
        default:
          printf("\n\n\tOpcao invalida!\n\n");
      }
    }
  } while(opcao != 4);

  /*char sensor[] = "0";
  char opcao = '/';
  do{
    printf("========================================\n");
    printf("            Escolha uma opcao           \n");
    printf("----------------------------------------\n");
    printf("| 1 | Situação atual do NodeMCU\n"); // 0x03
    printf("| 2 | Valor da entrada analógica\n"); // 0x04
    printf("| 3 | Valor das entradas digitais\n"); // 0x05
    printf("| 4 | Acender/Apagar LED\n"); // 0x06
    printf("| 0 | Sair\n");
    printf("========================================\n");
    printf("=>  ");
    scanf("%s", &opcao);
    system("cls || clear");

    switch(opcao){
      case '1': // solicitar status
        publicar(SBC_ESP, "30");
        break;
      case '2': // sensor analogico
        publicar(SBC_ESP, "40");
        break;
      case '3': // sensor digital
        printf("\nQual sensor digital deseja selecionar? [1-8] \n =>  ");
        scanf("%s", sensor);
        if(sensor[0] >= '1' && sensor[0] <= '8' && strlen(sensor) == 1){
          char texto[5];
	        sprintf(texto, "5%s", sensor);
          publicar(SBC_ESP, texto); // envia o comando e o sensor indicado
        }else{
          printf("\nOpção inválida!\n");
        }
        break;
      case '4':
        publicar(SBC_ESP, "60");
        break;
      case '0':
        printf("\n\n\tFinalizando...\n");
        break;
      default:
        printf("\n\n\tOpcao invalida!\n\n");
    }
  }while(opcao != '0');*/

  return 0;
}