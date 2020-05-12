#include "Arduino.h"
#include "eeprom.h"
#include "cmdline.h"

/*
The setup function is called once at startup of the sketch
                  +-\/-+
      (RES_)PC6  1|    |28  PC5 (AI 5) (D 19)
      (D 0) PD0  2|    |27  PC4 (AI 4) (D 18)
      (D 1) PD1  3|    |26  PC3 (AI 3) (D 17)
  INT0(D 2) PD2  4|    |25  PC2 (AI 2) (D 16)
  INT1(D 3) PD3  5|    |24  PC1 (AI 1) (D 15)
      (D 4) PD4  6|    |23  PC0 (AI 0) (D 14)
            VCC  7|    |22  GND
            GND  8|    |21  AREF
            PB6  9|    |20  AVCC
            PB7 10|    |19  PB5 (D 13)    | SCK  -+
 PWM+ (D 5) PD5 11|    |18  PB4 (D 12)    | MISO  |=> SPI
 PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM| MOSI -+
      (D 7) PD7 13|    |16  PB2 (D 10) PWM
      (D 8) PB0 14|    |15  PB1 (D 9)  PWM
                  +----+
*/

void
setup()
{
	Serial.begin(115200);
	Serial.print("95160 EEPROM reader writer 1.0\n");
}

// The loop function is called in an endless loop
void loop()
{
	Serial.print(">");

	read_line();
	if(!get_error()){
		parse_line();
	}
	if(!get_error()){
		execute();
	}

}
