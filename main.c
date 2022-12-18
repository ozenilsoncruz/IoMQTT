#include "libs/display.h"
#include "libs/mqtt_sbc.h"

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

/**
 * Navega entre um array de strings e exibe a opcao no display LCD 
 * @param linha1 - Primeira linha do display
 * @param opcoes - Array de opcoes que sera percorrido
 * @param btn_ant - Botao que retorna a opcao
 * @param btn_ok - Botao que seleciona a opcao
 * @param btn_prox -Botao que avanca a opcao
 * @param qtd_op - Quantidade de opcoes do menu 
 */
int menu_naviagtion(char *linha1, char opcoes[][], int btn_ant, int btn_ok, int btn_prox, int qtd_op){
  int op_ant = 1;
  int op = 0;

  while(btn_press(btn_ok) == 0){
    if (op_ant != op){            // se a opcao anterior
      op_ant = op;
      write_textLCD(linha1, opcoes[op]);
    }

    if(btn_press(btn_prox) == 1){ // avanca a linha
      op++;
      if(op > 5){
        op = 0;
      }
    }
    if(btn_press(btn_ant) == 1){  // retorna a linha a linha
      op--;
      if(op < 0){
        op = 5;
      }
    }
  }
  return 1;
  
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

  write_textLCD("   Problema 3   ", "      MQTT      ");
  while(btn_press(botao_2) == 0);

  do{
    char menu[6][16] =  {"Status", 
                         "Analogica", 
                         "Digital", 
                         "Led",
                         "Tempo",
                         "Sair"};

    // se o botao de enter for pressionado, seleciona a opcao
    
    int opcao = menu_naviagtion("      MQTT     ", menu, botao_1, botao_2, botao_3, 5);
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
        char menu[8][4] = {"D1", "D2", "D3", "D4", "D5", "D6", "D7",  "D8"};
        int dig = menu_naviagtion("Leitura Digital", sensores, botao_1, botao_2, botao_3, 8);
        char texto[5];
        sprintf(texto, "5%d", dig);
        publicar(SBC_ESP, texto); // envia o comando e o sensor indicado
        write_textLCD("      MQTT     ", menu[opcao]);
        break;
      case 3:
        publicar(SBC_ESP, "60"); // envia o comando e o sensor indicado
        break;
      case 4:
        char st[3] = "smh"; // tempo em sengundo, minutos ou horas

        //menu para selecionar o intervalo de tempo (em horas, minutos ou segundos)
        char sTime[3][10] = {"Tempo (s)","Tempo (m)", "Tempo (h)"};
        int st_int = menu_naviagtion("Intervalo em:", sTime, botao_1, botao_2, botao_3, 2);

        //menu para selecionar o intervalo de tempo em que a esp enviara as atualizacoes
        char tempo[10][2] = {"1", "2", "3","4", "5", "6", "7","8", "9", "10"};
        int tempo_int = menu_naviagtion(sTime[st_int], tempo, botao_1, botao_2, botao_3, 10);
        
        char texto[5];
        sprintf(texto, "7%d%s", tempo_int, st[st_int]);
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
  } while(opcao != 6);
  
  return 0;
}