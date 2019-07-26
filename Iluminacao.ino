/* Implementação do sensor LDR para iluminação
 * Interface LDR com Arduino Uno
 * Criado por Gustavo Valadares Castro e Gabriel Fernandes Rezende
 * Membros do projeto: Gustavo Castro, Gabriel Rezende e Letícia Capanema (ELE2A)
 * Revisão: 5
 * Data: 25/07/2019
 */
// Declara o pino de saída PWM para alimentação dos LEDs
  #define saida 5 
  
void setup() {
  // Inicia comunicação serial a 9600 bit por segundo:
  Serial.begin(9600);

  // Define o pino 5 como saída:
  pinMode(saida, OUTPUT);

}

void loop() {
  // Lê a entrada no pino analógico A0:
  int sensorValue = analogRead(A0);
  
  // Converte a leitura analógica (valores entre 0 e 1023) para a tensão 0 - 1V (com 0V para o máximo valor de luminosidade e 1V para o menor):
  float voltage = sensorValue * (1.0 / 1023.0);
  
  // Mostra no painel serial a leitura dos valores de tensão:
  Serial.println(voltage);

  //Estipulando um valor temporário de 0.5V, para valores abaixo desse, os LEDs não serâo ligados
  if (voltage < 0.5) {
    digitalWrite(saida,LOW);
    }

  //Envia para a saída PWM D5 um sinal variável correspondendo aos valores da leitura serial (0 - 12V):
  analogWrite(saida, 3.3*voltage);
}
