/*  EA076C
    Módulo 1: Leitura e escrita na memória EEPROM 24C16

    Alunos:
        Deivit Lopes Bonach     RA:166508
        Vinicius Henrique Luiz  RA:245342
*/

#include <Wire.h>
#define EEPROM_ADDR 0x50 // Endereço da memória EEPROM 24C16
/* Configuracao da comunicacao serial. */
void setup(void)  {
  Serial.begin(9600); // Set baud rate
  Wire.begin(); // Join I2C bus
}


// Escrita de um byte na memoria 
void escrever(unsigned int add, unsigned char dado){
  int enderecoEEPROM = 0b1010000;

  //byte menos significativo do endereço da memoria
  unsigned char byteLstAdd = (unsigned char)add; 
  
  //byte mais significativo do endereço da memória
  unsigned int byteMstAdd = add>>8;
  
  
  int primeiroByteTransmitido = (enderecoEEPROM |byteMstAdd);
  
  Serial.println(primeiroByteTransmitido, BIN);
  Wire.beginTransmission(primeiroByteTransmitido); // Inicio de transmissao entre o arduino e a memória, add de address, endereço da memoria
  //Wire.write(byteMstAdd); //byte mais significativo do endereço
  //Serial.println(byteMstAdd, BIN);  
  Wire.write(byteLstAdd);
  //Serial.println(byteLstAdd, BIN);  
  Wire.write(dado); // dados que vao ser salvos na memória, no endereço acima 
  //Serial.println(dado, BIN);
  Wire.endTransmission(); // Realiza a transmissao do dado
}

unsigned char ler(unsigned int add){
  byte data = 0;
  unsigned int byteMstAdd = add>>8;  
  int primeiroByteTransmitido = (EEPROM_ADDR |byteMstAdd);
  Wire.beginTransmission(primeiroByteTransmitido); // Inicia a transmissão para o endereço da memória
  byteLstAdd = (unsigned char)add
  Wire.write(byteLstAdd); // Escreve o byte de endereço de alta
  Wire.endTransmission(); // Finaliza a transmissão

  Wire.requestFrom(EEPROM_ADDR, 1); // Solicita 1 byte de dados da memória

  if (Wire.available()) {
    data = Wire.read(); // Lê o byte de dados recebido da memória
    //Serial.println(data, BIN);
  }

  return data;
}

void loop(){
  escrever(0x2EF,0xFF);
  ler(0x2EF);
}
