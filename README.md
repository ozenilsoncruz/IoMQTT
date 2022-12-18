# IoTPlatform

<details>
<summary>Texto do Problema</summary>

---

## Tema

Protótipo de um sistema IoT.

## Objetivos de Aprendizagem

Ao final da realização deste problema, o/a discente deverá ser capaz de:

- Entender como integrar sensores com aplicações através do protocolo MQTT;
- Compreender o uso e funcionalidades do protocolo MQTT;
- Assimilar conceitos básicos sobre integração de sistemas.

## Problema

No problema anterior foi implementado um protótipo de sistema de sensoriamento genérico. Como na fase de protótipo foi utilizada uma plataforma baseada na NodeMCU, que possui interface de comunicação sem fio, agora lhe é solicitado o desenvolvimento de um sistema com sensores sem fio.

Neste problema, o sistema continuará comandado pelo Single Board Computer (SBC), e todos os requisitos anteriormente solicitados para o sistema com a UART devem ser atendidos. MI - Sistemas Digitais (2022.2) 1 O protótipo atual deve incluir agora uma IHM (Interface Homem-Máquina) para apresentação das informações, em tempo real, das leituras atuais dos sensores. Ela também deve permitir a visualização do histórico com as 10 últimas medições de cada sensor, preferencialmente na forma de um gráfico temporal. Além disso, a IHM deve ser usada para o ajuste local e remoto do intervalo de tempo em que serão realizadas as medições

Finalmente, o SBC também deverá ser utilizado como centralizador das informações para que os dados possam ser acessados através da internet.

---

</details>

## Autores
<div align="justify">
    <li><a href="https://github.com/ozenilsoncruz">@Ozenilson Cruz</a></li>  <li><a href="https://github.com/traozin">@Antônio Neto</a></li>
</div>

##

<details>
<summary>Manual do Problema</summary>

### Requisitos 

O hardware usado no teste foi uma Orange Pi e uma NodeMCU ESP-8266

- **Orange Pi** [^orange]
  - CPU - H3 Quad-core Cortex-A7 H.265/HEVC 4K
  - GPU	- Mali400MP2 GPU @600MHz
  - Memory (SDRAM)	1GB DDR3 (shared with GPU)
  - 8GB eMMC Flash

- **NodeMCU ESP-8266** [^nodemcu]
  - Módulo NodeMcu Lua ESP-12E
  - Memória flash: 4 MB
  - Wireless padrão 802.11 b/g/n
  - Antena embutida
  - Suporta 5 conexões TCP/IP
  - Portas GPIO: 13
  - D0 (GPIO16) só pode ser usado como entrada ou saída, não suporta outras funções (interrupção, PWM, I2C, etc)

### Instruções para instalação

Esse projeto necessita que o usuário tenha a IDE do Arduino instalada em sua máquina, necessária para comunicação com a NodeMCU. Para isso, siga as instruções do link abaixo:

<details>
<summary>Configuração do ambiente arduino</summary>

