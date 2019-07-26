/* Implementação do Sensor de Temperatura e Umidade (DHT22) para iluminação
 * Interface DHT22 com Arduino Uno
 * Criado por Gustavo Valadares Castro e Gabriel Fernandes Rezende
 * Membros do projeto: Gustavo Castro, Gabriel Rezende e Letícia Capanema (ELE2A)
 * Revisão: 2
 * Data: 25/07/2019
 */
 
//Inclusão das bibliotecas
#include <dht.h>

//Definição dos pinos:
#define pinSensor 7 // Define a entrada digital por onde são enviados os sinais do sensor
#define rele_servo 8 // Define a saída para o relé como o pino 8

//Define o Intevalo de leitura
#define intervalo 2000 

//Cria-se variáveis e se instanciam-se os objetos:
unsigned long delayIntervalo;
dht sensorDHT;

void setup()
{
  // Define os pinos como entrada ou saída:
  pinMode(rele_servo, OUTPUT);
  pinMode(pinSensor, INPUT);
  
  // Inicial monitor serial:
  Serial.begin(9600);

  // Mostra informações sobre a biblioteca no monitor:
  Serial.print("VERSAO DA BIBLIOTECA: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Status,\tTempo(uS),\tUmidade(%),\tTemperatura(C)");

}

void loop() 
{

    if ( (millis() - delayIntervalo) > intervalo ) {
      
      // Lê os dados do sensor:
      unsigned long start = micros();
      int chk = sensorDHT.read22(pinSensor);
      unsigned long stop = micros();
  
      // Verifica a ocorrência de erros:
      switch (chk)
      {
      case DHTLIB_OK:
          Serial.print("OK,\t");
          break;
      case DHTLIB_ERROR_CHECKSUM: 
          Serial.print("Checksum error,\t");
          break;
      case DHTLIB_ERROR_TIMEOUT: 
          Serial.print("Time out error,\t");
          break;
      case DHTLIB_ERROR_CONNECT: 
          Serial.print("Connect error,\t");
          break;
      case DHTLIB_ERROR_ACK_L: 
          Serial.print("Ack Low error,\t");
          break;
      case DHTLIB_ERROR_ACK_H: 
          Serial.print("Ack High error,\t");
          break;
      default:
          Serial.print("Unknown error,\t");
          break;
      }
    
      // Exibe dados lidos pelo DHT no painel serial
      Serial.print(stop - start);
      Serial.print(", \t\t");
      Serial.print(sensorDHT.humidity, 1 /*Formatação para UMA casa decimal*/);
      Serial.print(", \t\t");
      Serial.println(sensorDHT.temperature, 1 /*Formatação para UMA casa decimal*/);

      //Caso a temperatura e ou umidade sejam menores que 20ºC e maiores que 80% respectivamente, será enviando um sinal para o módulo relé que acionará o servo responsável pela abertura e fechamento da janela
      if (sensorDHT.humidity > 80.0) {
        digitalWrite(rele_servo, HIGH);
        }
      if (sensorDHT.temperature <20.0) {
        digitalWrite(rele_servo, HIGH);
        }
      else {
        digitalWrite(rele_servo, LOW);
      }
    }
      delayIntervalo = millis();
}
