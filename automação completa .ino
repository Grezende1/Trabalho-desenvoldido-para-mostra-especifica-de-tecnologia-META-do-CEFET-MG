/*Esse Programa posssui direitos autorais
 * Programa de Automação Residencial para META e IoT
 * Criado por Gustavo Valadares Castro e Gabriel Fernandes Rezende
 * Membros do projeto: Gustavo Castro, Gabriel Rezende e Letícia Capanema (ELE2A)
 * Revisão e Data: 42, 21/10/2019
 * Data: 21/10/2019 
 ******************************************************************************** 
 * Lista de GPIOs:
 * GPIO0 = D3 - Saída, Servo Motor Janela
 * GPIO1 = TX
 * GPIO2 = D4 - Saída, Servo Motor Portão
 * GPIO3 = RX
 * GPIO4 = D2 - Saída, Ilminação Interna (Piso 1)
 * GPIO5 = D1 - Saída, Relé da bomba
 * GPIO9 = SD2  
 * GPIO10 = SD3
 * GPIO12 = D6 - Saída, Iluminação Externa Dimerizada
 * GPIO13 = D7 - Saída, Iluminação Interna (Piso 2)
 * GPIO14 = D5 - Entrada, Sensor DHT11 (Umidade e Temperatura)
 * GPIO15 = D8
 * GPIO16 = D0 - Entrada, Sensor de Umidade do Solo
 * ADC = Entrada do LDR
 */

/* Parâmetros da rede */
#include <ESP8266WiFi.h>                                                                        // Inclusão da biblioteca necessária para as funcionalidades wifi
#include <Servo.h>                                                                              // Inclusão da biblioteca necessária para as funcionalidades do servo motor
#include <DHT.h>                                                                                // Inclusão da biblioteca necessária para as funcionalidades do sensor DHT11
#include <DHT_U.h>                                                       

IPAddress ip(192,168,15,13);                                                                    // Ip do node
IPAddress Gateway(192,168,15,251);                                                              // Portão de comunicação 
IPAddress Subnet(255,255,255,0);                                                                // Máscara da rede

const char* ssid = "IotELE";                                                                    // Variável que armazena o nome da rede sem fio em que vai conectar
const char* password = "b3a9471a0e";                                                            // Variável que armazena a senha da rede sem fio em que vai conectar

int i, j, k, toggleJ, toggleP, toggleI1, toggleI2, countClient, clientes;              // Cria variáveis inteiras referentes a vetores, retenção lógica, etc
String ligaJ, desligaJ, ligaP, desligaP, ligaI1, desligaI1, ligaI2, desligaI2; // Cria strings que estão relacionados aos comandos lógicos
String analogL, analogT, analogU;                                                               // Cria strings que estão relacionados às medidas analógicas
float somaT, somaU, vet[101];                                                                   // Cria variáveis tipo float referentes a soma dos incrementos
float mediaT, mediaU;                                                                           // Cria variáveis tipo float referentes a média aritmética das medições

WiFiServer server(80);    // Caso ocorra problemas com a porta 80, utilize outra (ex.: 8082, 8089, etc) e a chamada da URL ficará ip: Porta (ex.:192.168.0.15:8082)

/* Pinout e variáveis servos */
Servo servo_motorJ;       // Declara um servo com o nome de servo_motorJ (J = janela)
Servo servo_motorP;       // Declara um servo com o nome de servo_motorP (P = portão)

/* Pinout sensor de umidade e temperatura */
DHT dht(D5, DHT11);       // Define pino D5 para sensor DHT11

/* Pinout irrigação */
#define sensor_solo D0    // Define pino D0 para sensor de umidade do solo
#define rele_bomba D1     // Define pino D1 para relé da bomba

/*Pinout iluminação interna*/
#define leds_interno1 D2  // Define pino D2 para relé iluminação interna (piso 1)
#define leds_interno2 D7  // Define pino D7 para relé iluminação interna (piso 2)

/* Pinout e variáveis iluminação externa */
int sensor_operation = 0; // Valor inicial para variável processada
int sensor_value = 0;     // Valor inicial da leitura do LDR
int pwm;                  // Declaração de variável para saída pwm

#define leds_externo D6   // Define pino D8 para iluminação externa

/* Fim da declaração de variáveis */

