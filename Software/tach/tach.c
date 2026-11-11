#include "tach.h"

// Wird von der ISR und aus dem Hauptprogramm gelesen/geschrieben,
// daher volatile und für Mehrfachzugriffe mit cli()/sei() abgesichert.
volatile uint16_t tach_pulse_count = 0;

// Ein Interrupt pro fallender Flanke des Tachosignals
ISR(INT0_vect) { tach_pulse_count++; }

void tach_init(void) {
  DDRD &= ~(1 << PD2);   // PD2 als Eingang
  PORTD |= (1 << PD2);   // interner Pull-up
  EICRA |= (1 << ISC01); // Interrupt bei fallender Flanke auslösen
  EIMSK |= (1 << INT0);  // INT0 aktivieren
  sei();
}

void tach_reset(void) { tach_pulse_count = 0; }

uint16_t tach_get_rpm(void) {
  // Zähler atomar auslesen und zurücksetzen (Interrupt kurz sperren)
  cli();
  uint16_t count = tach_pulse_count;
  tach_pulse_count = 0;
  sei();
  // 2 Impulse pro Umdrehung, 2-Sekunden-Messfenster:
  // RPM = (count/2 Umdrehungen) * (60/2 pro Minute) = count * 15
  return count * 15;
}
