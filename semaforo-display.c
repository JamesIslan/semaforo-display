#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5

#define I2C_SDA 14
#define I2C_SCL 15

char *TEXTO_SEMAFORO_SIGA[] = { // Texto do OLED quando a luz verde está acesa
  "               ",
  "               ",
  " SINAL ABERTO  ",
  "               ",
  " ATRAVESSE COM ",
  "    CUIDADO    ",
  "               ",
  "               "};

char *TEXTO_SEMAFORO_ATENCAO[] = { // Texto do OLED quando a luz amarela está acesa
  "               ",
  "               ",
  "   SINAL DE    ",
  "    ATENCAO    ",
  "               ",
  "  PREPARE SE   ",
  "               ",
  "               "};

char *TEXTO_SEMAFORO_PARE[] = { // Texto do OLED quando a luz vermelha está acesa
  "               ",
  "               ",
  " SINAL FECHADO ",
  "               ",
  "    AGUARDE    ",
  "               ",
  "               ",
  "               "};

int A_state = 0; // Botão A vem definido como desligado por padrão

void MostrarTexto(char *text[], uint8_t *ssd, struct render_area *frame_area, size_t line_count) { // Mostra o texto na tela
  memset(ssd, 0, ssd1306_buffer_length);
  int y_axis = 0;

  for (size_t index = 0; index < line_count; index++) { // Desenha o texto no OLED, linha por linha
    ssd1306_draw_string(ssd, 5, y_axis, text[index]);
    y_axis += 8;
  }

  render_on_display(ssd, frame_area);
}

void SinalAberto() {
  gpio_put(LED_R_PIN, 0);
  gpio_put(LED_G_PIN, 1);
  gpio_put(LED_B_PIN, 0);
}

void SinalAtencao() {
  gpio_put(LED_R_PIN, 1);
  gpio_put(LED_G_PIN, 1);
  gpio_put(LED_B_PIN, 0);
}

void SinalFechado() {
  gpio_put(LED_R_PIN, 1);
  gpio_put(LED_G_PIN, 0);
  gpio_put(LED_B_PIN, 0);
}

int EsperarPressionamentoBotao(int timeMS) { // Checa constantemente se BTN_A está pressionado
  for (int i = 0; i < timeMS; i++) {
    A_state = !gpio_get(BTN_A_PIN);
    if (A_state == 1) {
      return 1;
    }
    sleep_ms(1);
  }
  return 0;
}

void InicializarPerifericos() {
  // INICIALIZAÇÃO DE TIPOS STDIO
  stdio_init_all();

  // INICIALIZA E CONFIGURA LEDS
  gpio_init(LED_R_PIN);
  gpio_set_dir(LED_R_PIN, GPIO_OUT);
  gpio_init(LED_G_PIN);
  gpio_set_dir(LED_G_PIN, GPIO_OUT);
  gpio_init(LED_B_PIN);
  gpio_set_dir(LED_B_PIN, GPIO_OUT);

  // INICIALIZA E CONFIGURA BOTÕES
  gpio_init(BTN_A_PIN);
  gpio_set_dir(BTN_A_PIN, GPIO_IN);
  gpio_pull_up(BTN_A_PIN);

  // INICIALIZA E CONFIGURA TELA
  i2c_init(i2c1, ssd1306_i2c_clock * 400);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  ssd1306_init();
}

int main() {
  InicializarPerifericos();
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);
  uint8_t ssd[ssd1306_buffer_length];

  while (true) {
    SinalFechado(); // Sinal fechado por 8 segundos
    MostrarTexto(TEXTO_SEMAFORO_PARE, ssd, &frame_area, count_of(TEXTO_SEMAFORO_PARE));

    A_state = EsperarPressionamentoBotao(8000); // Waits for the button to be pressed

    if (A_state) {    // Quando o botão é pressionado...
      SinalAtencao(); // Sinal de atenção por 5 segundos
      MostrarTexto(TEXTO_SEMAFORO_ATENCAO, ssd, &frame_area, count_of(TEXTO_SEMAFORO_ATENCAO));
      sleep_ms(5000);

      SinalAberto(); // Sinal aberto por 10 segundos
      MostrarTexto(TEXTO_SEMAFORO_SIGA, ssd, &frame_area, count_of(TEXTO_SEMAFORO_SIGA));
      sleep_ms(10000);

    } else {          // Quando o botão não é pressionado...
      SinalAtencao(); // Sinal de atenção por 2 segundos
      MostrarTexto(TEXTO_SEMAFORO_ATENCAO, ssd, &frame_area, count_of(TEXTO_SEMAFORO_ATENCAO));
      sleep_ms(2000);

      SinalAberto(); // Sinal aberto por 8 segundos
      MostrarTexto(TEXTO_SEMAFORO_SIGA, ssd, &frame_area, count_of(TEXTO_SEMAFORO_SIGA));
      sleep_ms(8000);
    }
  }

  return 0;
}
