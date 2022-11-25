#include "libs/display.h"
#include "libs/mqtt_sbc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MQTT_ADDRESS   "10.0.0.101" // Ip da maquina Brocker
#define CLIENTID       "sbc"  

#define MQTT_PUBLISH_TOPIC     "sbc/sensores"
#define MQTT_SUBSCRIBE_TOPIC   "node/+/sensores/#"

int main() {
    initDisplay();  // inicializa o display lcd
    write_textLCD("Problema 2 - SD");

    mqtt_config();

    subscribe(MQTT_SUBSCRIBE_TOPIC);
    

    char sensor[] = "0";
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
            case '1':
                uart_send("30", uart_filestream);
                if(strcmp(uart_receive(uart_filestream, 2), "00") == 0){
                    write_textLCD("NodeMCU OK!");
                }
                break;
            case '2': // sensor analogico
                uart_send("40", uart_filestream);

                char texto[] = "";
                char* result = uart_receive(uart_filestream, 4);
                sprintf(texto, "Sensor A: %s", result);

                write_textLCD(texto);
                break;
            case '3': // sensor digital
                printf("\nQual sensor digital deseja selecionar? [1-8] \n =>  ");
                scanf("%s", &sensor);

                if(sensor[0] >= '1' && sensor[0] <= '8' && strlen(sensor) == 1){
                    uart_send("5", uart_filestream);
	  	            uart_send(sensor, uart_filestream);

                    char texto[] = "";
                    char* result = uart_receive(uart_filestream, 1); // recebe apenas 1 byte
                    sprintf(texto, "Sensor D%s: %s", sensor, result);

                    write_textLCD(texto);
                }else{
                    printf("\nOpção inválida!\n");
                }
                break;
            case '4':
                uart_send("60", uart_filestream);
                if(uart_receive(uart_filestream, 1)[0] == '1'){ 
                    write_textLCD("LED ligado!");
                }else{
                    write_textLCD("LED desligado!");
                }
                break;
            case '0':
                printf("\n\n\tFinalizando...\n");
                break;
            default:
                printf("\n\n\tOpcao invalida!\n\n");
        }
    } while(opcao != '0');

    return 0;
}