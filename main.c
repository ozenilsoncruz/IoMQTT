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
  increver(IHM_TIME);
  
  // define os botoes como modo de entrada
  pinMode(botao_1, INPUT);
  pinMode(botao_2, INPUT);
  pinMode(botao_3, INPUT);

  write_textLCD("   Problema 3    ", "      MQTT     ");
  while(btn_press(botao_2) == 0);

  int op_ant = 1;
  int opcao = 0;
  do{
    char menu[6][16] =  {"Status", 
                         "Analogica", 
                         "Digital", 
                         "Led",
                         "Tempo",
                         "Sair"};

    if (op_ant != opcao){
      op_ant = opcao;
      write_textLCD("      MQTT     ", menu[opcao]);
    }
    
    if(btn_press(botao_3) == 1){
      opcao++;
      if(opcao > 5){
        opcao = 0;
      }
    }
    if(btn_press(botao_1) == 1){
      opcao--;
      if(opcao < 0){
        opcao = 5;
      }
    }
    // se o botao de enter for pressionado, seleciona a opcao
    if(btn_press(botao_2) == 1){
      switch(opcao){
        case 0:
          publicar(SBC_ESP, "30");
          break;
        case 1:
          write_textLCD("Leitura Analogica", "A1");
          while(btn_press(botao_1) == 0){
            if(btn_press(botao_2) == 1){
              publicar(SBC_ESP, "40"); // envia o comando e o sensor indicado
            }
          }
          write_textLCD("      MQTT     ", menu[opcao]);
          publicar(SBC_ESP, "40");
          break;
        case 2:
          int sensor_ant = 0;
          int sensor = 1;
          while(btn_press(botao_1) == 0){
            if (sensor_ant != sensor){
              sensor_ant = sensor;
              char dig[5];
              sprintf(dig, "D%d", sensor);
              write_textLCD("Leitura Digital", dig);
            }
            if(btn_press(botao_3) == 1){
              sensor++;
              if(sensor > 8){
                sensor = 1;
              }
            }
            if(btn_press(botao_2) == 1){
              char texto[5];
              sprintf(texto, "5%d", sensor);
              publicar(SBC_ESP, texto); // envia o comando e o sensor indicado
            }
          }
          write_textLCD("      MQTT     ", menu[opcao]);
          break;
        case 3:
          publicar(SBC_ESP, "60"); // envia o comando e o sensor indicado
          break;
        case 4:
          int sT_ant = 1;
          int tempo_ant = 0;
          int tempo = 1;
          int sT = 0;
          char * sTime = "smh";
          while(btn_press(botao_2) == 0){ // seleciona o intervalo de tempo
            if (sT_ant != sT){
              sT_ant = sT;
              char t[10];
              sprintf(t, "Tempo (%c)", sTime[sT]);
              write_textLCD("Escolha o tempo:", t);
            }
            if(btn_press(botao_1) == 1){
              sT--;
              if(sT < 0){
                sT = 2;
              }
            }
            if(btn_press(botao_3) == 1){
              sT++;
              if(sT > 2){
                sT = 0;
              }
            }
          }
          while(btn_press(botao_2) == 0){
            if (tempo_ant != tempo){
              tempo_ant = tempo;
              char t[10];
              char dig[5];
              sprintf(t, "Tempo (%c)", sTime[sT]);
              sprintf(dig, "%d", tempo);
              write_textLCD(t, dig);
            }
            if(btn_press(botao_1) == 1){
              tempo--;
              if(tempo < 0){
                tempo = 10;
              }
            }
            if(btn_press(botao_3) == 1){
              tempo++;
              if(tempo > 10){
                tempo = 1;
              }
            }
          }
          char texto[5];
          sprintf(texto, "7%d%c", sensor-1, sTime[sT]);
          publicar(SBC_ESP, texto); // envia o comando e o sensor indicado
          write_textLCD("      MQTT     ", menu[opcao]);
          break;
        case 5:
          write_textLCD("Finalizando.....", "");
          return 0;
          break;
        default:
          printf("\n\n\tOpcao invalida!\n\n");
      }
    }
  } while(opcao != 6);
  
  return 0;
}