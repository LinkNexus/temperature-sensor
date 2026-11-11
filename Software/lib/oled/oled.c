
/*
 * oled.c
 *
 *  Created on: 31.10.2017
 *      Author: dennis
 */

#include <avr/interrupt.h>

#include "../i2c/I2C.h"
#include "font.h"
#include "global.h"
#include "oled.h"

// Aktuelle Cursor-Position (in Zeichen-Spalten/Seiten) und Schriftgröße
static uint8_t OLED_x = 0, OLED_y = 0, font_size = 0;

// Initialisierungssequenz für den SSD1306-Controller (wird beim Start
// einmal komplett über I2C an das Display gesendet)
const uint8_t ssd1306_oled_init[] PROGMEM = {
    Set_Display_Off, Set_NOP, Set_Lower_Column, Set_Higher_Column,
    Set_Memory_Addressing_Mode, Set_Horizontal_Adressing_Mode, Set_Page_Adress,
    Set_Multiplex_Ratio, Set_Multiplex_To, Set_Display_Offset, Set_No_Offset,
    Set_Display_Start_Line, Set_Segment_Remap_02,
    Set_COM_Output_Remapped, // Vllt remapped machen
    Set_COM_Pins, Set_COM_Pins_Hardware, Set_VCOMH_Deselect, Set_VCOMH,
    Set_Contrast_Control, Set_Contrast_1, Entire_Display_On, Set_Normal_Display,
    Set_Display_Clock, Set_Display_Clock_01,
    // Set_Precharge_Period, Set_Precharge_Period_To,
    Set_Charge_Bumb_Setting, Set_Charge_Bumb_Enable, Set_Display_On};

void oled_init(void) {
  // Anmerkung zum wiederkehrenden Muster in dieser Datei: Jede Funktion
  // startet den I2C-Transfer selbst und versucht bei einem Fehler
  // (i2c_fehler == 1, z.B. Display antwortet nicht) eine Neuinitialisierung
  // und einen Reset der Cursor-Position, bevor sie es erneut versucht.
  i2c_fehler = i2c_start(OLED_ssd1306);
  if (i2c_fehler == 1) {
    i2c_stop();
    oled_init;
    OLED_gotoxy(0, 0);
    clear_oled();
  } else {
    i2c_write(command); // es folgen Kommando-Bytes, keine Pixeldaten
    for (uint8_t tmp = 0; tmp < sizeof(ssd1306_oled_init); tmp++) {
      i2c_write(pgm_read_byte(&ssd1306_oled_init[tmp]));
    }
    i2c_stop();
    clear_oled();
  }
}

void clear_oled(void) {
  i2c_fehler = i2c_start(OLED_ssd1306);
  if (i2c_fehler == 1) {
    i2c_stop();
    oled_init;
    OLED_gotoxy(0, 0);
    clear_oled();
  } else {
    i2c_write(data); // es folgen Pixeldaten
    // gesamten Bildspeicher (128 Spalten * 8 Seiten) mit 0 überschreiben
    for (uint16_t i = 0; i < 128 << 4; i++) { // 128 * (64 / Byte)
      i2c_write(0);
    }
    i2c_stop();
    OLED_gotoxy(0, 0);
  }
}

void OLED_gotoxy(uint8_t x, uint8_t y) {
  OLED_x = x;
  OLED_y = y;
  i2c_fehler = i2c_start(OLED_ssd1306);
  if (i2c_fehler == 1) {
    i2c_stop();
    oled_init;
    OLED_gotoxy(0, 0);
    clear_oled();
  } else {
    i2c_write(command);
    i2c_write(Set_Page_Start | y);      // Seite (Zeile) auswählen
    i2c_write(Set_Column_Adress);       // Spaltenbereich festlegen ...
    i2c_write(x * 8);                   // ... beginnend bei Spalte x*8
    i2c_write(Set_Display_Start_Line | 0x3F);
    i2c_stop();
  }
}

void bildschirm_reinigen(void) {
  OLED_gotoxy(0, 0);
  clear_oled();
}

void oled_font_size(uint8_t byte) { font_size = byte; }

// Vier vordefinierte Layouts: Überschrift/Text jeweils in normaler (2)
// oder doppelter Größe (1), mit passender Startposition.
void ueberschrift1(void) {
  OLED_gotoxy(0, 0);
  oled_font_size(1);
}

void ueberschrift2(void) {
  OLED_gotoxy(0, 0);
  oled_font_size(0);
}

void text1(void) {
  OLED_gotoxy(0, 2);
  oled_font_size(1);
}

void text2(void) {
  OLED_gotoxy(0, 2);
  oled_font_size(0);
}