- Siga os passos para fazer a [Instalação do Arduino IDE](https://www.arduino.cc/en/Guide/Windows#toc4).
- Siga os passos para fazer a [Instalação do Driver da NodeMCU](https://www.blogdarobotica.com/2020/05/26/instalando-driver-serial-para-nodemcu-com-chip-ch340/).
  
</details>

##### Após configuração do ambiente, siga os passos abaixo:

**1.** Siga os passos para [Adicionar Bibliotecas na IDE Arduino](https://www.robocore.net/tutoriais/adicionando-bibliotecas-na-ide-arduino#:~:text=Dispon%C3%ADvel%20na%20IDE%20do%20Arduino,Include%20Library).
**2.** Baixe o arquivo `main_esp.ino` e faça upload para o NodeMCU.
**3.** Faça as devidas configurações de Broker e rede no arquivo `main_esp.ino`.
**4.** Em uma Orange Pi e em um computador pessoal, clone o repositório.
   ```sh
   git clone https://github.com/ozenilsoncruz/IoMQTT
   ```
**5.** Na  Orange Pi utilizando o terminal, navegue para a pasta do repositório e execute os passos abaixo: 
  - Faça as devidas configurações de Broker no arquivo `mqtt_sbc.h`.
  - Compile utilizando o Makefile com o comando:
    ```sh
        make
    ```
  - Execute com o comando:
    ```sh
        sudo ./main
    ```
**6.** No computador, navegue para a pasta do repositório e execute os passos abaixo:
  - Faça as devidas configurações de Broker no arquivo `ihm.py`.
  - Execute com o comando:
    ```sh
        python3 -m pip install -r requirements.txt 
    ```
  - Execute o script `ihm.py`:
    ```sh
        python3 ihm.py 
    ```
</details>

## Introdução

## Metodologia

#### Protocolo MQTT [^MQTT1]

Desenvolvido na década de 90 pela IBM e Eurotech, o MQTT (Message Queuing Telemetry Transport e tendo tradução para português sob o nome de Transporte de Filas de Mensagem de Telemetria) é um protocolo de mensagens que foi criado com o objetivo de oferecer um baixo consumo de rede, banda e também dos demais recursos de software. O formato utilizado no MQTT é de Cliente/Servidor.

Por esse motivo e também por ter fundamentos na pilha TCP/IP ou em outros protocolos de rede, o MQTT tem extrema utilidade dentro da área de desenvolvimento de projetos de comunicação entre máquinas, também conhecido pelo termo M2M (Machine to Machine). Outra área também onde se torna muito presente é para conectividade de IoT (Internet of Things).

#### Como funciona ?

Para funcionar, o Protocolo MQTT utiliza um modelo de Publish/Subscribe onde permite que o cliente faça postagens e/ou capte informações enquanto o servidor irá administrar esse envio e o recebimento dos respectivos dados.

Ou seja, em um MQTT haverá um publicador onde será responsável por publicar as mensagens em um determinado tópico onde um assinante irá inscrever-se neste tópico para poder acessar a mensagem.

Como não há uma conexão direta entre o assinante e o publicador, para que essas mensagens aconteçam, o protocolo MQTT irá precisar de um gerenciador de mensagens chamado de Broker.

#### Formas de comunicação

Em um Protocolo MQTT, a publicação e o recebimento de dados são realizados através de um tipo de servidor chamado de Broker. Ou seja, nessa comunicação haverá então um cliente que terá o papel de Publisher (Publicador) e transmitirá a mensagem, com um tópico de destino e o seu Payload (o conteúdo da mensagem).

Esta mensagem será então encaminhada ao Broker, que se encarregará de gerenciá-la e entregá-la ao Participante que anteriormente se inscreveu no tópico.

Na mesma linha, se um cliente pretender tornar-se subscritor de um tópico, apenas terá de passar o pedido ao Broker, que poderá estabelecer uma ligação entre o cliente e o tópico em questão.

De forma simplificada, essa comunicação pode ser dividida entre os seguintes termos:

- **Subscriber** (Subscrito) – a pessoa que estará inscrita no tópico e irá ter o papel de receptor.
- **Publisher** (Publicador) – a pessoa que estará responsável por ser o emissor e enviar os dados para um determinado tópico.
- **Broker** – será o intermediário para fazer uma ponte de comunicação entre o Publisher e o Subscriber, se tornando responsável por fazer o recebimento, enfileiramento e envio das mensagens.
- **Tópico** –será o endereço pelo qual os dados das mensagens serão enviados.
- **Client** (Cliente) – será o elemento que terá a capacidade de interagir com o Broker, podendo enviar e receber dados.
- **Mensagem** – será o pacote de dados trocados entre os clientes e o Broker.
- **Unsubscribe** – permite deixar de assinar um determinado tópico.
- **Payload** – será o conteúdo da mensagem que será enviada.

## Testes

Para averiguar o funcionamento correto do projeto implementado os seguintes testes foram realizados:

- Solicitação do status da NodeMCU;
- Envio de uma mensagem não esperada pela NodeMCU;
- Acendimento e apagamento do LED múltiplas vezes;
- Verificação da veracidade dos dos dados no histórico;
- Verificação do intervalo de tempo dos estados dos pinos automaticamente;
- Solicitação do valor das entradas digitais múltiplas vezes enquanto o valor do pino é alterado;
- Solicitação do valor da entrada analógica múltiplas vezes enquanto o valor do pino é alterado;


## Conclusão
Por meio deste projeto, conceitos importantes de comunicação mqtt foram devidamente incorporados e compreendidos para a solução, bem como a utilidade da utilização de microcontroladores para diversos tipos de aplicações.

O código deste projeto é capaz de resolver o problema apresentado utilizando de bibliotes nativas das linguagens C e Arduino. Toda via, algumas das soluções podem não apresentar o melhor funcionamento possível pois nem todas foram devidamente testadas.


[^MQTT1]: Compreender UART - [automacaoindustrial.info](https://www.automacaoindustrial.info/mqtt/l)

[^nodemcu]: NodeMCU ESP8266-12 V2 Especificações - [robocore.net](https://www.robocore.net/wifi/nodemcu-esp8266-12-v2)

[^orange]: Orange Pi PC Plus - [orangepi.org](http://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/details/Orange-Pi-PC-Plus.html)