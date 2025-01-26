#include <ctype.h>
#include <locale.h>
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

int A_state = 0; // Button A is set to OFF by default
uint8_t ssd[ssd1306_buffer_length];

struct render_area frame_area = {
  start_column : 0,
  end_column : ssd1306_width - 1,
  start_page : 0,
  end_page : ssd1306_n_pages - 1
};

void display_text(char **text, uint8_t *ssd, struct render_area *frame_area) { // Display text on OLED
  int y_axis = 0;

  for (uint index = 0; index <= count_of(text); index++) {
    ssd1306_draw_string(ssd, 5, y_axis, text[index]);
    y_axis += 8;
  }

  render_on_display(ssd, frame_area);
}

void clear_display() {
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);
}

void set_traffic_light_go() {
  gpio_put(LED_R_PIN, 0);
  gpio_put(LED_G_PIN, 1);
  gpio_put(LED_B_PIN, 0);
}

void set_traffic_light_caution() {
  gpio_put(LED_R_PIN, 1);
  gpio_put(LED_G_PIN, 1);
  gpio_put(LED_B_PIN, 0);
}

void set_traffic_light_stop() {
  gpio_put(LED_R_PIN, 1);
  gpio_put(LED_G_PIN, 0);
  gpio_put(LED_B_PIN, 0);
}

void set_display_go(char **text) {
  // text[0] = "";
  text[0] = "SINAL ABERTO";
  // text[2] = "";
  text[1] = "ATRAVESSAR";
  // text[4] = " ",
  text[2] = "COM CUIDADO";
  text[3] = " ";
  display_text(text, ssd, &frame_area);
}

void set_display_caution(char **text) {
  // text[0] = "";
  text[0] = "SINAL DE";
  text[1] = "ATENCAO";
  // text[2] = "";
  text[2] = "PREPARE-SE";
  text[3] = " ";
  display_text(text, ssd, &frame_area);
}

void set_display_stop(char **text) {
  // text[0] = "";
  text[0] = "SINAL FECHADO";
  // text[2] = "";
  text[1] = "AGUARDE";
  text[2] = " ";
  display_text(text, ssd, &frame_area);
}

int wait_for_button_pressing(int timeMS) {
  for (int i = 0; i < timeMS; i = i + 100) {
    A_state = !gpio_get(BTN_A_PIN);
    if (A_state == 1) {
      return 1;
    }
    sleep_ms(100);
  }
  return 0;
}

void run_peripherals_setup() {
  // INITIALIZE STDIO TYPES
  stdio_init_all();

  // START AND CONFIGURE LEDS
  gpio_init(LED_R_PIN);
  gpio_set_dir(LED_R_PIN, GPIO_OUT);
  gpio_init(LED_G_PIN);
  gpio_set_dir(LED_G_PIN, GPIO_OUT);
  gpio_init(LED_B_PIN);
  gpio_set_dir(LED_B_PIN, GPIO_OUT);

  // START AND CONFIGURE BUTTON
  gpio_init(BTN_A_PIN);
  gpio_set_dir(BTN_A_PIN, GPIO_IN);
  gpio_pull_up(BTN_A_PIN);

  // START AND CONFIGURE DISPLAY
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  ssd1306_init();
}

int main() {
  setlocale(LC_ALL, "Portuguese"); // Ler acentos
  run_peripherals_setup();
  char *text[5];

  calculate_render_area_buffer_length(&frame_area);

  while (true) {
    set_traffic_light_stop();
    clear_display();
    set_display_stop(text);

    A_state = wait_for_button_pressing(8000); // Waits for the button to be pressed

    if (A_state) {                 // When the button is pressed...
      set_traffic_light_caution(); // Caution signal for 5 seconds
      clear_display();
      set_display_caution(text);
      sleep_ms(5000);

      set_traffic_light_go(); // Stop signal for 10 seconds
      clear_display();
      set_display_go(text);
      sleep_ms(10000);

    } else {                       // When the button is not pressed...
      set_traffic_light_caution(); // Caution signal for 2 seconds
      clear_display();
      set_display_caution(text);
      sleep_ms(2000);

      set_traffic_light_go(); // Stop signal for 8 seconds
      clear_display();
      set_display_go(text);
      sleep_ms(8000);
    }
  }

  return 0;
}
