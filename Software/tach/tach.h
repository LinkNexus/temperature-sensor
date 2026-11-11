// Drehzahlmessung des Lüfters über den Tachosignal-Eingang (INT0).
// Zählt Impulse per Interrupt, die Umrechnung in RPM erfolgt in tach.c.

#ifndef TACH_H
#define TACH_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

// Konfiguriert INT0 als Eingang mit Pull-up und aktiviert den Interrupt
void tach_init(void);
// Setzt den Impulszähler zurück (vor Beginn eines neuen Messfensters)
void tach_reset(void);
// Liest den Impulszähler aus, setzt ihn zurück und liefert die RPM
uint16_t tach_get_rpm(void);

#endif
