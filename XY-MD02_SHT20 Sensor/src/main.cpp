#include "XY_MD02_Config.h"
#include <HardwareSerial.h>
// Create RS485 SoftwareSerial port
HardwareSerial rs485Serial(2);  // UART2 ESP32 16 RX 17 TX

void setup() {
  Serial.begin(9600);
  XYMD02::setupModbus (rs485Serial,1,16,17); // uart2, modbus ID = 1(in this case),to TX, to RX)
}

void loop() {
  XYMD02::UpdateValue(2000); //Update sensor value after 2s
}
