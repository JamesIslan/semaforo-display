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

char *TRAFFIC_LIGHT_TEXT_GO[] = { // OLED text when green light is on
  "               ",
  "               ",
  " SINAL ABERTO  ",
  "               ",
  " ATRAVESSE COM ",
  "    CUIDADO    ",
  "               ",
  "               "};

char *TRAFFIC_LIGHT_TEXT_CAUTION[] = { // OLED text when yellow light is on
  "               ",
  "               ",
  "   SINAL DE    ",
  "    ATENCAO    ",
  "               ",
  "  PREPARE SE   ",
  "               ",
  "               "};

char *TRAFFIC_LIGHT_TEXT_STOP[] = { // OLED text when red light is on
  "               ",
  "               ",
  " SINAL FECHADO ",
  "               ",
  "    AGUARDE    ",
  "               ",
  "               ",
  "               "};

int A_state = 0; // Button A is set to OFF by default

void display_text(char *text[], uint8_t *ssd, struct render_area *frame_area, size_t line_count) { // Display text on OLED
  memset(ssd, 0, ssd1306_buffer_length);
  int y_axis = 0;

  for (size_t index = 0; index < line_count; index++) { // Draw the text in the OLED, line per line
    ssd1306_draw_string(ssd, 5, y_axis, text[index]);
    y_axis += 8;
  }

  render_on_display(ssd, frame_area);
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

void manage_traffic_light_and_display(char color, uint8_t *ssd, struct render_area *frame_area) {
  switch (color) {
  case 'R':
    break;
  case 'G':
    break;
  case 'B':
    break;
  }
}

int wait_for_button_pressing(int timeMS) { // Check constantly if BTN_A is pressed
  for (int i = 0; i < timeMS; i++) {
    A_state = !gpio_get(BTN_A_PIN);
    if (A_state == 1) {
      return 1;
    }
    sleep_ms(1);
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
  i2c_init(i2c1, ssd1306_i2c_clock * 400);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  ssd1306_init();
}

int main() {
  run_peripherals_setup();
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);
  uint8_t ssd[ssd1306_buffer_length];

  while (true) {
    set_traffic_light_stop(); // Stop signal for 8 seconds
    display_text(TRAFFIC_LIGHT_TEXT_STOP, ssd, &frame_area, count_of(TRAFFIC_LIGHT_TEXT_STOP));

    A_state = wait_for_button_pressing(8000); // Waits for the button to be pressed

    if (A_state) {                 // When the button is pressed...
      set_traffic_light_caution(); // Caution signal for 5 seconds
      display_text(TRAFFIC_LIGHT_TEXT_CAUTION, ssd, &frame_area, count_of(TRAFFIC_LIGHT_TEXT_CAUTION));
      sleep_ms(5000);

      set_traffic_light_go(); // Go signal for 10 seconds
      display_text(TRAFFIC_LIGHT_TEXT_GO, ssd, &frame_area, count_of(TRAFFIC_LIGHT_TEXT_GO));
      sleep_ms(10000);

    } else {                       // When the button is not pressed...
      set_traffic_light_caution(); // Caution signal for 2 seconds
      display_text(TRAFFIC_LIGHT_TEXT_CAUTION, ssd, &frame_area, count_of(TRAFFIC_LIGHT_TEXT_CAUTION));
      sleep_ms(2000);

      set_traffic_light_go(); // Go signal for 8 seconds
      display_text(TRAFFIC_LIGHT_TEXT_GO, ssd, &frame_area, count_of(TRAFFIC_LIGHT_TEXT_GO));
      sleep_ms(8000);
    }
  }

  return 0;
}
