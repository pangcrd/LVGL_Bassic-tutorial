
#include <arduino.h>
#include "PCF8575.h"

// Set i2c address
PCF8575 pcf8575(0x20);
#define Numled 5
int LED_GPIO [Numled] = {P0, P1, P2, P3, P4 };

void setup()
{
	Serial.begin(115200);

	// Set pinMode to OUTPUT
  for (int i = 0; i < Numled; i++)
  {
    pcf8575.pinMode(i, OUTPUT);
  }
  
	pcf8575.begin();
}

void loop()
{
  while (true) {
    for (int i = 0; i < 5; i++) {
        pcf8575.digitalWrite(LED_GPIO[i], HIGH);
        delay(1000);
        pcf8575.digitalWrite(LED_GPIO[i], LOW);
    }
}

}