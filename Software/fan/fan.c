#include "fan.h"

// Der Lüfter läuft unterhalb dieses Tastgrads praktisch nicht an,
// daher wird der nutzbare Bereich auf [FAN_MIN_DUTY, FAN_MAX_DUTY] begrenzt.
#define FAN_MIN_DUTY 240
#define FAN_MAX_DUTY 255

void fan_init(void) {
  DDRD |= (1 << PD3); // PD3 = OC2B als PWM-Ausgang
  // Fast-PWM-Modus, nicht-invertierend auf OC2B (Timer2)
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << CS22); // Prescaler 64
  OCR2B = 0;            // Lüfter startet ausgeschaltet
}

void fan_set(uint8_t duty) { OCR2B = duty; }

// Stufenweise Zuordnung: je wärmer, desto höher der Tastgrad
uint8_t temp_to_duty(uint8_t temp) {
  if (temp < 15)
    return 0;
  if (temp < 20)
    return 240;
  if (temp < 25)
    return 245;
  if (temp < 30)
    return 250;
  return 255;
}

// Skaliert den nutzbaren Duty-Bereich linear auf 0-100% für die Anzeige
uint8_t duty_to_percent(uint8_t duty) {
  if (duty == 0)
    return 0;
  return ((duty - FAN_MIN_DUTY) * 100) / (FAN_MAX_DUTY - FAN_MIN_DUTY);
}
