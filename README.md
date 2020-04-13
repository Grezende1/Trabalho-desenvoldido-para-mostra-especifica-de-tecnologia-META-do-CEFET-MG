# Projeto de Automação Residencial para META e IoT
## Repositório dedicado ao desenvolvimento do software responsável pelo controle das funções de uma maquete didática via NodeMCU
### Sumário
* Descrição
* Pré-Requisitos
* Funcionalidades
* Créditos
### Descrição
Esse projeto encontra-se no inicio do desenvolvimento, e se trata da maquete de uma casa, na qual certas funções como irrigação, controle do portão e das janelas e iluminação externa, serão controladas por meio de um microcontrolador denominado ESP8266 na plataforma nodeMCU. 
### Pré-Requisitos
Foi utilizado o microcontrolador ESP8266 na plataforma nodeMCU. O programa foi desenvolvido na Arduino IDE, utilizando uma configuração de placa disponibilizada pelos desenvolvedores do ESP8266.

Segue abaixo, o link para o download da Arduino IDE e da configuração do nodeMCU para ser adicionado a sua IDE por meio do Gerenciador de Placas:
```
https://www.arduino.cc/en/Main/Software
https://github.com/esp8266/Arduino
```
### Funcionalidades 
* Controle de Irrigação: Aciona a bomba de irrigação no gramado automaticamente, quando o solo estiver muito seco. A umidade do solo será mensurada por um Sensor de Umidade do Solo Higrômetro composto pelo comparador LM393, e o acionamento será feito via módulo relé para arduino.

* Controle de Iluminação Externa: Acende, apaga e dimeriza as lâmpadas presentes na área externa de acordo com o nível de luminosidade do ambiente. O sensor ultilizado será um LDR.

* Controle de Janela Automatizado: Fecha uma janela da fachada ao detectar chuva ou em temperaturas muito frias, o sensor utilizado, será um DHT11 (sensor de umidade e temperatura) e o acionamento será feito via módulo relé para arduino [*Ainda encontra- se em planejamento, pois não foi achada ainda uma biblioteca compatível com o nodeMCU*].

* Abertura Automática do Portão atraves de um app: Controla um portão usando das funcionalidades wireless do ESP8266, que permitem fazê-lo por meio de aplicativos. O acionamento será feito via módulo relé para arduino [*Ainda encontra-se em planejamento*].
### Créditos
O projeto está sendo desenvolvido por:
* Gustavo Valadares Castro - Aluno do Terceiro Ano de Eletrotécnica no CEFET MG
* Gabriel Fernandes Rezende - Aluno do Terceiro Ano de Eletrotécnica no CEFET MG
* Letícia Capanema - Aluna do Segundo Ano de Eletrotécnica do CEFET  MG
