#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

//define o LED de saída
#define GPIO_LED_RED 13
#define GPIO_LED_BLUE 12
#define GPIO_LED_GREEN 11
#define GPIO_BUZZER 21

//define os pinos do teclado com as portas GPIO
uint columns[4] = {4, 3, 2, 28};   //Colunas
uint rows[4] = {8, 7, 6, 5};     //linhas

//mapa de teclas_ Relaciona cada botão do teclado com seu respectivo caractere.
char KEY_MAP[16] = {
    '1', '2' , '3', 'A',
    '4', '5' , '6', 'B',
    '7', '8' , '9', 'C',
    '*', '0' , '#', 'D'
};

uint _columns[4];
uint _rows[4];
char _matrix_values[16]; //Guarda o mapeamento das teclas do teclado.
uint all_columns_mask = 0x0; //identificar quais colunas estão ativas.
uint column_mask[4];  // Armazena a máscara individual de cada coluna.

//Mostra um número inteiro em binário. Útil para depuração.
void imprimir_binario(int num) {
 int i;
 for (i = 31; i >= 0; i--) 
 {
  (num & (1 << i)) ? printf("1") : printf("0");
 }
}

//inicializa o keypad
void pico_keypad_init(uint columns[4], uint rows[4], char matrix_values[16]) {

    for (int i = 0; i < 16; i++) {
        _matrix_values[i] = matrix_values[i];
    }

    for (int i = 0; i < 4; i++) {

        _columns[i] = columns[i];
        _rows[i] = rows[i];

        gpio_init(_columns[i]);
        gpio_init(_rows[i]);

        gpio_set_dir(_columns[i], GPIO_IN); // Colunas como entrada
        gpio_set_dir(_rows[i], GPIO_OUT); // Linhas como saída  

        gpio_put(_rows[i], 1); // Linhas iniciam em nível alto

        all_columns_mask = all_columns_mask + (1 << _columns[i]);
        column_mask[i] = 1 << _columns[i];
    }
}

//coleta o caracter pressionado
char pico_keypad_get_key(void) {
    int row;
    uint32_t cols;
    bool pressed = false;

    cols = gpio_get_all();
    cols = cols & all_columns_mask;
    imprimir_binario(cols);
    
    if (cols == 0x0) { // Nenhuma tecla pressionada
        return 0;
    }

    // Desativa todas as linhas
    for (int j = 0; j < 4; j++) {
        gpio_put(_rows[j], 0);
    }
    // Varredura para identificar a linha da tecla
    for (row = 0; row < 4; row++) {

        gpio_put(_rows[row], 1);

        busy_wait_us(10000);

        cols = gpio_get_all();
        gpio_put(_rows[row], 0);
        cols = cols & all_columns_mask;
        if (cols != 0x0) {
            break;
        }   
    }
    
    for (int i = 0; i < 4; i++) {
        gpio_put(_rows[i], 1);
    }

    if (cols == column_mask[0]) {
        return (char)_matrix_values[row * 4 + 0];
    }
    else if (cols == column_mask[1]) {
        return (char)_matrix_values[row * 4 + 1];
    }
    if (cols == column_mask[2]) {
        return (char)_matrix_values[row * 4 + 2];
    }
    else if (cols == column_mask[3]) {
        return (char)_matrix_values[row * 4 + 3];
    }
    else {
        return 0; // Se não identificar nenhuma tecla
    }
}

//função principal
int main() {
    stdio_init_all();
    pico_keypad_init(columns, rows, KEY_MAP);
    char caracter_press;

    gpio_init(GPIO_LED_RED);
    gpio_set_dir(GPIO_LED_RED, GPIO_OUT);
    
    gpio_init(GPIO_LED_BLUE);
    gpio_set_dir(GPIO_LED_BLUE, GPIO_OUT);
    
    gpio_init(GPIO_LED_GREEN);
    gpio_set_dir(GPIO_LED_GREEN, GPIO_OUT);

    gpio_init(GPIO_BUZZER);
    gpio_set_dir(GPIO_BUZZER, GPIO_OUT);

    while (true) {
        caracter_press = pico_keypad_get_key();
        
        if (caracter_press !=0){
        printf("\nTecla pressionada: %c\n", caracter_press);

            //Avaliação de caractere para os LED e BUZZER
            if (caracter_press=='A')
            {
                gpio_put(GPIO_LED_RED,true);
                gpio_put(GPIO_LED_BLUE,false);
                gpio_put(GPIO_LED_GREEN,false);
                gpio_put(GPIO_BUZZER,false);
            } else if (caracter_press=='B')
                {
                gpio_put(GPIO_LED_BLUE,true);
                gpio_put(GPIO_LED_RED,false);
                gpio_put(GPIO_LED_GREEN,false);
                gpio_put(GPIO_BUZZER,false);
                }
            else if (caracter_press=='C')
                {
                gpio_put(GPIO_LED_GREEN,true);
                gpio_put(GPIO_LED_RED,false);
                gpio_put(GPIO_LED_BLUE,false);
                gpio_put(GPIO_BUZZER,false);
                }
            else if (caracter_press=='D')
                {
                gpio_put(GPIO_LED_RED,true);
                gpio_put(GPIO_LED_BLUE,true);
                gpio_put(GPIO_LED_GREEN,true);
                gpio_put(GPIO_BUZZER,false);
                }
            else if (caracter_press=='#')
                {
                gpio_put(GPIO_BUZZER,true);
                gpio_put(GPIO_LED_RED,false);
                gpio_put(GPIO_LED_GREEN,false);
                gpio_put(GPIO_LED_BLUE,false);
                }
     }      else {
                gpio_put(GPIO_LED_RED,false);
                gpio_put(GPIO_LED_BLUE,false);
                gpio_put(GPIO_LED_GREEN,false);
                gpio_put(GPIO_BUZZER,false);
            }

        busy_wait_us(1000); 
    }
}