void setup() {
  /* Zera o vetor de temperatura */
  for ( i=0; i <= 100 ; i++ ){
     vet[i] = 0;
     }

  /* Zera o vetor de umidade */
  for ( j=0; j <= 100 ; j++ ){
     vet[j] = 0;
     }

  /* Setuup servos */
  servo_motorJ.attach(D3);          // Define o pino servo_motor como saída
  servo_motorP.attach(D4);                    
 
  /* Setup sensor de umidade e temperatura*/
  dht.begin();                      // Inicia o DHT
                   
  /* Setup irrigação */
  pinMode(sensor_solo,  INPUT);
  pinMode(rele_bomba,   OUTPUT);

  /* Setup iluminação interna */
  pinMode(leds_interno1, OUTPUT);
  pinMode(leds_interno2, OUTPUT);

  /* Setup iluminação externa */
  pinMode(leds_externo, OUTPUT);

  /* Setup do status inicial dos pinos */    
  servo_motorJ.write(170);                      
  servo_motorP.write(180);
  digitalWrite(rele_bomba,    LOW);   
  digitalWrite(leds_interno1, LOW);
  digitalWrite(leds_interno2, LOW);   
  digitalWrite(leds_externo,  LOW);
    
  /* Setup do node */
  Serial.begin(9600);                               // Inicializa o serial
  delay(10);                                        // Intervalo de 10 milissegundos

  Serial.println("");                               // Pula uma linha da janela serial
  Serial.println("");                        
  Serial.print("Conectando a ");                    // Escreve o texto na serial
  Serial.print(ssid);                               // Escreve o nome da rede na serial

  WiFi.begin(ssid,password);                        // Passa os parâmetros para a função que vai fazer a conexão com a rede sem fio
  WiFi.config(ip,Gateway,Subnet);
    while (WiFi.status() != WL_CONNECTED) {         // Enquanto o status for diferente de conectado
       delay(500);                                  // Intervalo de 500 milissegundos
       Serial.print(".");                           // Escreve o caracter na serial
       };
               
  Serial.println("");
  Serial.print("Conectado a rede sem fio ");        // Escreve o texto na serial
  Serial.println(ssid);                             // Escreve o nome da rede na serial
  server.begin();                                   // Inicia o servidor para receber dados na porta definida em: "WiFiServer server(porta);"
  Serial.println("Servidor iniciado");              // Escreve o texto no serial

  Serial.print("IP para se conectar ao NodeMCU: "); // Escreve o texto no serial: "IP para se conectar ao NodeMCU"
  Serial.print("http://");                          // Escreve em seguida: "http://"
  Serial.println(WiFi.localIP());                   // Escreve em seguida, o ip recebido dentro da rede sem fio (o ip é recebido de forma automática)
  
  delay(1000);
}

