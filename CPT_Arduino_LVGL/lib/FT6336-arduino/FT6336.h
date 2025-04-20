
#ifndef FT6336_H
#define FT6336_H

#include "Arduino.h"
#include <Wire.h>

#define FT6336_ADDR  (uint8_t)0x38

#define ROTATION_LEFT       (uint8_t)3
#define ROTATION_INVERTED   (uint8_t)2
#define ROTATION_RIGHT      (uint8_t)1
#define ROTATION_NORMAL     (uint8_t)0

#define FT6336_DEVIDE_MODE  (uint8_t)0x00
#define FT6336_TD_STATUS    (uint8_t)0x02

#define FT6336_TOUCH_1      (uint8_t)0X03
#define FT6336_TOUCH_2      (uint8_t)0X09

#define FT6336_TOUCH_REG        {FT6336_TOUCH_1, FT6336_TOUCH_2}

#define FT6336_ID_G_CIPHER_MID    (uint8_t)0x9F    //default: 0x26
#define FT6336_ID_G_CIPHER_LOW    (uint8_t)0XA0    //0x01:FT6336G, 0x02:FT6336U
#define FT6336_ID_G_LIB_VERSION   (uint8_t)0xA1
#define FT6336_ID_G_CIPHER_HIGH     (uint8_t)0XA3  //default: 0x64
#define FT6336_ID_G_MODE            (uint8_t)0XA4
#define FT6336_ID_G_FOCALTECH_ID    (uint8_t)0XA8  //default: 0x11
#define FT6336_ID_G_THGROUP         (uint8_t)0X80
#define FT6336_ID_G_PERIODACTIVE    (uint8_t)0X88


class TP_Point {
  public:
    TP_Point(void);
    TP_Point(uint8_t id, uint16_t x, uint16_t y, uint16_t size);

    bool operator==(TP_Point);
    bool operator!=(TP_Point);

    uint8_t id;
    uint16_t x;
    uint16_t y;
    uint8_t size;
};

class FT6336 {
  public:
    FT6336(uint8_t _sda, uint8_t _scl, uint8_t _int, uint8_t _rst, uint16_t _width, uint16_t _height);
    void begin(uint8_t _addr=FT6336_ADDR);
    uint8_t reset();
    void setRotation(uint8_t rot);
    void read(void);
    uint8_t touches = 0;
    bool isTouched = false;
    TP_Point points[2];

  private:
    // static void ARDUINO_ISR_ATTR onInterrupt();
    TP_Point readPoint(uint8_t *data);
    void writeByteData(uint16_t reg, uint8_t val);
    uint8_t readByteData(uint16_t reg);
    void writeBlockData(uint16_t reg, uint8_t *val, uint8_t size);
    void readBlockData(uint8_t *buf, uint16_t reg, uint8_t size);
    uint8_t rotation = ROTATION_NORMAL;
    uint8_t addr;
    uint8_t pinSda;
    uint8_t pinScl;
    uint8_t pinInt;
    uint8_t pinRst;
    uint16_t width;
    uint16_t height;
};

#endif 
