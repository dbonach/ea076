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
char activeColumn = 0;
char activeLine = NO_ACTIVE_LINE;
char lastActiveLine = NO_ACTIVE_LINE;
char debouncedLine = NO_ACTIVE_LINE;

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

void loop() {
    /* Funcionamento do loop de varredura:

        A cada iteracao do loop uma coluna eh colocada em nivel baixo e as linhas sao varridas.
    Caso uma linha seja encontrada em nivel baixo, a variavel activeLine armazena a linha ativa 
    e o fluxo eh quebrado para evitar que a a variavel activeLine seja marcada como NO_ACTIVE_LINE.
    Caso nenhuma linha seja encontrada em nivel baixo, a variavel activeLine eh marcada como NO_ACTIVE_LINE.
        Apos a varredura das linhas, a coluna eh colocada em nivel alto e eh validado se a proxima
    coluna deve ser varrida.
        Caso tenha encontrado uma linha ativa a varredura continua na mesma coluna para realizar o 
    debounce da mesma. Caso esteja sendo feito o debounce de acionamento a varredura tambem continua
    na mesma coluna.

    */

    digitalWrite(colPins[activeColumn], LOW);   // coluna eh colocada em nivel baixo

    for (int row = 0; row < ROWS ; row++) {     // linhas sao varridas

        if (digitalRead(rowPins[row]) == LOW) { // caso encontrou linha ativa
            activeLine = row;
            break;
            
        } else if (row == ROWS - 1) {           // caso nao encontrou linha ativa
            activeLine = NO_ACTIVE_LINE;
        }
    }

    digitalWrite(colPins[activeColumn], HIGH);  // coluna eh colocada em nivel alto

    // atualiza a coluna caso nao tenha encontrado nenhuma linha ativa e nao esteja em debounce
    if (activeLine == NO_ACTIVE_LINE && debouncedLine == NO_ACTIVE_LINE) {
        updateColumn();
    }
}

ISR(TIMER1_COMPA_vect) {
    /* Funcionamento do vetor de interrupcao:
    
        Toda vez que o fluxo passa pelo vetor, o estado da ultima linha ativa eh salvo.
    Assim eh possivel comparar o par de estados (linha ativa atual e ultima linha ativa) 
    para fazer o debounce de acionamento e desacionamento da tecla.
        O primeiro if verifica se no intervalo de 50ms uma linha foi acionada consecutivamente.
    Isto sendo verdadeiro a variavel debouncedLine salva a linha que foi validada forcando o
    fluxo entrar no segundo if nas proximas iteracoes, onde eh avaliado se no intervalo de 50ms 
    as linhas estavam desativadas consecutivamente, ou seja, se a tecla foi solta. 
    
    */

    if (activeLine == lastActiveLine && activeLine != NO_ACTIVE_LINE && debouncedLine == NO_ACTIVE_LINE) {

            debouncedLine = activeLine;

    } else if (activeLine == lastActiveLine && activeLine == NO_ACTIVE_LINE && debouncedLine != NO_ACTIVE_LINE) {

        char pressedKey = keypad[debouncedLine][activeColumn];
        Serial.println("Pressed key: " + String(pressedKey));

        debouncedLine = NO_ACTIVE_LINE;
    }

    lastActiveLine = activeLine;
}

// atualiza coluna ativa, variando de 0 a 2
void updateColumn() {
    activeColumn++;

    if (activeColumn >= COLS) {
        activeColumn = 0;
    }
}