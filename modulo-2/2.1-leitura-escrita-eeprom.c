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

/* Escrita de um byte na memoria */
void escrever(unsigned int add, unsigned char dado){

  /* Desloca 8 bits a direita para separar os 3 bits mais significativos do endereço */
  unsigned int byteMstAdd = add>>8;

  /* byte menos significativo do endereço da memoria */
  unsigned char byteLstAdd = (unsigned char)add;

  /* Atribuição de uma mascara para juntar os bits do endereço da 
  EEPROM com os 3 primeiros bits do endereço do dado */  
  int primeiroByteTransmitido = (EEPROM_ADDR |byteMstAdd);
  
  /* Processo de escrita do byte dado na EEPROM no endereço add */
  Wire.beginTransmission(primeiroByteTransmitido);
  Wire.write(byteLstAdd); 
  Wire.write(dado); 
  Wire.endTransmission();
}

unsigned char ler(unsigned int add){
  
  /* Desloca 8 bits a direita para separar os 3 bits mais significativos do endereço */  
  unsigned int byteMstAdd = add >> 8;

  /* byte menos significativo do endereço da memoria */
  unsigned int byteLstAdd = (unsigned char) add;

  /* Atribuição de uma mascara para juntar os bits do endereço da 
  EEPROM com os 3 primeiros bits do endereço do dado */
  int primeiroByteTransmitido = (EEPROM_ADDR | byteMstAdd); 

  /* Escrita com o objetivo de escolher o endereço a ser lido */
  Wire.beginTransmission(primeiroByteTransmitido); // Inicia a transmissão para o endereço da memória
  Wire.write(byteLstAdd); // Escreve o byte restante de endereço de memoria
  Wire.endTransmission(); // Executa a transmissão
  Wire.requestFrom(primeiroByteTransmitido, 1); // Solicita 1 byte de dados da memória

  byte data = Wire.read();
  
  return data;
}

void loop(){
  escrever(0x20F, 0xF3);
  delay(5);
  ler(0x20F);
  
  /* Delay com objetivo de segurar o loop e analisar apenas a primeira iteracao */
  delay(10000000);
}