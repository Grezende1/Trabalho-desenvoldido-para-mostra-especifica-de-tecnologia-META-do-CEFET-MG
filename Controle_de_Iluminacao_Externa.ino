/* Implementação do sensor LDR para iluminação
 * Interface LDR com NodeMCU
 * Criado por Gustavo Valadares Castro e Gabriel Fernandes Rezende
 * Membros do projeto: Gustavo Castro, Gabriel Rezende e Letícia Capanema (ELE2A)
 * Revisão: 8
 * Data: 29/07/2019
 */
//Valor inicial para a variável de leitura e as demais
int sensor_operation = 0;
int sensorValue = 0;
int pwm;

void setup() {
  // Inicia comunicação serial a 9600 bit por segundo:  
  Serial.begin(9600);

  // Define o pino D5 como saída:
  pinMode(D5, OUTPUT);
}

void loop() {
  //Mostra no monitor serial, o valor medido no formato de 10 bits
  Serial.println(sensorValue);
  
  //Envia para a saída PWM D5 um sinal correspondendo a operação do valor medido pelo sensor:
  analogWrite(D5, pwm);
  
  //Lê os valores da entrada analógica A0 e o processa para que seja usado na saída PWM:
  sensorValue = analogRead(A0);  
  sensor_operation = (sensorValue - 100)*3;

  //Estipula-se um valor para o input, para que valores abaixo desse (maior brilho externo), os LEDs não serâo ligados:
  if(sensorValue < 150) { 
    pwm = 0;
  }
  //Atualiza o valor da variável pwm:
  else {
    (pwm = sensor_operation);
  }

delay(200);
}