void loop() {
  /* Página HTML */
  WiFiClient client = server.available();           // Verifica se algum cliente está conectado no servidor
    if (!client) {                                  // Se não existir cliente conectado, reexecuta o processo até que algum se conecte ao servidor
       return;
       }
       
  Serial.println("Novo cliente se conectou!");      // Escreve o texto no serial: "Novo cliente se conectou!"
    while(!client.available()){                     // Enquanto o cliente estiver conectado, executar as operações dentro do loop
       char ch = static_cast<char>(client.read());
       Serial.print(ch);
       countClient = clientes + 1;
       delay(1);                                    // Intervalo de 1 milissegundo
       }

  String request = client.readStringUntil('\r');    // Faz a leitura da primeira linha de requisição
  Serial.println(request);                          // Escreve a requisição na serial
  client.flush();                                   // Aguarda até que todos os dados de saída sejam enviados ao cliente

  String html  = "<html><head><title>NODEMCU</title>";
  html += "<meta charset=\"utf-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html  +="<meta http-equiv=\"refresh\" content=\"5\">";

  html += "<style>";
  html += "*{ box-sizing: border-box;}";
  html += "body { font-family: Arial, Helvetica, sans-serif;}";
  html += "header {";
  html += "background-color: darkblue;";
  html += "padding: 0.30px;";
  html += "text-align: center;";
  html += "font-size: 15px;";
  html += "color: yellow;}";

  /* cria duas colunas na paginar */
  html += "nav {";
  html += "float: left;";
  html += "width: 10%;";
  html += "height: 620px;";
  html += "background: #ccc;";
  html += "padding: 10px;}";

  /* Style the list inside the menu */
  html += "nav ul {";
  html += "list-style-type: none;";
  html += "padding: 0;}";

  html += "article {";
  html += "float: left;";
  html += "padding: 20px;";
  html += "width: 90%;";
  html += "background-color: #f1f1f1;";
  html += "height: 620px;}";

  /* Clear floats after the columns */
  html += "section:after {";
  html += "content: "";";
  html += "display: table;";
  html += "clear: both;}";

  /* Style do rodapé */
  html += "footer {";
  html += "background-color: darkblue;";
  html += "padding: 1px;";
  html += "text-align: center;";
  html += "color: white;}";

  /* layout */
  html += "@media (max-width: 300px) {";
  html += "nav, article {";
  html += "width: 100%;";
  html += "height: auto;  }";
  html += "}";

  html += "</style>";
  html += "</head>";

  html += "<body>";
  html += "<p>";
  html += "<header id=\"principal\" >";
  html += "<h3>CENTRO FEDERAL DE EDUCAÇÃO TECNOLÓGICA DE MINAS GERAIS - CEFET MG <p><i>CURSO TÉCNICO DE ELETROTÉCNICA</i></p></h3>";
  html += "</header>";
  html += "<section>";
  html += "<nav>";
  html += "<ul>";
  html += "<li><a href=\"#principal\">Supervisório do Projeto</a></li>";
  html += "<p> </p>";
  html += "</ul>";
  html += "</nav>";
 
  html += "<article >";
  html += "<h3>Supervisório IoT ESP8266 -  Casa Inteligente </h3>";
 
  html += "<p><style>   table, th , td {border: 5px solid grey;}</style></p>";
  html += "<p><table>   <colgroup> <col span=\"4\" style=\"background-color:grey\"> </colgroup></p>";
  html += "<p><tr>      <th>I/O</th> <th>Endereço</th> <th>Descrição</th> <th>Valor</th> <th>Comando</th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio00</th> <th> Comando da Janela              </th> <th id = \"GPio00\"></th> <th><a href=\"?fuction = ligaJ\"><button onclick = ResultonJ()>Janela ON</button></a><a href=\"?fuction = desligaJ\"><button onclick = ResultoffJ()>Janela OFF</button></a> </th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio02</th> <th> Comando do Portão              </th> <th id = \"GPio02\"></th> <th><a href=\"?fuction = ligaP\"><button onclick = ResultonP()>Portão ON</button></a><a href=\"?fuction = desligaP\"><button onclick = ResultoffP()>Portão OFF</button></a> </th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio04</th> <th> Comando da Iluminação (Piso 1) </th> <th id = \"GPio04\"></th> <th><a href=\"?fuction = ligaI1\"><button onclick = ResultonI1()>Interna1 ON</button></a><a href=\"?fuction = desligaI1\"><button onclick = ResultoffI1()>Interna1 OFF</button></a> </th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio13</th> <th> Comando da Iluminação (Piso 2) </th> <th id = \"GPio13\"></th> <th><a href=\"?fuction = ligaI2\"><button onclick = ResultonI2()>Interna2 ON</button></a><a href=\"?fuction = desligaI2\"><button onclick = ResultoffI2()>Interna2 OFF</button></a> </th></p>";
  html += "<p><tr>     <th>Output</th> <th>GPio05</th> <th> Status da Irrigação             </th> <th id = \"GPio05\"></th> </p>";
  html += "<p><tr>      <th>Output</th> <th>GPio12</th> <th> Status Iluminação Externa      </th> <th id = \"GPio12\"></th> </p>";
  html += "<p><tr>      <th>Input</th> <th>GPio14</th> <th> Temperatura (°C)                </th> <th><a href=\"?fuction=analogT\"></a> </p>";
  html += mediaT;
  html += "</th></p>";
  html += "<p><tr>      <th>Input</th> <th>GPio14</th> <th> Umidade do Ar (%)              </th> <th><a href=\"?fuction=analogU\"></a> </p>";
  html += mediaU;
  html += "</th></p>";
  html += "<p></table></p>";
  
  html += "</th></p>";
  html += "<p></table></p>";
 
  html += "</article>";
  html += "</section>";

  html += "<footer>";
  html += "<h8><i>Coordenação de Eletrotécnica - CEFET MG - Todos os direitos reservados &#9879</i></h8>";
  html += "</footer>";

  html += "<br class=\"quebrapagina\">";
  html += "</body>";

  html += "</html>";

  client.flush();
  client.print(html);
  
  delay(1);                                      // Intervalo de 1 milissegundo
  Serial.println("Cliente desconectado:");       // Escreve o texto no serial: "Cliente desconectado"
  Serial.println("");
  
  /* Programa iluminação externa */
  sensor_value = analogRead(A0);               // Lê o valor de 10 bits na entrada analógica A0
  sensor_operation = (sensor_value - 100)*3;   // Processa o valor de modo que a saída pwm funcione como desejado
     
  if(sensor_value < 150) {                     // Para níveis de luminosidade inferiores ao valor, desligar a saída
     pwm = 0;
     }
     else {
        (pwm = sensor_operation);                 // Atualiza o valor da variável pwm
        }
     
  analogWrite(leds_externo, pwm);              // Joga para a saída o valor atualizado de pwm

  if (sensor_value > 150) {
     client.println("<script> document.getElementById(\"GPio12\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio12\").style.backgroundColor=\"green\"</script>");
     }
  if (sensor_value <= 150) {
     client.println("<script> document.getElementById(\"GPio12\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio12\").style.backgroundColor=\"red\"</script>");
     }
     
  /* Programa sensor de umidade e temperatura */ 
  if (request.indexOf("analogT") != 1){
     float temperatura = dht.readTemperature();
     somaT = 0;
     mediaT = 0;
     
     for ( i = 0; i <= 99; i++) {                 // Executa um pequeno loop em que se coleta 100 valores percentuais da entrada e forma-se um vetor
        temperatura = vet[i];     
        somaT = somaT + vet[i];
        delay(1);
        }
        mediaT = somaT/100;
        Serial.println(mediaT);        
     }
     
  if (request.indexOf("analogU") != 1){
     float umidade = dht.readHumidity();
     mediaU = 0;
     somaU = 0;
     
     for ( j = 0; j <= 99; j++) {                 // Executa um pequeno loop em que se coleta 100 valores percentuais da entrada e forma-se um vetor
        umidade = vet[j];     
        somaU = somaU + vet[j];
        delay(1);
        }
        mediaU = somaU/100;
        Serial.println(mediaU);
     }

  /* Programa iluminação interna */
  /* Piso 1 */
  if (request.indexOf("ligaI1") != -1) {
     digitalWrite(leds_interno1, HIGH);
     toggleI1 = 1;
     }

  if (request.indexOf("desligaI1") != -1) {
     digitalWrite(leds_interno1, LOW);
     toggleI1 = 2;
     }

  if((toggleI1 == 1)&&(request.indexOf("ResultonI1") != 1)){
     client.println("<script> document.getElementById(\"GPio04\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio04\").style.backgroundColor=\"green\"</script>");
     }

  if((toggleI1 == 2)&&(request.indexOf("ResultoffI1") != 1)){
     client.println("<script> document.getElementById(\"GPio04\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio04\").style.backgroundColor=\"red\"</script>");
     }
     
  /* Piso 2 */
  if (request.indexOf("ligaI2") != -1) {
     digitalWrite(leds_interno2, HIGH);
     toggleI2 = 1;
     }

  if (request.indexOf("desligaI2") != -1) {
     digitalWrite(leds_interno2, LOW);
     toggleI2 = 2;
     }

  if((toggleI2 == 1)&&(request.indexOf("ResultonI2") != 1)){
     client.println("<script> document.getElementById(\"GPio13\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio13\").style.backgroundColor=\"green\"</script>");
     }

  if((toggleI2 == 2)&&(request.indexOf("ResultoffI2") != 1)){
     client.println("<script> document.getElementById(\"GPio13\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio13\").style.backgroundColor=\"red\"</script>");
  } 
     
  /* Programa irrigação */
  Serial.print("Condição:");
  
  if (digitalRead(sensor_solo) == LOW) {  //
     Serial.print(" SEM UMIDADE ");
     digitalWrite(rele_bomba, HIGH);
     } 
  else {
     Serial.print(" COM UMIDADE ");
     digitalWrite(rele_bomba, LOW);
     }
 
  if (digitalRead(rele_bomba) == HIGH) {  // 
     Serial.println(" - Bomba LIGADA");
     } 
  else {
     Serial.println(" - Bomba DESLIGADA"); //
     }
     
  if (digitalRead(rele_bomba == HIGH)){
     client.println("<script> document.getElementById(\"GPio05\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio05\").style.backgroundColor=\"green\"</script>");
     }

  if (digitalRead(rele_bomba == LOW)){
     client.println("<script> document.getElementById(\"GPio05\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio05\").style.backgroundColor=\"red\"</script>");
     }
  
  /* Programa controle dos servos */
  /* Janela */
  if (request.indexOf("ligaJ") != -1) {
     servo_motorJ.write(80);
     toggleJ = 1;
     }

  if (request.indexOf("desligaJ") != -1) {
     servo_motorJ.write(170);
     toggleJ = 2;
     }

  if ((toggleJ == 1)&&(request.indexOf("ResultonJ") != 1)){
     client.println("<script> document.getElementById(\"GPio00\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio00\").style.backgroundColor=\"green\"</script>");
     }

  if ((toggleJ == 2)&&(request.indexOf("ResultoffJ") != 1)){
     client.println("<script> document.getElementById(\"GPio00\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio00\").style.backgroundColor=\"red\"</script>");
     }
  
  /* Portão */
  if (request.indexOf("ligaP") != -1) {
     servo_motorP.write(90);
     toggleP = 1;
     }

  if (request.indexOf("desligaP") != -1) {
     servo_motorJ.write(180);
     toggleP = 2;
     }

  if ((toggleP == 1)&&(request.indexOf("ResultonP") != 1)){
     client.println("<script> document.getElementById(\"GPio02\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio02\").style.backgroundColor=\"green\"</script>");
     }

  if ((toggleP == 2)&&(request.indexOf("ResultoffP") != 1)){
     client.println("<script> document.getElementById(\"GPio02\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio02\").style.backgroundColor=\"red\"</script>");
     }

  delay(100);
  
  client.flush();

  delay(1);
}
/* Programa de Automação Residencial para META e IoT
 * Criado por Gustavo Valadares Castro e Gabriel Fernandes Rezende
 * Membros do projeto: Gustavo Castro, Gabriel Rezende e Letícia Capanema (ELE2A)
 * Revisão e Data: 42, 21/10/2019
 * Data: 21/10/2019 
 ******************************************************************************** 
 * Lista de GPIOs:
 * GPIO0 = D3 - Saída, Servo Motor Janela
 * GPIO1 = TX
 * GPIO2 = D4 - Saída, Servo Motor Portão
 * GPIO3 = RX
 * GPIO4 = D2 - Saída, Ilminação Interna (Piso 1)
 * GPIO5 = D1 - Saída, Relé da bomba
 * GPIO9 = SD2  
 * GPIO10 = SD3
 * GPIO12 = D6 - Saída, Iluminação Externa Dimerizada
 * GPIO13 = D7 - Saída, Iluminação Interna (Piso 2)
 * GPIO14 = D5 - Entrada, Sensor DHT11 (Umidade e Temperatura)
 * GPIO15 = D8
 * GPIO16 = D0 - Entrada, Sensor de Umidade do Solo
 * ADC = Entrada do LDR
 */

