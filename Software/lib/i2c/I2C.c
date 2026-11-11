#include "I2C.h"

// Wartet, bis die TWI-Hardware die laufende Operation abgeschlossen hat
// (TWINT-Flag wird von der Hardware gesetzt).
#define TWI_WAIT() while (!(TWCR & (1 << TWINT)))

void i2c_init(void) {
  // SCL-Frequenz = F_CPU / (16 + 2 * TWBR * Prescaler)
  // Bei 8MHz ergibt TWBR=32 und Prescaler=1 → 100kHz (Standard-Modus)
  TWBR = 32;
  TWSR = 0x00; // Prescaler-Bits = 1
}

uint8_t i2c_start(uint8_t address) {
  // START-Bedingung erzeugen
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  TWI_WAIT();

  // Statuscode prüfen: 0x08 = START, 0x10 = wiederholter START gesendet
  uint8_t status = TWSR & 0xF8;
  if (status != 0x08 && status != 0x10)
    return 1; // Fehler

  // Adressbyte (inkl. R/W-Bit) senden
  TWDR = address;
  TWCR = (1 << TWINT) | (1 << TWEN);
  TWI_WAIT();

  // Statuscode 0x18 = SLA+W wurde bestätigt (ACK empfangen)
  status = TWSR & 0xF8;
  if (status != 0x18)
    return 1; // Fehler — Gerät antwortet nicht

  return 0; // erfolgreich
}

void i2c_write(uint8_t data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  TWI_WAIT();
  // Statuscode 0x28 = Byte gesendet, ACK empfangen — könnte hier geprüft werden
}

void i2c_stop(void) {
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  // Kein Warten nötig — das TWSTO-Bit wird von der Hardware selbst gelöscht
}
