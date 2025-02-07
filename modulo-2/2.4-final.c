/*  EA076C
    Módulo 1: Leitura e escrita na memória EEPROM 24C16

    Alunos:
        Deivit Lopes Bonach     RA:166508
        Vinicius Henrique Luiz  RA:245342
*/

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Wire.h>

// definicao de constantes
#define ROWS 4
#define COLS 3
#define NO_ACTIVE_LINE 100
#define TRUE 1
#define FALSE 0
#define EEPROM_ADDR 0x50  // endereço da memória EEPROM 24C16
#define ADDRESS 32        // endereco PCF

// arrays que mapeiam os pinos do arduino para as linhas e colunas do teclado
const char rowPins[ROWS] = {2, 3, 4, 5};
const char colPins[COLS] = {6, 7, 8};

// mapeamento do teclado para obtencao das teclas acionadas
char keypad[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

// configuracao display LCD
int RS = 10;
int E = 9;
int D4 = 17;
int D5 = 16;
int D6 = 15;
int D7 = 11;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

// declaracao de variaveis globais
char activeColumn = 0;
char currentLine = NO_ACTIVE_LINE;
char previousLine = NO_ACTIVE_LINE;
char debouncedLine = NO_ACTIVE_LINE;
char pressedKey = NO_ACTIVE_LINE;
char selectedComand = 0;
char coleta = FALSE;
int contadorMemoria = 0;
// Definir o pino de entrada analógica onde o sensor está conectado
const int pinoSensor = A0;
int temp;
float temperatura;
char contadorInterrucao = 0;
int availableSpace = 1023;
char salveTemp = FALSE;

void setup() {
    // desabilita a flag global de interrupcao por precaucao
    cli();

    Serial.begin(9600);  // set baud rate to 9600 bps
    Wire.begin();        // Join I2C bus
    lcd.begin(16, 2);    // configura a instancia de lcd como 16x2

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
    showInDisplay("bem-vindo", "escolha funcao");

    // habilita a flag global de interrupcao
    sei();
}

// configuracao do timer1 para gerar uma interrupcao a cada 50ms
void configuracao_Timer1() {
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

void loop() {
    // Ler o valor analógico do sensor
    int valorSensor = analogRead(pinoSensor);

    // Converter o valor analógico para temperatura em graus Celsius
    float tensao = valorSensor * (5.0 / 1023.0);  // Converter o valor para tensão (0-5V)
    temperatura = tensao * 100;                   // Cada 10mV de variação corresponde a 1°C

    // exibe temperatura no display 7 segmentos
    sendRpmTo4DigitDisplay(temp);

    if (salveTemp == TRUE) {
        // escreve na memoria
        escrever(contadorMemoria++, temp >> 8);
        escrever(contadorMemoria++, temp);
        Serial.println("tempe gravada: " + String(temp));
        salveTemp = FALSE;
    }

    // Menu
    switch (pressedKey) {
        case '1':
            showInDisplay("1 - reset", "confirmar?");
            pressedKey = NO_ACTIVE_LINE;
            selectedComand = 1;
            break;
        case '2':
            showInDisplay("2 - status", "confirmar?");
            pressedKey = NO_ACTIVE_LINE;
            selectedComand = 2;
            break;
        case '3':
            showInDisplay("3 - start", "confirmar?");
            pressedKey = NO_ACTIVE_LINE;
            selectedComand = 3;
            break;
        case '4':
            showInDisplay("4 - stop", "confirmar?");
            pressedKey = NO_ACTIVE_LINE;
            selectedComand = 4;
            break;
        case '5':
            showInDisplay("5 - transf. dado", "confirmar?");
            pressedKey = NO_ACTIVE_LINE;
            selectedComand = 5;
            break;
        case '*':
            showInDisplay("cancelado", "escolha funcao");
            pressedKey = NO_ACTIVE_LINE;
            selectedComand = 0;
            break;
        case '#':
            pressedKey = NO_ACTIVE_LINE;
            executeComand();
            selectedComand = 0;
            break;
    }

    varreduraDoTeclado();
}

void executeComand() {
    switch (selectedComand) {
        case 1:
            resetMemory();
            break;
        case 2:
            memoryStatus();
            break;
        case 3:
            startColect();
            break;
        case 4:
            finishColect();
            break;
        case 5:
            transferData();
            break;
    }
}

void varreduraDoTeclado() {
    /* Funcionamento do loop de varredura:

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

    digitalWrite(colPins[activeColumn], LOW);  // coluna eh colocada em nivel baixo

    for (int row = 0; row < ROWS; row++) {  // linhas sao varridas

        if (digitalRead(rowPins[row]) == LOW) {  // caso encontrou linha ativa
            currentLine = row;
            break;
        } else if (row == ROWS - 1) {  // caso nao encontrou linha ativa ao final da varredura
            currentLine = NO_ACTIVE_LINE;
        }
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
            debouncedLine = currentLine;  // salva a linha que foi feito debounce de acionamento

            // este eh o debounce de desacionamento
            // as linhas eram nulas e debouncedLine indica que ja foi feito o debounce de acionamento, logo o ciclo foi completo
        } else if (currentLine == NO_ACTIVE_LINE && debouncedLine != NO_ACTIVE_LINE) {
            pressedKey = keypad[debouncedLine][activeColumn];
            debouncedLine = NO_ACTIVE_LINE;
        }
    }

    previousLine = currentLine;

    contadorInterrucao++;

    if (contadorInterrucao == 40) {
        // converte temperatura float para int
        temp = int(temperatura * 100);

        if (coleta == TRUE) {
            salveTemp = TRUE;
        }

        contadorInterrucao = 0;
    }
}

// atualiza coluna sendo varrida, variando de 0 a 2
void updateColumn() {
    activeColumn++;

    if (activeColumn >= COLS) {
        activeColumn = 0;
    }
}

// envia mensagem para o display lcd
void showInDisplay(String firstLinePhrase, String secondLinePhrase) {
    lcd.clear();
    lcd.print(firstLinePhrase);
    lcd.setCursor(0, 1);
    lcd.print(secondLinePhrase);
}

/* Escrita de um byte na memoria */
void escrever(unsigned int add, unsigned char dado) {
    /* Desloca 8 bits a direita para separar os 3 bits mais significativos do endereço */
    unsigned int byteMstAdd = add >> 8;

    /* byte menos significativo do endereço da memoria */
    unsigned char byteLstAdd = (unsigned char)add;

    /* Atribuição de uma mascara para juntar os bits do endereço da
    EEPROM com os 3 primeiros bits do endereço do dado */
    int primeiroByteTransmitido = (EEPROM_ADDR | byteMstAdd);

    /* Processo de escrita do byte dado na EEPROM no endereço add */
    Wire.beginTransmission(primeiroByteTransmitido);
    Wire.write(byteLstAdd);
    Wire.write(dado);
    Wire.endTransmission();
    delay(5);
}

unsigned char ler(unsigned int add) {
    /* Desloca 8 bits a direita para separar os 3 bits mais significativos do endereço */
    unsigned int byteMstAdd = add >> 8;

    /* byte menos significativo do endereço da memoria */
    unsigned int byteLstAdd = (unsigned char)add;

    /* Atribuição de uma mascara para juntar os bits do endereço da
    EEPROM com os 3 primeiros bits do endereço do dado */
    int primeiroByteTransmitido = (EEPROM_ADDR | byteMstAdd);

    /* Escrita com o objetivo de escolher o endereço a ser lido */
    Wire.beginTransmission(primeiroByteTransmitido);  // Inicia a transmissão para o endereço da memória
    Wire.write(byteLstAdd);                           // Escreve o byte restante de endereço de memoria
    Wire.endTransmission();                           // Executa a transmissão
    Wire.requestFrom(primeiroByteTransmitido, 1);     // Solicita 1 byte de dados da memória

    byte data = Wire.read();

    return data;
}

void resetMemory(void) {
    // zera o contador de memoria
    escrever(0x7FF, 0x00);
    escrever(0x7FE, 0x00);

    showInDisplay("reset concluido", "");
}

// exibe valor do contador da memoria
void memoryStatus(void) {
    // Le penultimo endereco
    unsigned char contLSB = ler(0x7FE);

    Serial.println("Contador memory LSB: " + String(contLSB, HEX));

    // Le ultimo endereco
    unsigned char contMSB = ler(0x7FF);

    Serial.println("Contador memory MSB: " + String(contMSB, HEX));

    // Atribuição de uma mascara para juntar os bits do contador
    contadorMemoria = (contMSB << 8) | contLSB;

    Serial.println("Contador memory: " + String(contadorMemoria));

    // Numero memorias livres
    int freeMemory = availableSpace - contadorMemoria / 2;

    showInDisplay("gravado: " + String(contadorMemoria), "disponivel: " + String(freeMemory));
}

// Habilita a flag coleta
void startColect(void) {
    showInDisplay("start", "inicio da coleta");
    coleta = TRUE;
}

// Finaliza a coleta
void finishColect(void) {
    showInDisplay("stop: fim", "no dados: " + String(contadorMemoria / 2));
    coleta = FALSE;
}

void transferData(void) {
    showInDisplay("transf. dados", "esc. qtde:");

    String input = "";

    while (true) {
        varreduraDoTeclado();

        if (pressedKey != NO_ACTIVE_LINE) {
            if (pressedKey == '*') {
                break;
            }

            if (pressedKey == '#') {
                if (contadorMemoria < input.toInt()) {
                    showInDisplay("quantidade", "indisponivel");
                }

                for (int i = 0; i < input.toInt() * 2; i += 2) {
                    unsigned char msbByte = ler(i);
                    unsigned char lsbByte = ler(i + 1);

                    int value = (msbByte << 8) | lsbByte;

                    Serial.println("Valor: " + String(value));
                }

                showInDisplay("transf. concluida", "");
                break;
            }

            input += String(pressedKey);
            pressedKey = NO_ACTIVE_LINE;

            showInDisplay("transf. dados", "esc. qtde: " + input);
        }
    }
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
    sendDigit(112 | number);  // 1110000 OR number
}

/* Encode tens */
void encodeDigit1(int number) {
    sendDigit(176 | number);  // 11010000 OR number
}

/* Encode hundreds */
void encodeDigit2(int number) {
    sendDigit(208 | number);  // 10110000 OR number
}

/* Encode thousands (more valuable) */
void encodeDigit3(int number) {
    sendDigit(224 | number);  // 01110000 OR number
}

/* Send encoded values through I2C bus */
void sendDigit(int value) {
    Wire.beginTransmission(ADDRESS);  // Inicio de transmissao entre o arduino e o PCF de endereco 0100000b
    Wire.write(value);                // Prepara os bits (value) a serem transmitidos
    Wire.endTransmission();           // Realiza a transmissao do dado
}
