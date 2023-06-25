/*  EA076C
    Modulo 2: Teclado matricial

    Alunos:
        Deivit Lopes Bonach     RA:166508
        Vinicius Henrique Luiz  RA:245342
*/

#include <Arduino.h>
#include <LiquidCrystal.h>

// definicao de constantes
#define ROWS 4
#define COLS 3
#define NO_ACTIVE_LINE 100
#define TRUE 1
#define FALSE 0

// arrays que mapeiam os pinos do arduino para as linhas e colunas do teclado
const char rowPins[ROWS] = {2, 3, 4, 5};
const char colPins[COLS] = {6, 7, 8};

// mapeamento do teclado para obtencao das teclas acionadas
char keypad[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

<<<<<<< HEAD
*/
#include <Arduino.h>
#include <avr/interrupt.h>
/* Create variables */
int columns[3] = {1, 1, 1};
int activeLine;
int pressedKey;
int flagKeyWasPressed = 0;
int lines[4] = {1, 1, 1, 1};
int state = 0;
volatile unsigned long contador = 0; // Variável para contar os segundos
int colunaAtiva;
int linhaAtiva;
int teclaConfimada;
=======
// configuracao display LCD
int RS = 19;
int E = 18;
int D4 = 17;
int D5 = 16;
int D6 = 15;
int D7 = 14;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

// declaracao de variaveis globais
char activeColumn = 0;
char currentLine = NO_ACTIVE_LINE;
char previousLine = NO_ACTIVE_LINE;
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

>>>>>>> 1cbc88b1c2fd7f7185235772d223f059f19d7cf6
void setup()
{
<<<<<<< HEAD
    // Configura o temporizador Timer1
    noInterrupts(); // Desabilita as interrupções
    TCCR1A = 0; // Configura o Timer1 em modo normal
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // Configura o prescaler em 64
    OCR1A = 1249; // Define o valor de comparação para interrupção a cada 50ms
    TIMSK1 = (1 << OCIE1A); // Habilita a interrupção do Timer1 no comparador A

    Serial.begin(9600);

    //Configuração entradas e saidas
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
=======
    // desabilita a flag global de interrupcao por precaucao
    cli();
>>>>>>> 1cbc88b1c2fd7f7185235772d223f059f19d7cf6

<<<<<<< HEAD
    //Saidas em nivel logico alto
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);

    interrupts(); // Habilita as interrupções
=======
    Serial.begin(9600); // set baud rate to 9600 bps
    lcd.begin(16,2); // configura a instancia de lcd como 16x2

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

    // exibe mensagem inicia no display LCD e no monitor serial
    Serial.println("Waiting for key to be pressed.");
    lcd.print("Waiting for key");
    lcd.setCursor(0, 1);
    lcd.print("to be pressed...");
    
    // habilita a flag global de interrupcao
    sei();
>>>>>>> 1cbc88b1c2fd7f7185235772d223f059f19d7cf6
}

<<<<<<< HEAD

// Rotina de interrupção do Timer1 (ocorre a cada segundo)
ISR(TIMER1_COMPA_vect) {

  contador++; // Incrementa o contador de segundos

  // Coloque o código que deseja executar a cada segundo dentro desta função
}

