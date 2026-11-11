#include <avr/io.h>
#include <util/delay.h>

#include "dht11/dht11.h"
#include "fan/fan.h"
#include "lib/i2c/I2C.h"
#include "lib/oled/global.h"
#include "lib/oled/oled.h"
#include "tach/tach.h"

int main(void) {
  i2c_init();
  oled_init();
  fan_init();
  tach_init();

  _delay_ms(2000);

  uint8_t temp = 0, hum = 0;

  while (1) {
    cli();
    tach_reset();
    uint8_t result = dht11_read(&temp, &hum);
    sei();

    bildschirm_reinigen();

    if (result == 0) {
      uint8_t duty = temp_to_duty(temp);
      fan_set(duty);
      oled_write("Temp: %i C", temp);
      OLED_gotoxy(0, 2);
      oled_write("Hum:  %i %%", hum);
      OLED_gotoxy(0, 4);
      oled_write("Fan:  %i%%", duty_to_percent(duty));
      OLED_gotoxy(0, 6);
      oled_write("RPM:  ...");
    } else {
      fan_set(0);
      oled_write("Fehler: %i", result);
    }

    _delay_ms(2000);

    uint16_t rpm = tach_get_rpm();
    OLED_gotoxy(0, 6);
    oled_write("RPM: %i  ", rpm);
  }
}
