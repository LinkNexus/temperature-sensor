#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

// Merkt sich den Rückgabewert von i2c_start() für das OLED-Modul,
// damit bei einem I2C-Fehler eine Neuinitialisierung ausgelöst werden kann.
extern unsigned char i2c_fehler;

#endif // GLOBAL_H_INCLUDED