/* Parâmetros da rede */
#include <ESP8266WiFi.h>                                                                        // Inclusão da biblioteca necessária para as funcionalidades wifi
#include <Servo.h>                                                                              // Inclusão da biblioteca necessária para as funcionalidades do servo motor
#include <DHT.h>                                                                                // Inclusão da biblioteca necessária para as funcionalidades do sensor DHT11
#include <DHT_U.h>                                                       

IPAddress ip(192,168,15,13);                                                                    // Ip do node
IPAddress Gateway(192,168,15,251);                                                              // Portão de comunicação 
IPAddress Subnet(255,255,255,0);                                                                // Máscara da rede

const char* ssid = "IotELE";                                                                    // Variável que armazena o nome da rede sem fio em que vai conectar
const char* password = "b3a9471a0e";                                                            // Variável que armazena a senha da rede sem fio em que vai conectar

int i, j, k, toggleJ, toggleP, toggleI1, toggleI2, countClient, clientes;              // Cria variáveis inteiras referentes a vetores, retenção lógica, etc
String ligaJ, desligaJ, ligaP, desligaP, ligaI1, desligaI1, ligaI2, desligaI2; // Cria strings que estão relacionados aos comandos lógicos
String analogL, analogT, analogU;                                                               // Cria strings que estão relacionados às medidas analógicas
float somaT, somaU, vet[101];                                                                   // Cria variáveis tipo float referentes a soma dos incrementos
float mediaT, mediaU;                                                                           // Cria variáveis tipo float referentes a média aritmética das medições

