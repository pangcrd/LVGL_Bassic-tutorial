
#include "SDCard.h"


// this function determines the minimum of two numbers
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))


SPIClass SDspi = SPIClass(VSPI);


/**
 * @brief Initialize the SD card
 * @return true if successful, false otherwise
 */
bool initSDCard() {
    Serial.println("Initializing SD card...");

    if (!SD.begin(SS, SDspi, 20000000)) {
        Serial.println("Card Mount Failed");

        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 10);
        tft.println("TF card mount failed!");

        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }

    Serial.println("TF init success"); 
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 10);
    tft.println("TF card successfully mounted");

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) Serial.println("MMC");
    else if (cardType == CARD_SD) Serial.println("SDSC");
    else if (cardType == CARD_SDHC) Serial.println("SDHC");
    else Serial.println("UNKNOWN");

    Serial.printf("SD Card Size: %lluMB\n", SD.cardSize() / (1024 * 1024));
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    return true;
}

/**
 * @brief List all files in a directory
 */
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        Serial.print(file.isDirectory() ? "  DIR : " : "  FILE: ");
        Serial.print(file.name());
        if (!file.isDirectory()) {
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        if (levels && file.isDirectory()) listDir(fs, file.name(), levels - 1);
        file = root.openNextFile();
    }
}

/** File operation functions (create, remove, read, write, etc.) */
void createDir(fs::FS &fs, const char *path) { Serial.printf("Creating Dir: %s\n", path); fs.mkdir(path) ? Serial.println("Dir created") : Serial.println("mkdir failed"); }

void removeDir(fs::FS &fs, const char *path) { Serial.printf("Removing Dir: %s\n", path); fs.rmdir(path) ? Serial.println("Dir removed") : Serial.println("rmdir failed"); }

void readFile(fs::FS &fs, const char *path) { File file = fs.open(path); if (!file) { Serial.println("Failed to open file for reading"); return; } while (file.available()) Serial.write(file.read()); file.close(); }

void writeFile(fs::FS &fs, const char *path, const char *message) { File file = fs.open(path, FILE_WRITE); if (!file) { Serial.println("Failed to open file for writing"); return; } file.print(message) ? Serial.println("File written") : Serial.println("Write failed"); file.close(); }

void appendFile(fs::FS &fs, const char *path, const char *message) { File file = fs.open(path, FILE_APPEND); if (!file) { Serial.println("Failed to open file for appending"); return; } file.print(message) ? Serial.println("Message appended") : Serial.println("Append failed"); file.close(); }

void renameFile(fs::FS &fs, const char *path1, const char *path2) { fs.rename(path1, path2) ? Serial.printf("Renamed %s to %s\n", path1, path2) : Serial.println("Rename failed"); }

void deleteFile(fs::FS &fs, const char *path) { fs.remove(path) ? Serial.printf("Deleted %s\n", path) : Serial.println("Delete failed"); }

void testFileIO(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}


/**
 * @brief Read 16-bit value from SD file
 */
uint16_t read16(File &f) {
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read();
    ((uint8_t *)&result)[1] = f.read();
    return result;
}

/**
 * @brief Read 32-bit value from SD file
 */
uint32_t read32(File &f) {
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read();
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read();
    return result;
}

/**
 * @brief Draw BMP image from SD card onto TFT display
 */
void drawBmp(fs::FS & fs, const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  // Open requested file on SD card
  File bmpFS = fs.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16-bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      // Serial.print("Loaded in "); Serial.print(millis() - startTime);
      // Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

//====================================================================================
//   Print information about the image
//====================================================================================





