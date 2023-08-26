#include <SPI.h>      //biblioteca de comunicacao
#include <MFRC522.h>  //biblioteca propria do sensor

#define RST_PIN 9
#define SS_PIN  7

bool exibMsg = true;


MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instancia pra se referir ao sensor, vai usar mfrc522

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //inicia a serial em 9600, pode mudar dependendo
  SPI.begin();        //inicia a comunicacao SPI
  mfrc522.PCD_Init(); //inicia o sensor do rfid (usando a instancia)
  Serial.println(F("\nCodigo de escrita e leitura V02\n"));

}

void gravacao(){
  //vamos comecar pelo comeco:
   byte buffer0[18];

  Serial.println(F("\ndigite o nome do produto: "));
  grav_aux(4, buffer0);
  
  Serial.println(F("\ndigite a data de validade: "));
  grav_aux(5, buffer0);

  Serial.println(F("\ndigite a data de fabricação: "));
  grav_aux(6, buffer0);


  mfrc522.PICC_HaltA(); //encerra a comunicação com a tag
  mfrc522.PCD_StopCrypto1();
  Serial.println("\n*Comunicacao com a tag encerrada*.");
  //Serial.flush()
  exibMsg = true;

}


void grav_aux(int blockgravar, byte bufferxd[18]){

  MFRC522::MIFARE_Key key;
  for(byte i = 0; i<6; i++) key.keyByte[i] = 0xFF; //isso vai zerar todas as chaves pra FF


  MFRC522::StatusCode status; // isso daqui e pra falar o estado, nao sei
  byte inp; 
  
  while (Serial.available() > 0) Serial.read(); //vai limpar o imput da serial pra dps ler os dados gravados

  Serial.setTimeout(60000L);
  inp = Serial.readBytesUntil('\n',bufferxd,16);   //leitura dos dados gravados
  for (byte i = inp; i < 16; i++) bufferxd[i] = ' ';  //esta zerando o buffer   
  
  // aqui ta autenticando e jogando no status
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockgravar, &key, &(mfrc522.uid));
  
  if(status != MFRC522::STATUS_OK){ //se falhar
    Serial.println(F("Autenticacao falhou: "));
    Serial.println(mfrc522.GetStatusCodeName(status));//fala oq faio
    return; // e retorna
  }
  
  //Agora vai escrever no bloco
  status = mfrc522.MIFARE_Write(blockgravar, bufferxd, 16); //aqui no baguio vai escrever memo
  if(status != MFRC522::STATUS_OK)//se nao tiver ok, falhar ai
  {
    Serial.println(F("Falha na gravacao da tag"));
    Serial.println(mfrc522.GetStatusCodeName(status));//mostra qual erro deu
  }
  else{
    Serial.println(F("\n!DADO GRAVADO COM SUCESSO!"));
  }

}


void leit_aux(int blockgravar, byte bufferxd[18], byte lengthxd){

  
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  MFRC522::StatusCode status; 

  
  //Aqui primeiro vai autenticar para poder receber os dados
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockgravar, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)//se nao tiver tudo certo
  {
    Serial.println(F("A autentificacao falhou!"));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  //aqui vai de fato fazer a leitura
  status = mfrc522.MIFARE_Read(blockgravar, bufferxd, &lengthxd);//pega pra fz a leitura
  if(status != MFRC522::STATUS_OK)// se der algo errado na leitura
  {
    Serial.println(F("A leitura falhou"));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //agora vai exibir a leitura
  for(uint8_t i = 0; i < 16; i++)
  {
    if(bufferxd[i] != 32)
    {
      Serial.write(bufferxd[i]);
    }
  }
}

void leitura(){
  
  
  //essa parte vai ficar em loop
  byte buffer0[18];
  byte block;
  byte len;
  


  Serial.println(F("\nCARTAO IDENTIFICADO"));
  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));


  //block = 0;
  //len = 18;
  //Serial.print(F("UID: "));
  //registro(block, buffer0, len);
  
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  
  block = 4;
  len = 18;
  Serial.print(F("\nProduto: "));
  leit_aux(block, buffer0, len);

  block = 5;
  Serial.print(F("\nValidade: "));
  leit_aux(block, buffer0, len);

  block = 6;
  Serial.print(F("\nFabricação: "));
  leit_aux(block, buffer0, len);
  
  /*
  //aqui vai de fato fazer a leitura
  status = mfrc522.MIFARE_Read(block, buffer0, &len);//pega pra fz a leitura
  if(status != MFRC522::STATUS_OK)// se der algo errado na leitura
  {
    Serial.println(F("A leitura falhou"));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //agora vai exibir a leitura
  for(uint8_t i = 0; i < 16; i++)
  {
    if(buffer0[i] != 32)
    {
      Serial.write(buffer0[i]);
    }
  }*/

  Serial.print(" ");
  Serial.println(F("\nLEITURA ENCERRADA"));
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  exibMsg = true;
}
void mensagem(){
  Serial.println("\nSelecione o modo do operação leitura(l) ou gravacao(g)\n");
}

//String serialReceived;
//char commandChar;

void loop() {
  
  MFRC522::MIFARE_Key key;
  for(byte i = 0; i<6; i++) key.keyByte[i] = 0xFF;

  if(!mfrc522.PICC_IsNewCardPresent()) //se NAO tiver tag, retorna
  {
    return; 
  }

  if(!mfrc522.PICC_ReadCardSerial()) //caso nao tambem, retorna
  {
    return;
  }



  if(exibMsg){
    mensagem();
    exibMsg = false;
  }

  int op = 0;
  if(Serial.available() > 0) {
    op = Serial.read();
    if(op == 'g'){
      gravacao();

    }
    else if(op == 'l'){
      leitura();
    }
    /*else{
      Serial.print("escolhe outro ae taokey");
    }*/
  }
}
