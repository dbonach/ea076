
/* Macro de clock do microcontrolador */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#include <Wire.h>

/* Definicao do endereco do PCF a ser utilizado */
#define ADDRESS 32 // 00100000b == 32d
// declaracao variavel
float temperatura;

/* Contabiliza interrupcoes
   associadas ao TIMER0_COMPA. */
volatile unsigned int timerZeroInterruptCount = 0;

// Definir o pino de entrada analógica onde o sensor está conectado
const int pinoSensor = A0;
int temp;
void setup() {
  // Inicializar a comunicação serial
  Serial.begin(9600);
  Wire.begin(); // Join I2C bus

  /* Configuracao de TIMER0 para gerar base de tempo */
  configuracao_Timer0();
}

// Codigo disponibilizado pelo professor
// Nao é necessario modificar essa função
void configuracao_Timer0(){
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuracao Temporizador 0 (8 bits) para gerar interrupcoes periodicas a cada 1.6ms no modo Clear Timer on Compare Match (CTC)
  // Relogio = 16e6 Hz
  // Prescaler = 1024
  // Faixa = 25 (contagem de 0 a OCR0A = 24)
  // Intervalo entre interrupcoes: (Prescaler/Relogio)*Faixa = (1024/16e6)*(24+1) = 1.6ms

  // TCCR0A – Timer/Counter Control Register A
  // COM0A1 COM0A0 COM0B1 COM0B0 – – WGM01 WGM00
  // 0      0      0      0          1     0
  TCCR0A = 0x02;

 // OCR0A – Output Compare Register A
  OCR0A = 24;

  // TIMSK0 – Timer/Counter Interrupt Mask Register
  // – – – – – OCIE0B OCIE0A TOIE0
  // – – – – – 0      1      0
  TIMSK0 = 0x02;

  // TCCR0B – Timer/Counter Control Register B
  // FOC0A FOC0B – – WGM02 CS02 CS01 CS0
  // 0     0         0     1    0    1
  TCCR0B = 0x05;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

ISR(TIMER0_COMPA_vect){

    /* Incremento de variavel utilizado como referencia da base de tempo */
    timerZeroInterruptCount++;

    /* Como interrupcao do Timer0 corresponde a 1.6ms, para termos 2s:
        2000/1.6 = 1250 interrupcoes. */
    if (timerZeroInterruptCount == 1250) {
        timerZeroInterruptCount = 0;
        //converte float para int
        temp = int (temperatura*100);
    }
}



void loop() {
  // Ler o valor analógico do sensor
  int valorSensor = analogRead(pinoSensor);

  // Converter o valor analógico para temperatura em graus Celsius
  float tensao = valorSensor * (5.0 / 1023.0);  // Converter o valor para tensão (0-5V)
  temperatura = tensao * 100;  // Cada 10mV de variação corresponde a 1°C
 
 //exibe temperatura no display 7 segmentos
  sendRpmTo4DigitDisplay(temp);
}

/* Send temperature to 4-digit display */
void sendRpmTo4DigitDisplay(int number) {
    encodeDigit3(number / 1000);
    encodeDigit2(number % 1000 / 100);
    encodeDigit1(number % 1000 % 100 / 10);
    encodeDigit0(number % 1000 % 100 % 10);
}

/* Encode units (less valuable) */
void encodeDigit0(int number) {   
    sendDigit(112 | number); // 1110000 OR number
}

/* Encode tens */
void encodeDigit1(int number) {
    sendDigit(176 | number); // 11010000 OR number
}

/* Encode hundreds */
void encodeDigit2(int number) {
    sendDigit(208 | number); // 10110000 OR number
}

/* Encode thousands (more valuable) */
void encodeDigit3(int number) {
    sendDigit(224 | number); // 01110000 OR number
}

/* Send encoded values through I2C bus */
void sendDigit(int value) {
    Wire.beginTransmission(ADDRESS); // Inicio de transmissao entre o arduino e o PCF de endereco 0100000b
    Wire.write(value); // Prepara os bits (value) a serem transmitidos 
    Wire.endTransmission(); // Realiza a transmissao do dado
}