WiFiServer server(80);    // Caso ocorra problemas com a porta 80, utilize outra (ex.: 8082, 8089, etc) e a chamada da URL ficará ip: Porta (ex.:192.168.0.15:8082)

/* Pinout e variáveis servos */
Servo servo_motorJ;       // Declara um servo com o nome de servo_motorJ (J = janela)
Servo servo_motorP;       // Declara um servo com o nome de servo_motorP (P = portão)

/* Pinout sensor de umidade e temperatura */
DHT dht(D5, DHT11);       // Define pino D5 para sensor DHT11

/* Pinout irrigação */
#define sensor_solo D0    // Define pino D0 para sensor de umidade do solo
#define rele_bomba D1     // Define pino D1 para relé da bomba

/*Pinout iluminação interna*/
#define leds_interno1 D2  // Define pino D2 para relé iluminação interna (piso 1)
#define leds_interno2 D7  // Define pino D7 para relé iluminação interna (piso 2)

/* Pinout e variáveis iluminação externa */
int sensor_operation = 0; // Valor inicial para variável processada
int sensor_value = 0;     // Valor inicial da leitura do LDR
int pwm;                  // Declaração de variável para saída pwm

#define leds_externo D6   // Define pino D8 para iluminação externa

/* Fim da declaração de variáveis */

