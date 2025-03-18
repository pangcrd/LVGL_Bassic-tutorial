/*******************************************************************
    An example of using the built-in SD card slot of the CYD.

    The example will test reading and writing files to the SD card.
    It will print its output to the serial monitor

    Based on the basic SD_Test example in the ESP32 library.

    https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/

    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Twitter: https://twitter.com/witnessmenow

	Make some changes to the original code to make it work with bitmap images
	By Pangcrd
	Youtube: https://www.youtube.com/@pangcrd
	GitHub: https://github.com/pangcrd

	BMP rendering logic based on multiple open-source implementations.

 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#ifndef SDCARD_H
#define SDCARD_H

#include "Arduino.h"
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif
#include "SD.h"
#include "SPI.h"
#include "TFT_eSPI.h"
//#include <JPEGDecoder.h>  // JPEG decoder library
#include <TJpg_Decoder.h>


extern TFT_eSPI tft;
// SD Card initialization
bool initSDCard();

// File management functions
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);

// BMP file reading functions
uint16_t read16(File &f);
uint32_t read32(File &f);

void drawBmp(fs::FS &fs, const char *filename, int16_t x, int16_t y);





#endif // SDCARD_H
