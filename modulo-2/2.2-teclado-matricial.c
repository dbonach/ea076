/*  EA076C
    Modulo 2: Teclado matricial

    Alunos:
        Deivit Lopes Bonach     RA:166508
        Vinicius Henrique Luiz  RA:245342
*/

#include <Arduino.h>

// definicao de constantes
#define ROWS 4
#define COLS 3
#define NO_ACTIVE_LINE 100
#define TRUE 1
#define FALSE 0

// arrays que mapeiam os pinos do arduino para as linhas e colunas do teclado
const char rowPins[ROWS] = {2, 3, 4, 5};
const char colPins[COLS] = {6, 7, 8};

// mapeamento do teclado para obtencao das teclas
char keypad[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// declaracao de variaveis globais
char debouncePass = 0;
char activeColumn = 0;
char activeLine = NO_ACTIVE_LINE;
char lastActiveLine = NO_ACTIVE_LINE;
char debouncedLine = NO_ACTIVE_LINE;
char flagDebounce = FALSE;

// configuracao do timer1 para gerar uma interrupcao a cada 50ms
void configuracao_Timer1(){
    
    // configura timer1 para operar no modo CTC (Clear Timer on Compare Match)
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);
    
    // valor de comparacao que corresponde a 50ms considerando o prescaler de 1024 e clock de 16MHz
    OCR1A = 781;
    
    // ativa interrupcao por comparacao com o registrador OCR1A
    TIMSK1 |= (1 << OCIE1A);
    
    // configura o prescaler para 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

void setup()
{
    // desabilita a flag global de interrupcao por precaucao
    cli();

    Serial.begin(9600); // set baud rate to 9600 bps

    // mapeamento dos pinos correspondentes a linha e colunas do teclado
    // colunas sao configuradas como saida e linhas como entrada com pull-up
    // o acionamento de uma tecla faz com que a linha correspondente seja colocada em nivel baixo
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);

    // coloca as colunas em nivel alto
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);

    /* configura o timer1 para gerar interrupcoes a cada 50ms 
       que serao utilizadas para validar o acionamento das teclas */
    configuracao_Timer1();
    
    // habilita a flag global de interrupcao
    sei();
}

// loop de varredura do teclado
void loop() {
    /* a logica para identificar se uma tecla foi acionada eh baixar o nivel
       de uma coluna e verificar se alguma linha foi colocada em nivel baixo */

    // coluna eh colocada em nivel baixo
    digitalWrite(colPins[activeColumn], LOW);

    // varre as linhas
    for (int row = 0; row <= ROWS ; row++) {

        // caso nao encontrou nenhuma linha ativa atribui o valor NO_ACTIVE_LINE
        if (row == ROWS) {
            activeLine = NO_ACTIVE_LINE;

        // caso encontre uma linha ativa atribui o valor da linha
        } else if (digitalRead(rowPins[row]) == LOW) {
            activeLine = row;

            // fluxo eh quebrado para evitar que a linha seja desativada pelo if anterior na ultima iteracao
            break;
        }
    }

    // coluna eh colocada em nivel alto
    digitalWrite(colPins[activeColumn], HIGH);

    // atualiza a coluna caso nao tenha encontrado nenhuma linha ativa e nao esteja em debounce
    if (activeLine == NO_ACTIVE_LINE && flagDebounce == FALSE) {
        updateColumn();
    }
}

// interrupcao a cada 50ms
ISR(TIMER1_COMPA_vect) {

    // valida se uma tecla esta ativa consecutivamente no intervalo de 50ms
    if (activeLine != NO_ACTIVE_LINE && flagDebounce == FALSE) {

        if (activeLine == lastActiveLine) {
            flagDebounce = TRUE;
            debouncedLine = activeLine; // salva a linha que foi acionada
        }

    // validacao se a tecla foi solta
    // eh feito duas passagens sendo um debounce para validar se a tecla foi solta
    } else if (activeLine == NO_ACTIVE_LINE && flagDebounce == TRUE) {

        debouncePass++;

        // na segunda passagem os estados sao resetados e a tecla eh impressa
        if (debouncePass >= 2) {

            char pressedKey = keypad[debouncedLine][activeColumn];
            Serial.println("Pressed key: " + String(pressedKey));

            debouncePass = 0;
            flagDebounce = FALSE;
        }   
    }

    // atualiza a ultima linha ativa no intervalo de 50ms
    lastActiveLine = activeLine;
}

// atualiza coluna ativa, variando de 0 a 2
void updateColumn() {
    activeColumn++;

    if (activeColumn >= COLS) {
        activeColumn = 0;
    }
}