void setup() {
  /* Zera o vetor de temperatura */
  for ( i=0; i <= 100 ; i++ ){
     vet[i] = 0;
     }

  /* Zera o vetor de umidade */
  for ( j=0; j <= 100 ; j++ ){
     vet[j] = 0;
     }

  /* Setuup servos */
  servo_motorJ.attach(D3);          // Define o pino servo_motor como saída
  servo_motorP.attach(D4);                    
 
  /* Setup sensor de umidade e temperatura*/
  dht.begin();                      // Inicia o DHT
                   
  /* Setup irrigação */
  pinMode(sensor_solo,  INPUT);
  pinMode(rele_bomba,   OUTPUT);

  /* Setup iluminação interna */
  pinMode(leds_interno1, OUTPUT);
  pinMode(leds_interno2, OUTPUT);

  /* Setup iluminação externa */
  pinMode(leds_externo, OUTPUT);

  /* Setup do status inicial dos pinos */    
  servo_motorJ.write(170);                      
  servo_motorP.write(180);
  digitalWrite(rele_bomba,    LOW);   
  digitalWrite(leds_interno1, LOW);
  digitalWrite(leds_interno2, LOW);   
  digitalWrite(leds_externo,  LOW);
    
  /* Setup do node */
  Serial.begin(9600);                               // Inicializa o serial
  delay(10);                                        // Intervalo de 10 milissegundos

  Serial.println("");                               // Pula uma linha da janela serial
  Serial.println("");                        
  Serial.print("Conectando a ");                    // Escreve o texto na serial
  Serial.print(ssid);                               // Escreve o nome da rede na serial

  WiFi.begin(ssid,password);                        // Passa os parâmetros para a função que vai fazer a conexão com a rede sem fio
  WiFi.config(ip,Gateway,Subnet);
    while (WiFi.status() != WL_CONNECTED) {         // Enquanto o status for diferente de conectado
       delay(500);                                  // Intervalo de 500 milissegundos
       Serial.print(".");                           // Escreve o caracter na serial
       };
               
  Serial.println("");
  Serial.print("Conectado a rede sem fio ");        // Escreve o texto na serial
  Serial.println(ssid);                             // Escreve o nome da rede na serial
  server.begin();                                   // Inicia o servidor para receber dados na porta definida em: "WiFiServer server(porta);"
  Serial.println("Servidor iniciado");              // Escreve o texto no serial

  Serial.print("IP para se conectar ao NodeMCU: "); // Escreve o texto no serial: "IP para se conectar ao NodeMCU"
  Serial.print("http://");                          // Escreve em seguida: "http://"
  Serial.println(WiFi.localIP());                   // Escreve em seguida, o ip recebido dentro da rede sem fio (o ip é recebido de forma automática)
  
  delay(1000);
}

