/*  EA076C
  Módulo 2: Teclado matricial

  Alunos:
    Deivit Lopes Bonach     RA:166508
    Vinicius Henrique Luiz  RA:245342
*/

/*
Pin relation:
  pino 11 = coluna 1
  pino 12 = coluna 2
  pino 13 = coluna 3
  pino 10 = linha 1
  pino 9 = linha 2
  pino 8 = linha 3
  pino 7 = linha 4

  pin 7 to 10 input
  pint 11 to 13 output

  output:
    columns[3]
    activeLine
    pressedKey
    flagKeyWasPressed

  input:
    lines[4]

*/
#include <Arduino.h>

/* Create variables */
int columns[3] = {1, 1, 1};
int activeLine;
int pressedKey;
int flagKeyWasPressed = 0;
int lines[4] = {1, 1, 1, 1};
int state = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);

    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
}

void loop()
{
    switch (state)
    { // state machine
    case 0:
        digitalWrite(11, LOW);

        if (digitalRead(11) == 0)
        {
            Serial.println("apertou a tecla 1");
        }
        break;
    case 1:
        // do something
        break;
    case 2:
        // do something
        break;
    case 3:
        // do something
        break;
    case 4:
        // do something
        break;
    case 5:
        // do something
        break;
    case 6:
        // do something
        break;
    case 7:
        // do something
        break;
    }
}