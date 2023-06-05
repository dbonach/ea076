/*  EA076C
  Módulo 2: Teclado matricial

  Alunos:
    Deivit Lopes Bonach     RA:166508
    Vinicius Henrique Luiz  RA:245342
*/

void setup(void)  {
}

void loop(void) {
  switch (variável)
{
    /*pino 10 = linha 1
      pino 9 = linha 2
      pino 8 = linha 3
      pino 7 = linha 4
      */
   case constante1:
     //coloca coluna 1 em 0V e mantem as demais em 5V
     // pino 11 = coluna 1
     // pino 12 = coluna 2
     // pino 13 = coluna 3
     digitalWrite(11,LOW);
     digitalWrite(12,HIGH);
     digitalWrite(13,HIGH);
     
     if (digitalRead(10)){
      
     }

   break;

   case constante2:
     Instruções;
   break;

   default
     Instruções;
}

}