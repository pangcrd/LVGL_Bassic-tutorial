#include "Arduino.h"
#include <FT6336.h>
#include <Wire.h>

FT6336::FT6336(uint8_t _sda, uint8_t _scl, uint8_t _int, uint8_t _rst, uint16_t _width, uint16_t _height) :
  pinSda(_sda), pinScl(_scl), pinInt(_int), pinRst(_rst), width(_width), height(_height) {

}

void FT6336::begin(uint8_t _addr) {
  addr = _addr;
  Wire.begin(pinSda, pinScl);
  if(reset())
  {
    Serial.print("touch ic error!\r\n");
  }
}

uint8_t FT6336::reset() {
  uint8_t tmp[2];
  pinMode(pinInt, INPUT);
  pinMode(pinRst, OUTPUT);
  digitalWrite(pinInt, 1);
  digitalWrite(pinRst, 1);
  delay(20);
  digitalWrite(pinRst, 0);
  delay(20);
  digitalWrite(pinRst, 1);
  delay(500);
  readBlockData(&tmp[0], FT6336_ID_G_FOCALTECH_ID, 1);
  if(tmp[0] != 0x11)
  {
    return 1;
  }
  readBlockData(tmp, FT6336_ID_G_CIPHER_MID, 2);
  if(tmp[0] != 0x26)
  {
    return 1;
  }
  if((tmp[1]!=0x00)&&(tmp[1]!=0x01)&&(tmp[1]!=0x02))
  {
    return 1;
  }
  readBlockData(&tmp[0], FT6336_ID_G_CIPHER_HIGH, 1);
  if(tmp[0] != 0x64)
  {
    return 1;
  }
  return 0;
}

void FT6336::setRotation(uint8_t rot) {
  rotation = rot;
}

void FT6336::read(void) {
  uint8_t data[4];
  uint8_t pointInfo = readByteData(FT6336_TD_STATUS);
  touches = pointInfo;
  isTouched = (touches > 0 && touches < 3);
  if (isTouched) {
    for (uint8_t i=0; i<touches; i++) {
      readBlockData(data, FT6336_TOUCH_1 + i * 6, 4);
      points[i] = readPoint(data);
    }
  }
}
TP_Point FT6336::readPoint(uint8_t *data) {
  uint16_t temp;
  uint8_t id = data[2] >> 4;
  uint16_t x = (uint16_t)((data[0]&0x0F)<<8) + data[1];
  uint16_t y = (uint16_t)((data[2]&0x0F)<<8) + data[3];
  switch (rotation){
    case ROTATION_NORMAL:
      x = x;
      y = y;
      break;
    case ROTATION_LEFT:
      temp = x;
      x = height - y;
      y = temp;
      break;
    case ROTATION_INVERTED:
      x = width - x;
      y = height - y;
      break;
    case ROTATION_RIGHT:
      temp = x;
      x = y;
      y = width - temp;
      break;
    default:
      break;
  }
  return TP_Point(id, x, y, 0);
}
void FT6336::writeByteData(uint16_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
 // Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  Wire.write(val);
  Wire.endTransmission();
}
uint8_t FT6336::readByteData(uint16_t reg) {
  uint8_t x;
  Wire.beginTransmission(addr);
 // Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);
  x = Wire.read();
  return x;
}
void FT6336::writeBlockData(uint16_t reg, uint8_t *val, uint8_t size) {
  Wire.beginTransmission(addr);
//  Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  // Wire.write(val, size);
  for (uint8_t i=0; i<size; i++) {
    Wire.write(val[i]);
  }
  Wire.endTransmission();
}
void FT6336::readBlockData(uint8_t *buf, uint16_t reg, uint8_t size) {
  Wire.beginTransmission(addr);
//  Wire.write(highByte(reg));
  Wire.write(lowByte(reg));
  Wire.endTransmission();
  Wire.requestFrom(addr, size);
  for (uint8_t i=0; i<size; i++) {
    buf[i] = Wire.read();
  }
}
TP_Point::TP_Point(void) {
  id = x = y = size = 0;
}
TP_Point::TP_Point(uint8_t _id, uint16_t _x, uint16_t _y, uint16_t _size) {
  id = _id;
  x = _x;
  y = _y;
  size = _size;
}
bool TP_Point::operator==(TP_Point point) {
  return ((point.x == x) && (point.y == y) && (point.size == size));
}
bool TP_Point::operator!=(TP_Point point) {
  return ((point.x != x) || (point.y != y) || (point.size != size));
}
