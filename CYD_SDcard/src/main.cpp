/** 
////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// Example CYD micro SD card FUNCTION: Read jpg/bitmap images, Create/read/delete .txt files  // 
// Design UI on Squareline Studio. LVGL V9.1                                                  //
// Youtube:https://www.youtube.com/@pangcrd                                                   //
// Github: https://github.com/pangcrd                                                         //
// Example for JPG library:                                                                   //
// https://github.com/Bodmer/TJpg_Decoder                                                     //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////*/

#include "SDCard.h"
// Include the jpeg decoder library
#include <TJpg_Decoder.h>


TFT_eSPI tft = TFT_eSPI();

unsigned long startTime = 0;
int BitmapImageCount = 0;  

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;
  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);
  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);
  // Return 1 to decode next block
  return 1;
}

/** Display more bitmap image from SD card */
void ShowBitmapImage(){
  const char* imageFiles[] = { /** Add more Image here */

    "/firefly.bmp", "/cat.bmp", "/priscilla.bmp", "/moon.bmp", "/tree.bmp"
  };
  const int totalImages = sizeof(imageFiles) / sizeof(imageFiles[0]); // Ex: int= 4 byte*imageFiles = 20 byte
    unsigned long ImageTime = millis();
    if (ImageTime - startTime >= 5000) { // Delay time per picture
            startTime = ImageTime; 

            drawBmp(SD, imageFiles[BitmapImageCount], 0, 0);
            BitmapImageCount++;// Next picture
            if (BitmapImageCount >= totalImages) {
              BitmapImageCount = 0; 
            }
        }
}

/** Display JPG image from SD card*/
void ShowJPGimage(){
    // Time recorded for test purposes
    uint32_t t = millis();

    // Get the width and height in pixels of the jpeg if you wish
    uint16_t w = 0, h = 0;
    TJpgDec.getSdJpgSize(&w, &h, "/priscilla.jpg");
    Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

    // Draw the image, top left at 0,0
    TJpgDec.drawSdJpg(0, 0, "/priscilla.jpg");

    // How much time did rendering take
    t = millis() - t;
    Serial.print(t); Serial.println(" ms");
}


/** Screen brightness */
void TFT_SET_BL(uint8_t Value) {
  if (Value < 0 || Value > 100) {
    printf("TFT_SET_BL Error \r\n");
  } else {
    analogWrite(TFT_BL, Value * 2.55);
  }
}

void setup() {
    Serial.begin(115200);

    tft.init();
    tft.setRotation(2);
    //tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)
    TFT_SET_BL(20);

    if (!initSDCard()) {
      Serial.println("SD card failed to mount! Stopping...");
      while (1); 
  }

    // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);
    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);

    /** SD test function */
    // drawBmp(SD, "/firefly.bmp", 0, 0);
    // listDir(SD, "/", 0);
    // createDir(SD, "/mydir");
    // listDir(SD, "/", 0);
    // removeDir(SD, "/mydir");
    // listDir(SD, "/", 2);
    // writeFile(SD, "/hello.txt", "Hello ");
    // appendFile(SD, "/hello.txt", "World!\n");
    // readFile(SD, "/test.txt");
    // deleteFile(SD, "/foo.txt");
    // renameFile(SD, "/hello.txt", "/foo.txt");
    // readFile(SD, "/foo.txt");
    // testFileIO(SD, "/test.txt");
}

void loop() {
  //ShowBitmapImage();
  ShowJPGimage();
}