void oled_write_char(char c) {
  if (font_size != 0) {
    // Doppelt große Darstellung: das 8x8-Font-Bitmap wird auf 16x16 Pixel
    // gestreckt, indem jedes Bit auf 2x2 Pixel verdoppelt wird. Dazu wird
    // jede Spalte des Zeichens in zwei Bildschirmseiten (obere/untere
    // Hälfte) mit je 8 Byte geschrieben.
    uint8_t tmp = 0;
    OLED_gotoxy(OLED_x, OLED_y);
    i2c_fehler = i2c_start(OLED_ssd1306);
    if (i2c_fehler == 1) {
      i2c_stop();
      oled_init;
      OLED_gotoxy(0, 0);
      clear_oled();
    } else {
      i2c_write(data);
      // Obere Hälfte: unteres Nibble jeder Spalte auf 4 Doppel-Pixel verteilen
      for (uint8_t count = 0; count < 8; count++) {
        tmp = pgm_read_byte(&font[(unsigned char)c][count]);
        tmp = tmp & 0x0f;
        // jedes der 4 Bits wird auf die passende Ziel-Bitposition gespreizt
        // und per *3 verdoppelt (Bitmuster 0b11 statt 0b1)
        tmp = ((tmp & 0x01) * 3) + (((tmp & 0x02) << 1) * 3) +
              (((tmp & 0x04) << 2) * 3) + (((tmp & 0x08) << 3) * 3);
        i2c_write(tmp); // Spalte horizontal verdoppeln: zweimal senden
        i2c_write(tmp);
      }
    }
    i2c_stop();
    OLED_gotoxy(OLED_x, OLED_y + 1);
    i2c_fehler = i2c_start(OLED_ssd1306);
    if (i2c_fehler == 1) {
      i2c_stop();
      oled_init;
      OLED_gotoxy(0, 0);
      clear_oled();
    } else {
      i2c_write(data);
      // Untere Hälfte: oberes Nibble jeder Spalte genauso spreizen
      for (uint8_t count = 0; count < 8; count++) {
        tmp = pgm_read_byte(&font[(unsigned char)c][count]);
        tmp = (tmp & 0xf0) >> 4;
        tmp = ((tmp & 0x01) * 3) + (((tmp & 0x02) << 1) * 3) +
              (((tmp & 0x04) << 2) * 3) + (((tmp & 0x08) << 3) * 3);
        i2c_write(tmp);
        i2c_write(tmp);
      }
      i2c_stop();
      // Cursor hinter das (doppelt breite) Zeichen setzen, zurück auf die
      // ursprüngliche Zeile (Y wurde für die untere Hälfte nur temporär erhöht)
      OLED_x += 2;
      OLED_y -= 1;
    }
  } else {
    // Normale Größe: Font-Bitmap unverändert byteweise ausgeben
    OLED_gotoxy(OLED_x, OLED_y);
    i2c_fehler = i2c_start(OLED_ssd1306);
    if (i2c_fehler == 1) {
      i2c_stop();
      oled_init;
      OLED_gotoxy(0, 0);
      clear_oled();
    } else {
      i2c_write(data);
      for (uint8_t count = 0; count < 8; count++) {
        i2c_write(pgm_read_byte(&font[(unsigned char)c][count]));
      }
      i2c_stop();
      OLED_x += 1;
    }
  }
}

//***************************************************************************************
// Gibt eine im RAM liegende, nullterminierte Zeichenkette Zeichen für
// Zeichen auf dem Display aus.
void oled_write_str(char *str) {
  while (*str) {
    oled_write_char(*str++);
  }
}

//***************************************************************************************
// Minimaler printf-Ersatz für Formatstrings im Flash-Speicher (PROGMEM),
// aufgerufen über das oled_write(...)-Makro. Unterstützt %s, %c sowie
// Zahlen als %b (binär), %i (dezimal), %o (oktal), %x (hex) und eine
// einstellige Breitenangabe zum Auffüllen mit führenden Nullen (z.B. %2i).
void oled_write_P(const char *Buffer, ...) {
  va_list ap;
  va_start(ap, Buffer);

  int format_flag;
  char str_buffer[10];      // Zielpuffer für die Zahl-zu-String-Umwandlung
  char str_null_buffer[10]; // Hilfspuffer, u.a. für führende Nullen
  char move = 0;            // gewünschte Mindestbreite (aus z.B. "%3i")
  char Base = 0;            // Zahlenbasis für itoa (2/8/10/16)
  int tmp = 0;
  char by;
  char *ptr;

  // Format-String Byte für Byte aus dem Flash lesen und interpretieren
  for (;;) {
    by = pgm_read_byte(Buffer++);
    if (by == 0)
      break; // Ende des Format-Strings

    if (by == '%') {
      by = pgm_read_byte(Buffer++);
      if (isdigit(by) > 0) {
        // einstellige Breitenangabe (z.B. die "2" in "%2i") einlesen
        str_null_buffer[0] = by;
        str_null_buffer[1] = '\0';
        move = atoi(str_null_buffer);
        by = pgm_read_byte(Buffer++);
      }

      switch (by) {
      case 's':
        ptr = va_arg(ap, char *);
        while (*ptr) {
          oled_write_char(*ptr++);
        }
        break;
      case 'b':
        Base = 2;
        goto ConversionLoop;
      case 'c':
        // Int-Argument als einzelnes Zeichen ausgeben
        format_flag = va_arg(ap, int);
        oled_write_char(format_flag++);
        break;
      case 'i':
        Base = 10;
        goto ConversionLoop;
      case 'o':
        Base = 8;
        goto ConversionLoop;
      case 'x':
        Base = 16;
      //****************************
      ConversionLoop:
        // Zahl in der gewählten Basis in str_buffer wandeln
        //****************************
        itoa(va_arg(ap, int), str_buffer, Base);
        int b = 0;
        while (str_buffer[b++] != 0) {
        };
        b--;
        // falls kürzer als die geforderte Breite: mit führenden Nullen auffüllen
        if (b < move) {
          move -= b;
          for (tmp = 0; tmp < move; tmp++) {
            str_null_buffer[tmp] = '0';
          }
          // tmp ++;
          str_null_buffer[tmp] = '\0';
          strcat(str_null_buffer, str_buffer);
          strcpy(str_buffer, str_null_buffer);
        }
        oled_write_str(str_buffer);
        move = 0;
        break;
      }
    } else {
      // normales Zeichen ohne '%' direkt ausgeben
      oled_write_char(by);
    }
  }
  va_end(ap);
}
