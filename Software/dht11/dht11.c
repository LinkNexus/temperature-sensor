#include "dht11.h"
#include <util/delay.h>

uint8_t dht11_read(uint8_t *temp, uint8_t *hum) {
  uint8_t bits[5] = {0}; // Feuchte, (Nachkomma unbenutzt), Temp, (Nachkomma unbenutzt), Checksumme

  // Start-Signal: Leitung als Ausgang auf LOW ziehen (mind. 18ms laut Datenblatt)
  DHT_DDR |= (1 << DHT_PIN);
  DHT_PORT &= ~(1 << DHT_PIN);
  _delay_ms(25);
  // Leitung wieder freigeben und auf Eingang mit Pull-up umschalten,
  // der Sensor übernimmt danach den Bus
  DHT_PORT |= (1 << DHT_PIN);
  _delay_us(30);
  DHT_DDR &= ~(1 << DHT_PIN);
  DHT_PORT |= (1 << DHT_PIN);

  // Sensor antwortet mit LOW — bleibt die Leitung HIGH, antwortet nichts
  _delay_us(40);
  if (DHT_IN & (1 << DHT_PIN))
    return 2;

  uint8_t timeout;

  // Warten auf Ende des Antwort-LOW-Pulses
  timeout = 255;
  while (!(DHT_IN & (1 << DHT_PIN)))
    if (--timeout == 0)
      return 3;

  // Warten auf Ende des Antwort-HIGH-Pulses, danach beginnen die Datenbits
  timeout = 255;
  while (DHT_IN & (1 << DHT_PIN))
    if (--timeout == 0)
      return 5;

  // 40 Datenbits einlesen: jedes Bit beginnt mit einem kurzen LOW,
  // die Länge des folgenden HIGH-Pulses codiert 0 oder 1
  for (uint8_t i = 0; i < 40; i++) {
    timeout = 255;
    while (!(DHT_IN & (1 << DHT_PIN)))
      if (--timeout == 0)
        return 6;
    // Nach 30us ist ein "0"-Bit bereits wieder LOW, ein "1"-Bit noch HIGH
    _delay_us(30);
    if (DHT_IN & (1 << DHT_PIN))
      bits[i / 8] |= (1 << (7 - (i % 8)));
    timeout = 255;
    while (DHT_IN & (1 << DHT_PIN))
      if (--timeout == 0)
        return 7;
  }

  // Prüfsumme: Byte 4 muss der Summe der Bytes 0-3 entsprechen
  if (bits[4] != ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF))
    return 4;

  *hum = bits[0];
  *temp = bits[2];
  return 0;
}
