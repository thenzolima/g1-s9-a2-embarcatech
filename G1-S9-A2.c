#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs e buzzer
#define LED_VERMELHO 13 
#define LED_AZUL 12
#define LED_VERDE 11
#define BUZZER 21

// Definição dos pinos do teclado
uint colunas[4] = {4, 3, 2, 28};  //colunas
uint linhas[4] = {8, 7, 6, 5};    //linhas

// Mapeamento das teclas do teclado
char mapa_teclas[16] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D'
};

// Variáveis internas para gerenciar o teclado
uint _colunas[4];
uint _linhas[4];
char _valores_matriz[16];
uint mascara_colunas = 0x0;
uint mascara_coluna[4];

// Função para inicializar o teclado
void inicializar_teclado(uint colunas[4], uint linhas[4], char valores[16]) {
    for (int i = 0; i < 16; i++) {
        _valores_matriz[i] = valores[i];
    }

    for (int i = 0; i < 4; i++) {
        _colunas[i] = colunas[i];
        _linhas[i] = linhas[i];

        gpio_init(_colunas[i]);
        gpio_init(_linhas[i]);

        gpio_set_dir(_colunas[i], GPIO_IN);  // Colunas como entrada
        gpio_set_dir(_linhas[i], GPIO_OUT);  // Linhas como saída

        gpio_put(_linhas[i], 1);  // Linhas começam em nível alto

        mascara_colunas += (1 << _colunas[i]);
        mascara_coluna[i] = 1 << _colunas[i];
    }
}

// Função para pegar a tecla pressionada
char pegar_tecla(void) {
    int linha;
    uint32_t colunas_ativas;

    colunas_ativas = gpio_get_all();
    colunas_ativas = colunas_ativas & mascara_colunas;

    if (colunas_ativas == 0x0) {  // Nenhuma tecla pressionada
        return 0;
    }

    // Desativa todas as linhas
    for (int j = 0; j < 4; j++) {
        gpio_put(_linhas[j], 0);
    }

    // Verifica qual linha foi pressionada
    for (linha = 0; linha < 4; linha++) {
        gpio_put(_linhas[linha], 1);
        busy_wait_us(300);
        colunas_ativas = gpio_get_all();
        gpio_put(_linhas[linha], 0);
        colunas_ativas = colunas_ativas & mascara_colunas;
        if (colunas_ativas != 0x0) {
            break;
        }
    }

    for (int i = 0; i < 4; i++) {
        gpio_put(_linhas[i], 1);
    }

    // Retorna a tecla pressionada
   for (int i = 0; i < 4; i++) {
    if (colunas_ativas == mascara_coluna[i]) {
        return _valores_matriz[linha * 4 + i];
    }
}

    return 0;  // Se não identificar nenhuma tecla
}

// Função para controlar LEDs e buzzer
void controlar_leds_buzzer(char tecla) {
    gpio_put(BUZZER, false);  // Desliga o buzzer

    // Desliga todos os LEDs
    gpio_put(LED_VERMELHO, false);
    gpio_put(LED_AZUL, false);
    gpio_put(LED_VERDE, false);

    // Acende o LED correspondente e emite som dependendo da tecla (#)
    switch (tecla) {
        case 'A':
            gpio_put(LED_VERMELHO, true);
            break;
        case 'B':
            gpio_put(LED_AZUL, true);
            break;
        case 'C':
            gpio_put(LED_VERDE, true);
            break;
        case 'D':
            gpio_put(LED_VERMELHO, true);
            gpio_put(LED_AZUL, true);
            gpio_put(LED_VERDE, true);
            break;
        case '#':
            // Frequência de 1500Hz para o buzzer
            for (int i = 0; i < 1000; i++) {

                    gpio_put(BUZZER, true);
                    busy_wait_us(333); // Meio ciclo (0.666ms)
                    gpio_put(BUZZER, false);
                    busy_wait_us(333); // Outro meio ciclo (0.666ms)
                
            }
            break;
        default:
        break;
    }
}

// Função principal
int main() {
    stdio_init_all();
    inicializar_teclado(colunas, linhas, mapa_teclas);
    char tecla_press;

    // Inicializa LEDs e buzzer
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);

    while (true) {
        tecla_press = pegar_tecla();

        if (tecla_press != 0) {

            printf("\nTecla pressionada: %c\n", tecla_press);
            controlar_leds_buzzer(tecla_press); // Controla LEDs e buzzer

        } else {

            // Se nenhuma tecla for pressionada, desliga tudo
            gpio_put(LED_VERMELHO, false);
            gpio_put(LED_AZUL, false);
            gpio_put(LED_VERDE, false);
            gpio_put(BUZZER, false);
        }
    }
}