void loop() {
  /* Página HTML */
  WiFiClient client = server.available();           // Verifica se algum cliente está conectado no servidor
    if (!client) {                                  // Se não existir cliente conectado, reexecuta o processo até que algum se conecte ao servidor
       return;
       }
       
  Serial.println("Novo cliente se conectou!");      // Escreve o texto no serial: "Novo cliente se conectou!"
    while(!client.available()){                     // Enquanto o cliente estiver conectado, executar as operações dentro do loop
       char ch = static_cast<char>(client.read());
       Serial.print(ch);
       countClient = clientes + 1;
       delay(1);                                    // Intervalo de 1 milissegundo
       }

  String request = client.readStringUntil('\r');    // Faz a leitura da primeira linha de requisição
  Serial.println(request);                          // Escreve a requisição na serial
  client.flush();                                   // Aguarda até que todos os dados de saída sejam enviados ao cliente

  String html  = "<html><head><title>NODEMCU</title>";
  html += "<meta charset=\"utf-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html  +="<meta http-equiv=\"refresh\" content=\"5\">";

  html += "<style>";
  html += "*{ box-sizing: border-box;}";
  html += "body { font-family: Arial, Helvetica, sans-serif;}";
  html += "header {";
  html += "background-color: darkblue;";
  html += "padding: 0.30px;";
  html += "text-align: center;";
  html += "font-size: 15px;";
  html += "color: yellow;}";

  /* cria duas colunas na paginar */
  html += "nav {";
  html += "float: left;";
  html += "width: 10%;";
  html += "height: 620px;";
  html += "background: #ccc;";
  html += "padding: 10px;}";

  /* Style the list inside the menu */
  html += "nav ul {";
  html += "list-style-type: none;";
  html += "padding: 0;}";

  html += "article {";
  html += "float: left;";
  html += "padding: 20px;";
  html += "width: 90%;";
  html += "background-color: #f1f1f1;";
  html += "height: 620px;}";

  /* Clear floats after the columns */
  html += "section:after {";
  html += "content: "";";
  html += "display: table;";
  html += "clear: both;}";

  /* Style do rodapé */
  html += "footer {";
  html += "background-color: darkblue;";
  html += "padding: 1px;";
  html += "text-align: center;";
  html += "color: white;}";

  /* layout */
  html += "@media (max-width: 300px) {";
  html += "nav, article {";
  html += "width: 100%;";
  html += "height: auto;  }";
  html += "}";

  html += "</style>";
  html += "</head>";

  html += "<body>";
  html += "<p>";
  html += "<header id=\"principal\" >";
  html += "<h3>CENTRO FEDERAL DE EDUCAÇÃO TECNOLÓGICA DE MINAS GERAIS - CEFET MG <p><i>CURSO TÉCNICO DE ELETROTÉCNICA</i></p></h3>";
  html += "</header>";
  html += "<section>";
  html += "<nav>";
  html += "<ul>";
  html += "<li><a href=\"#principal\">Supervisório do Projeto</a></li>";
  html += "<p> </p>";
  html += "</ul>";
  html += "</nav>";
 
  html += "<article >";
  html += "<h3>Supervisório IoT ESP8266 -  Casa Inteligente </h3>";
 
  html += "<p><style>   table, th , td {border: 5px solid grey;}</style></p>";
  html += "<p><table>   <colgroup> <col span=\"4\" style=\"background-color:grey\"> </colgroup></p>";
  html += "<p><tr>      <th>I/O</th> <th>Endereço</th> <th>Descrição</th> <th>Valor</th> <th>Comando</th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio00</th> <th> Comando da Janela              </th> <th id = \"GPio00\"></th> <th><a href=\"?fuction = ligaJ\"><button onclick = ResultonJ()>Janela ON</button></a><a href=\"?fuction = desligaJ\"><button onclick = ResultoffJ()>Janela OFF</button></a> </th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio02</th> <th> Comando do Portão              </th> <th id = \"GPio02\"></th> <th><a href=\"?fuction = ligaP\"><button onclick = ResultonP()>Portão ON</button></a><a href=\"?fuction = desligaP\"><button onclick = ResultoffP()>Portão OFF</button></a> </th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio04</th> <th> Comando da Iluminação (Piso 1) </th> <th id = \"GPio04\"></th> <th><a href=\"?fuction = ligaI1\"><button onclick = ResultonI1()>Interna1 ON</button></a><a href=\"?fuction = desligaI1\"><button onclick = ResultoffI1()>Interna1 OFF</button></a> </th></p>";
  html += "<p><tr>      <th>Output</th> <th>GPio13</th> <th> Comando da Iluminação (Piso 2) </th> <th id = \"GPio13\"></th> <th><a href=\"?fuction = ligaI2\"><button onclick = ResultonI2()>Interna2 ON</button></a><a href=\"?fuction = desligaI2\"><button onclick = ResultoffI2()>Interna2 OFF</button></a> </th></p>";
  html += "<p><tr>     <th>Output</th> <th>GPio05</th> <th> Status da Irrigação             </th> <th id = \"GPio05\"></th> </p>";
  html += "<p><tr>      <th>Output</th> <th>GPio12</th> <th> Status Iluminação Externa      </th> <th id = \"GPio12\"></th> </p>";
  html += "<p><tr>      <th>Input</th> <th>GPio14</th> <th> Temperatura (°C)                </th> <th><a href=\"?fuction=analogT\"></a> </p>";
  html += mediaT;
  html += "</th></p>";
  html += "<p><tr>      <th>Input</th> <th>GPio14</th> <th> Umidade do Ar (%)              </th> <th><a href=\"?fuction=analogU\"></a> </p>";
  html += mediaU;
  html += "</th></p>";
  html += "<p></table></p>";
  
  html += "</th></p>";
  html += "<p></table></p>";
 
  html += "</article>";
  html += "</section>";

  html += "<footer>";
  html += "<h8><i>Coordenação de Eletrotécnica - CEFET MG - Todos os direitos reservados &#9879</i></h8>";
  html += "</footer>";

  html += "<br class=\"quebrapagina\">";
  html += "</body>";

  html += "</html>";

  client.flush();
  client.print(html);
  
  delay(1);                                      // Intervalo de 1 milissegundo
  Serial.println("Cliente desconectado:");       // Escreve o texto no serial: "Cliente desconectado"
  Serial.println("");
  
  /* Programa iluminação externa */
  sensor_value = analogRead(A0);               // Lê o valor de 10 bits na entrada analógica A0
  sensor_operation = (sensor_value - 100)*3;   // Processa o valor de modo que a saída pwm funcione como desejado
     
  if(sensor_value < 150) {                     // Para níveis de luminosidade inferiores ao valor, desligar a saída
     pwm = 0;
     }
     else {
        (pwm = sensor_operation);                 // Atualiza o valor da variável pwm
        }
     
  analogWrite(leds_externo, pwm);              // Joga para a saída o valor atualizado de pwm

  if (sensor_value > 150) {
     client.println("<script> document.getElementById(\"GPio12\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio12\").style.backgroundColor=\"green\"</script>");
     }
  if (sensor_value <= 150) {
     client.println("<script> document.getElementById(\"GPio12\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio12\").style.backgroundColor=\"red\"</script>");
     }
     
  /* Programa sensor de umidade e temperatura */ 
  if (request.indexOf("analogT") != 1){
     float temperatura = dht.readTemperature();
     somaT = 0;
     mediaT = 0;
     
     for ( i = 0; i <= 99; i++) {                 // Executa um pequeno loop em que se coleta 100 valores percentuais da entrada e forma-se um vetor
        temperatura = vet[i];     
        somaT = somaT + vet[i];
        delay(1);
        }
        mediaT = somaT/100;
        Serial.println(mediaT);        
     }
     
  if (request.indexOf("analogU") != 1){
     float umidade = dht.readHumidity();
     mediaU = 0;
     somaU = 0;
     
     for ( j = 0; j <= 99; j++) {                 // Executa um pequeno loop em que se coleta 100 valores percentuais da entrada e forma-se um vetor
        umidade = vet[j];     
        somaU = somaU + vet[j];
        delay(1);
        }
        mediaU = somaU/100;
        Serial.println(mediaU);
     }

  /* Programa iluminação interna */
  /* Piso 1 */
  if (request.indexOf("ligaI1") != -1) {
     digitalWrite(leds_interno1, HIGH);
     toggleI1 = 1;
     }

  if (request.indexOf("desligaI1") != -1) {
     digitalWrite(leds_interno1, LOW);
     toggleI1 = 2;
     }

  if((toggleI1 == 1)&&(request.indexOf("ResultonI1") != 1)){
     client.println("<script> document.getElementById(\"GPio04\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio04\").style.backgroundColor=\"green\"</script>");
     }

  if((toggleI1 == 2)&&(request.indexOf("ResultoffI1") != 1)){
     client.println("<script> document.getElementById(\"GPio04\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio04\").style.backgroundColor=\"red\"</script>");
     }
     
  /* Piso 2 */
  if (request.indexOf("ligaI2") != -1) {
     digitalWrite(leds_interno2, HIGH);
     toggleI2 = 1;
     }

  if (request.indexOf("desligaI2") != -1) {
     digitalWrite(leds_interno2, LOW);
     toggleI2 = 2;
     }

  if((toggleI2 == 1)&&(request.indexOf("ResultonI2") != 1)){
     client.println("<script> document.getElementById(\"GPio13\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio13\").style.backgroundColor=\"green\"</script>");
     }

  if((toggleI2 == 2)&&(request.indexOf("ResultoffI2") != 1)){
     client.println("<script> document.getElementById(\"GPio13\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio13\").style.backgroundColor=\"red\"</script>");
  } 
     
  /* Programa irrigação */
  Serial.print("Condição:");
  
  if (digitalRead(sensor_solo) == LOW) {  //
     Serial.print(" SEM UMIDADE ");
     digitalWrite(rele_bomba, HIGH);
     } 
  else {
     Serial.print(" COM UMIDADE ");
     digitalWrite(rele_bomba, LOW);
     }
 
  if (digitalRead(rele_bomba) == HIGH) {  // 
     Serial.println(" - Bomba LIGADA");
     } 
  else {
     Serial.println(" - Bomba DESLIGADA"); //
     }
     
  if (digitalRead(rele_bomba == HIGH)){
     client.println("<script> document.getElementById(\"GPio05\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio05\").style.backgroundColor=\"green\"</script>");
     }

  if (digitalRead(rele_bomba == LOW)){
     client.println("<script> document.getElementById(\"GPio05\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio05\").style.backgroundColor=\"red\"</script>");
     }
  
  /* Programa controle dos servos */
  /* Janela */
  if (request.indexOf("ligaJ") != -1) {
     servo_motorJ.write(80);
     toggleJ = 1;
     }

  if (request.indexOf("desligaJ") != -1) {
     servo_motorJ.write(170);
     toggleJ = 2;
     }

  if ((toggleJ == 1)&&(request.indexOf("ResultonJ") != 1)){
     client.println("<script> document.getElementById(\"GPio00\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio00\").style.backgroundColor=\"green\"</script>");
     }

  if ((toggleJ == 2)&&(request.indexOf("ResultoffJ") != 1)){
     client.println("<script> document.getElementById(\"GPio00\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio00\").style.backgroundColor=\"red\"</script>");
     }
  
  /* Portão */
  if (request.indexOf("ligaP") != -1) {
     servo_motorP.write(90);
     toggleP = 1;
     }

  if (request.indexOf("desligaP") != -1) {
     servo_motorJ.write(180);
     toggleP = 2;
     }

  if ((toggleP == 1)&&(request.indexOf("ResultonP") != 1)){
     client.println("<script> document.getElementById(\"GPio02\").innerHTML = 'ON'</script>");
     client.println("<script> document.getElementById(\"GPio02\").style.backgroundColor=\"green\"</script>");
     }

  if ((toggleP == 2)&&(request.indexOf("ResultoffP") != 1)){
     client.println("<script> document.getElementById(\"GPio02\").innerHTML = 'OFF'</script>");
     client.println("<script> document.getElementById(\"GPio02\").style.backgroundColor=\"red\"</script>");
     }

  delay(100);
  
  client.flush();

  delay(1);
}