void loop()
{
    switch (state)
    { // state machine
    case 0:
        //coluna 1 em nivel logico 0v
        digitalWrite(11, LOW);
=======
void loop() {
    /* Funcionamento do loop de varredura:
>>>>>>> 1cbc88b1c2fd7f7185235772d223f059f19d7cf6

<<<<<<< HEAD
        //coluna 2 em nivel logico 5v
        digitalWrite(12, HIGH);

        //coluna3 em nivel logico 5v
        digitalWrite(13, HIGH);


        //Verifica tecla 1 pressionada
        if (digitalRead(10) == 0)
        {
            Serial.println("apertou a tecla 1");

            //salva tecla
            colunaAtiva = 1;
            linhaAtiva = 1;

            //salta para o estado do debouncing
            state = 3;
=======
        A cada iteracao do loop uma coluna eh colocada em nivel baixo e as linhas sao varridas.
    O acionamento de uma tecla faz com que a linha feche contato com a coluna assim ela passa para
    nivel baixo, assim caso uma linha seja encontrada em nivel baixo esta é a linha acionada.
    No if interno ao loop a variavel currentLine armazena o estado da linha, sendo ela uma linha
    valida ou o não acionamento caso nenhuma linha seja encontrada em nivel baixo.
        Apos a varredura das linhas, a coluna eh colocada em nivel alto e eh validado se a proxima
    coluna deve ser varrida.
        Caso tenha encontrado uma linha ativa a varredura continua na mesma coluna para realizar o 
    debounce da mesma. Caso esteja sendo feito o debounce de acionamento a varredura tambem continua
    na mesma coluna.

    */

    digitalWrite(colPins[activeColumn], LOW);   // coluna eh colocada em nivel baixo

    for (int row = 0; row < ROWS ; row++) {     // linhas sao varridas

        if (digitalRead(rowPins[row]) == LOW) { // caso encontrou linha ativa
            currentLine = row;
            break;
            
        } else if (row == ROWS - 1) {   // caso nao encontrou linha ativa ao final da varredura
            currentLine = NO_ACTIVE_LINE;
>>>>>>> 1cbc88b1c2fd7f7185235772d223f059f19d7cf6
        }
<<<<<<< HEAD

        //Verifica tecla 4 pressionada
        else if (digitalRead(9) == 0)
        {
            Serial.println("apertou a tecla 4");
            //salva tecla
            colunaAtiva = 1;
            linhaAtiva = 2;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla 7 pressionada
        else if (digitalRead(8) == 0)
        {
            Serial.println("apertou a tecla 7");
            //salva tecla
            colunaAtiva = 1;
            linhaAtiva = 3;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla * pressionada
        else if (digitalRead(7) == 0)
        {
            Serial.println("apertou a tecla *");
            //salva tecla
            colunaAtiva = 1;
            linhaAtiva = 4;

            //salta para o estado do debouncing
            state = 3;
        }
        else{
            // salta para case 1
            state = 1;
        }
        break;
    case 1:
        //coluna 1 em nivel logico 1
        digitalWrite(11, HIGH);

        //coluna 2 em nivel logico 0
        digitalWrite(12, LOW);

        //coluna3 em nivel logico 1
        digitalWrite(13, HIGH);


        //Verifica tecla 2 pressionada
        if (digitalRead(10) == 0)
        {
            Serial.println("apertou a tecla 2");

            //salva tecla
            colunaAtiva = 2;
            linhaAtiva = 1;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla 5 pressionada
        else if (digitalRead(9) == 0)
        {
            Serial.println("apertou a tecla 5");
            //salva tecla
            colunaAtiva = 2;
            linhaAtiva = 2;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla 8 pressionada
        else if (digitalRead(8) == 0)
        {
            Serial.println("apertou a tecla 8");
            //salva tecla
            colunaAtiva = 2;
            linhaAtiva = 3;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla 0 pressionada
        else if (digitalRead(7) == 0)
        {
            Serial.println("apertou a tecla 0");
            //salva tecla
            colunaAtiva = 2;
            linhaAtiva = 4;

            //salta para o estado do debouncing
            state = 3;
        }
        else{
            // salta para case 2
            state = 2;
        }
        break;
    case 2:
        //coluna 1 em nivel logico 0v
        digitalWrite(11, HIGH);

        //coluna 2 em nivel logico 5v
        digitalWrite(12, HIGH);

        //coluna3 em nivel logico 5v
        digitalWrite(13, LOW);


        //Verifica tecla 3 pressionada
        if (digitalRead(10) == 0)
        {
            Serial.println("apertou a tecla 3");

            //salva tecla
            colunaAtiva = 3;
            linhaAtiva = 1;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla 6 pressionada
        else if (digitalRead(9) == 0)
        {
            Serial.println("apertou a tecla 6");
            //salva tecla
            colunaAtiva = 3;
            linhaAtiva = 2;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla 9 pressionada
        else if (digitalRead(8) == 0)
        {
            Serial.println("apertou a tecla 9");
            //salva tecla
            colunaAtiva = 3;
            linhaAtiva = 3;

            //salta para o estado do debouncing
            state = 3;
        }

        //Verifica tecla # pressionada
        else if (digitalRead(7) == 0)
        {
            Serial.println("apertou a tecla #");
            //salva tecla
            colunaAtiva = 3;
            linhaAtiva = 4;

            //salta para o estado do debouncing
            state = 3;
        }
        else{
            // salta para case 1
            state = 1;
        }
        break;
    //Deboucing tecla precionada
    case 3:
        // do something
        break;
    
    //Confirma tecla precionada
    case 4:
        //Confirma tecla pressionada
        teclaConfimada = 1;

        //salta para o estado do debouncing tecla solta
        state = 5;
        break;
    
    //Deboucing tecla solta
    case 5:
        // do something
        break;
    case 6:
        // do something
        break;
    case 7:
        // do something
        break;
=======
>>>>>>> 1cbc88b1c2fd7f7185235772d223f059f19d7cf6
    }

    digitalWrite(colPins[activeColumn], HIGH);  // coluna eh colocada em nivel alto

    // atualiza a coluna caso nao tenha encontrado nenhuma linha ativa e nao esteja em debounce
    if (currentLine == NO_ACTIVE_LINE && debouncedLine == NO_ACTIVE_LINE) {
        updateColumn();
    }
}

ISR(TIMER1_COMPA_vect) {
    /* Funcionamento do vetor de interrupcao:
    
        Toda vez que o fluxo passa pelo vetor, o estado da ultima linha ativa ou nao eh salvo.
    Assim eh possivel comparar o par de estados (valor da linha atual e anterior) no 
    intervalo de 50ms para fazer o debounce de acionamento e desacionamento da tecla.
        O if externo garante que estamos analisando uma mesma linha acionada no intervalo
    de 50ms ou o nao acionamento, pois em ambos os casos precisamos do mesmo estado da linha
    consecutivamente.
        Dentro do if, o primeiro if garante que estamos analisando uma linha acionada e que 
    nao estamos no fluxo de debounce do desacionamento, dentro do if a variavel debouncedLine 
    vai ser atribuida com o valor da linha acionada, esta mesma variavel eh utilizada para o 
    else if seguinte que verifica se a tecla foi solta.
        O else if valida se a linha esta desacionada, pois no intervalo de 50ms 
    teremos o par de nenhuma linha ativa e a variavel debounceLine tera o valor de uma linha
    valida, indicando que estamos realizando o debounce de desacionamento da tecla.

    */

    // no intervalo de 50ms as mesmas linhas estavam ativadas ou desativadas
    if (currentLine == previousLine) {

        // as linhas eram validas e nao estava sendo feito debounce de desacionamento
        if (currentLine != NO_ACTIVE_LINE && debouncedLine == NO_ACTIVE_LINE) {

            debouncedLine = currentLine; // salva a linha que foi feito debounce de acionamento

        // este eh o debounce de desacionamento
        // as linhas eram nulas e debouncedLine indica que ja foi feito o debounce de acionamento, logo o ciclo foi completo
        } else if (currentLine == NO_ACTIVE_LINE && debouncedLine != NO_ACTIVE_LINE) {

            char pressedKey = keypad[debouncedLine][activeColumn];
            Serial.println("Pressed key: " + String(pressedKey));
            showInDisplay("Pressed key: " + String(pressedKey));

            debouncedLine = NO_ACTIVE_LINE;
        }
    }

    previousLine = currentLine;
}

// atualiza coluna sendo varrida, variando de 0 a 2
void updateColumn() {
    activeColumn++;

    if (activeColumn >= COLS) {
        activeColumn = 0;
    }
}

// envia mensagem para o display lcd
void showInDisplay(String phrase) {
    lcd.clear();
    lcd.print(phrase);
}