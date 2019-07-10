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

/*----------------------------------*/
/*INCLUSÃO DE BIBLIOTECAS*/
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
/*----------------------------------*/

/*----------------------------------*/
/*DECLARAÇÃO DE PINAGEM*/
const int RST_PIN = D3;
const int SS_PIN = D8;

const int LedVermelho = D0;
const int LedVerde = D1;
const int LedAmarelo = D2;
/*----------------------------------*/

/*----------------------------------*/
/*DECLARAÇÃO DE VARIAVEIS PARA CONEXÃO WIFI*/
const int httpsPort = 80;
WiFiClient client;

const char* nomeRede = "BRAINIAC";
const char* password = "qwertyuiop";
const char* host = "192.168.137.1";
/*----------------------------------*/

/*----------------------------------*/
/*DECLARAÇÃO DE VARIAVEIS PARA CONTROLE*/
bool tagEncontrada = true;
/*----------------------------------*/

/*----------------------------------*/
/*INSTACIANDO DE VARIAVEIS PARA MFRC522*/
MFRC522 mfrc522(SS_PIN, RST_PIN);
/*----------------------------------*/

/*---------------------------------------------------------*/
/*DECLARAÇÃO DE FUNÇÕES*/
void desligaLeds();
void ligaLeds();
void iniciarWifi();
bool verificaConexaoWifi();
void procurandoWifi();
void procuraTag();
String enviarDados(String);
bool verificarAcesso(String); 
String criarUID();
String urlencode(String);
String urldecode(String);
unsigned char h2int(char);

/*---------------------------------------------------------*/
void setup() {
  pinMode(LedVermelho, OUTPUT); // Inicializando LED Vermelho para SAIDA
  pinMode(LedVerde, OUTPUT);    // Inicializando LED Verde para SAIDA
  pinMode(LedAmarelo, OUTPUT);  // Inicializando LED Amarelo para SAIDA

  desligaLeds();

  
  Serial.begin(115200); // Inicia a serial

  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
}

/*---------------------------------------------------------*/
void loop() {
  if(!verificaConexaoWifi()){
    iniciarWifi();
  }else{
    
    if(tagEncontrada){
      Serial.println("PROCURANO cartao");
      tagEncontrada = false;
    }
    procuraTag();
    
    // Procura por cartao RFID
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // Seleciona o cartao RFID
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
  
    /*SE CHEGAR AQUI É PQ UMA TAG FOI SELECIONADA*/
    digitalWrite(LedAmarelo, HIGH);
    tagEncontrada = true;
    
    //Crianco a Variavel que contem a UID do Cartão
    String conteudo = criarUID();

    //Apresentando a UID do cartão
    Serial.println("\n");
    Serial.println("UID: " + conteudo);
    Serial.println("\n");

    if(verificarAcesso(String(conteudo.substring(1)))){
      Serial.println("Acesso Liberado");
      desligaLeds();
      digitalWrite(LedVerde, HIGH);
    }else{
      Serial.println("Acesso Negado");
      desligaLeds();
      digitalWrite(LedVermelho, HIGH);
    }
    
  }
  Serial.println("\n");
  Serial.println("\n");
  Serial.println("\n");

  delay(2000);
  
}

/****************IMPLIMENTAÇÃO DAS FUNÇÕES*******************/

/*---------------------------------------------------------*/
void desligaLeds(){
  digitalWrite(LedVermelho, LOW);
  digitalWrite(LedVerde, LOW);
  digitalWrite(LedAmarelo, LOW);
}

/*---------------------------------------------------------*/
void ligaLeds(){
  digitalWrite(LedVermelho, HIGH);
  digitalWrite(LedVerde, HIGH);
  digitalWrite(LedAmarelo, HIGH);
}

/*---------------------------------------------------------*/
void procurandoWifi(){
  ligaLeds();
  delay(125);
  desligaLeds();
  delay(125);
  Serial.print(".");
}

/*---------------------------------------------------------*/
bool verificaConexaoWifi(){
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }else{
    return true;
  }
}

/*---------------------------------------------------------*/
void iniciarWifi() {
  
  Serial.flush();
  Serial.print("Conectando em ");
  Serial.println(nomeRede);
  Serial.println(" ");
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(nomeRede, password);

  while (WiFi.status() != WL_CONNECTED) {
    procurandoWifi();
  }

  Serial.println(" ");
  Serial.print("WiFi Conectado IP: ");
  Serial.println(WiFi.localIP());

  desligaLeds();
  digitalWrite(LedVerde, HIGH);
  
  //Debug
  //Serial.print("Conectando em ");
  //Serial.println(host);

  if(!verificaConexaoWifi()){
    desligaLeds();
    digitalWrite(LedVermelho, HIGH);
    Serial.println("Erro na conexão com o Wifi");
  }

}
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
void procuraTag(){
  desligaLeds();
  digitalWrite(LedAmarelo, HIGH);
  delay(100);
  digitalWrite(LedAmarelo, LOW);
  delay(100);
  Serial.flush();
  Serial.print(".");
}

/*---------------------------------------------------------*/
//Funçöes que envia a UID
String enviarDados(String uid){
  
  if ( !client.connect(host, httpsPort) ) {
    Serial.println("Falha na conexao com o site ");
  }

  
  String url = "/MY-Projects/Comunicacao_de_Dados/index.php";
  String parametros = "/?uid=" + urlencode(uid);
  url = url + parametros;
  
  //Debug
  Serial.println("requesting URL: "+ url);
  //Serial.print(url);
  //Serial.println(parametros);

  client.print(String("GET /") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );


    String line;
    //Serial.println("[Response: ");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        line = client.readStringUntil('\n');
        
      }
    }
  Serial.println("line: " + line);
  return line;
}

/*---------------------------------------------------------*/
bool verificarAcesso(String uid) {
  String line = enviarDados(uid);
  
  if (line.startsWith("Liberado")) {
    //Serial.println("esp8266/Arduino CI successfull!");
    return true;
  } else {
    //Serial.println("esp8266/Arduino CI has failed");
    return false;
  }
}

/*---------------------------------------------------------*/
//Função que monta a UID do Cartão
String criarUID(){

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

/*---------------------------------------------------------*/
//Função que codifica (faz ela uma url) a String
String urlencode(String str){
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

/*---------------------------------------------------------*/
//Função que decodifica (tira ela da url) a String
String urldecode(String str){
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

/*---------------------------------------------------------*/
//Função para completandar a urlencode e deconde
unsigned char h2int(char c){
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
