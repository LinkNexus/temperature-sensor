// Treiber für den DHT11-Temperatur-/Feuchtesensor über das
// proprietäre 1-Draht-Timing-Protokoll (kein echtes I2C/SPI).

#ifndef DHT11_H
#define DHT11_H

#include <avr/io.h>
#include <stdint.h>

// Pin, an dem der DHT11-Datenanschluss hängt
#define DHT_PIN PD4
#define DHT_DDR DDRD
#define DHT_PORT PORTD
#define DHT_IN PIND

// Liest Temperatur und Feuchte aus. Rückgabe 0 = ok, sonst Fehlercode
// (siehe Kommentare in dht11.c für die Bedeutung der einzelnen Codes).
uint8_t dht11_read(uint8_t *temp, uint8_t *hum);

#endif
