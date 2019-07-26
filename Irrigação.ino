/* Implementação do Sensor de Umidade (Higrômetro) para iluminação
 * Interface Higrômetro com Arduino Uno
 * Criado por Gustavo Valadares Castro e Gabriel Fernandes Rezende
 * Membros do projeto: Gustavo Castro, Gabriel Rezende e Letícia Capanema (ELE2A)
 * Revisão: 1
 * Data: 25/07/2019
 */
 
// Desfinição dos pinos:  
#define pinSensorD 2 // Define a o pino para onde o sensor enviará o sinal lógico
#define rele_bomba 12 // Define a saída para o relé como o pino 12

void setup() {
  // Define o pino 2 como entrada
  pinMode(pinSensorD, INPUT);

  // Inicia comunicação serial a 9600 bit por segundo:
  Serial.begin(9600);
}

void loop() {
  // Mostra no monitor serial, a condição em que o solo se encontra:
  Serial.print("Condição:");
  
  if (digitalRead(pinSensorD)) {
     Serial.print("SEM UMIDADE ");
  } else {
     Serial.print("COM UMIDADE ");
  }
  
  // Para valores de umidade menores que a calibração do sensor, ele apresentará nível lógico LOW na saída, indicando a necessidade de ligar a bomba
  if (digitalRead(pinSensorD) == LOW) {
     Serial.println("Bomba LIGADA");
     digitalWrite(rele_bomba, HIGH);
  } else {
    Serial.println("Bomba DESLIGADA");
    digitalWrite(rele_bomba, LOW);
  }
  delay(2000);
}
