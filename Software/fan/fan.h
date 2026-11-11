// Ansteuerung des Lüfters per PWM (Timer2) sowie Umrechnung
// zwischen Temperatur, PWM-Tastgrad und Prozentanzeige.

#ifndef FAN_H
#define FAN_H

#include <avr/io.h>
#include <stdint.h>

// Konfiguriert Timer2 als PWM-Ausgang für den Lüfter (0% Duty am Anfang)
void fan_init(void);
// Setzt den PWM-Tastgrad direkt als Registerwert (0-255)
void fan_set(uint8_t duty);
// Bildet eine Temperatur (°C) auf einen passenden PWM-Tastgrad ab
uint8_t temp_to_duty(uint8_t temp);
// Rechnet einen PWM-Tastgrad in eine Prozentanzeige (0-100%) um
uint8_t duty_to_percent(uint8_t duty);

#endif
