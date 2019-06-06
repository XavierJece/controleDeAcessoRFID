/*
******************************************************************

*************************PINAGEM**********************************

  RFID-RC522   Wemos          Nodemcu
  RST/Reset RST  D3  [1*]        D3 [1*]      GPIO-0  [1*]
  SPI SS  SDA[3] D8  [2*]        D8 [2*]      GPIO-15 [2*]
  SPI MOSI MOSI  D7              D7           GPIO-13
  SPI MISO MISO  D6              D6           GPIO-12
  SPI SCK SCK    D5              D5           GPIO-14

  [1*] (1, 2) Configuracao tipica definindo como RST_PIN no sketch / programa.
  [2*] (1, 2) Configuracao, tipica definindo como SS_PIN no sketch / programa.
  [3*] O pino SDA pode ser rotulado como SS em algumas placas de MFRC522 / mais antigas, significam a mesma coisa.
******************************************************************************************************************
*/

//Incluindo Bibliotecas
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//Definino Pinos RFID - LED
#define RST_PIN    D3
#define SS_PIN     D8

#define LED        D2 // Vermelho
#define LED        D4 // Verde
#define LED        D1 // Amarelo


//Variveis para conexção Wifi
const char* ssid = "JX";
const char* password = "qwertyuiop";
const char* host = "192.168.43.42";
const int httpsPort = 80;
WiFiClient client;

//Variaveis para controle
bool flag = true;
bool primeiraVez = true;
  /*
  * A variavel codigoDisciplina serve para fazer em qual materia é para ter presença
  *  CODIGO => DISCIPLINA
  *     1   => Circuitos Digitais
  *     2   => WEB I
  *     3   => Arquiterura e Organização de Computadores
  */
int  codigoDisciplina = 2;

// Instanciando MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);

//Declarando Funçöes
void apresentacaoInicio();
void iniciarWifi();
String enviarDados(String);
bool verificarPresenca (String);
String criarUID();
String urlencode(String);
String urldecode(String);
unsigned char h2int(char);

//************** SETUUUUP **********************
void setup()
{
  Serial.begin(115200); // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  //Declarando os dois Leds delidados
  pinMode(D2, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D1, OUTPUT);
}

//************** LOOOOOP **********************
void loop() {
  apresentacaoInicio();

  digitalWrite(D1, HIGH);     // Liga LED Amarelo
  delay(100);
  digitalWrite(D1, LOW);     // DESlIGA LED Amarelo
  
  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  digitalWrite(D1, HIGH);     // Liga LED Amarelo

  
  //flag setada como true para aprensetar mensagem novamente
  flag = true;

  //Crianco a Variavel que contem a UID do Cartão
  String conteudo = criarUID();

  Serial.println("UID: " + conteudo); //Apresentando a UID do cartão

  //UID do cartao do Boniolo
  if (verificarPresenca(String(conteudo.substring(1)))) {
    
    digitalWrite(D1, LOW);     // DESlIGA LED Amarelo
    Serial.println("Presenta dada !");
    Serial.println();
    digitalWrite(D4, HIGH);  // LIGA  LED Verde
    delay(2000);              // DELAY /espera 3 segundos

  } else {
    digitalWrite(D1, LOW);     // DESlIGA LED Amarelo
    Serial.println("Erro ao dar presenta ! Certeza que faz está matéria?");
    Serial.println();
    digitalWrite(D2, HIGH);  // LIGA LED Vermelho
    delay(2000);              // DELAY /espera 3 segundos
  }
}

//************** FUNÇÖES **********************

//Função para apresentar msg no inicio
void apresentacaoInicio(){
  if (flag) {
    iniciarWifi(); // Chama a funcao que inica a conexäo wifi

    Serial.println("Aproxime o seu cartao do leitor...");
    digitalWrite(D4, LOW);     // DESlIGA LED Verde
    digitalWrite(D2, LOW);     // DESlIGA LED Vermelho
    flag = false;
    primeiraVez = false;
  }
}


//Funçäo para conctar ao wifi
void iniciarWifi() {
  if (primeiraVez){
    //Debug
    //Serial.print("Conectando em ");
    //Serial.println(ssid);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (primeiraVez){
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println(" ");
    Serial.print("WiFi Conectado IP: ");
    Serial.println(WiFi.localIP());

    //Debug
    //Serial.print("Conectando em ");
    //Serial.println(host);
  }

  if (!client.connect(host, httpsPort)) {
    Serial.println("falha na conecção");
    return;
  }
}

//Funçöes que envia a UID
String enviarDados(String uid) {
  String url = "/MY-Projects/Comunicacao_de_Dados/index.php";
  String parametros = "?uid=" + urlencode(uid) + "&codDisc=" + codigoDisciplina;

  //Debug
  //Serial.print("requesting URL: ");
  //Serial.print(url);
  //Serial.println(parametros);

  client.print(String("GET ") + url + parametros + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  //Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //Serial.println("headers received");
      break;
    }
  }

  //O que foi imprimido na tela
  String line = client.readStringUntil('\n');

  //Debug
  //Serial.println("reply was:");
  //Serial.println("==========");
  //Serial.println(line);
  //Serial.println("==========");
  //Serial.println("closing connection");

  return line;
}


//Funçäo para Se a presença foi dada
bool verificarPresenca (String uid) {
  String line = enviarDados(uid);

  if (line.startsWith("Sucesso")) {
    //Serial.println("esp8266/Arduino CI successfull!");
    return true;
  } else {
    //Serial.println("esp8266/Arduino CI has failed");
    return false;
  }
}

//Função que monta a UID do Cartão
String criarUID() {

  //Mostra UID na serial (Debug)
  //Serial.print("UID da tag :");
  String conteudo = "";
  byte letra;

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  //Debug
  //Serial.println();
  //Serial.print("Mensagem : ");
  conteudo.toUpperCase();

  return conteudo;
}

//Função que decodifica (tira ela da url) a String
String urldecode(String str)
{

  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {

      encodedString += c;
    }

    yield();
  }

  return encodedString;
}


//Função que codifica (faz ela uma url) a String
String urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;

}

//Função para completandar a urlencode e deconde
unsigned char h2int(char c)
{
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
    
