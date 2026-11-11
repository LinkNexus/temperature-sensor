// Einfacher I2C(TWI)-Treiber für ATmega: Init sowie Start/Write/Stop
// als Bausteine für Master-Sende-Transfers (z.B. zum OLED-Display).

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>

// Initialisiert die TWI-Hardware (Bitrate/Prescaler).
void i2c_init(void);
// Sendet START-Bedingung + Adressbyte. Rückgabe 0 = ok, 1 = Fehler (kein ACK).
uint8_t i2c_start(uint8_t address);
// Sendet ein Datenbyte an das zuvor adressierte Gerät.
void i2c_write(uint8_t data);
// Sendet STOP-Bedingung und beendet den Transfer.
void i2c_stop(void);

#endif
