/** Open Weather get data 
 *  https://openweathermap.org/find
 *  https://github.com/pangcrd
 *  https://www.youtube.com/@pangcrd
*/
#include "OpenWeatherGet.h"
#include <TFT_eSPI.h> 

/** Change to your screen resolution */
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); 

static const unsigned char PROGMEM image_network_4_bars_bits[] = {0x00,0x0e,0x00,0x0e,0x00,0x0e,0x00,0x0e,
    0x00,0xee,0x00,0xee,0x00,0xee,0x00,0xee,0x0e,0xee,0x0e,0xee,0x0e,
    0xee,0x0e,0xee,0xee,0xee,0xee,0xee,0xee,0xee,0x00,0x00};

static const unsigned char PROGMEM image_location_bits[] = {0x0f,0x80,0x30,0x60,0x40,0x10,0x47,0x10,0x88,
    0x88,0x90,0x48,0x90,0x48,0x50,0x50,0x48,0x90,0x27,0x20,0x20,0x20,
    0x10,0x40,0x08,0x80,0x05,0x00,0x07,0x00,0x02,0x00};

/** Previous data status */
String previousTime = "";
String previousTemp = "";
String previousHumi = "";
String previousWind = "";
String previousPressure = "";
String previousWeather = "";
String previousStatus = "";

/** Delete old value every time you update new value to avoid overlapping or screen flickering */
void removeText(String current, int x, int y, uint16_t bg, uint16_t fg, String &lastText) {
  if (current != lastText) {
    tft.fillRect(x, y, tft.textWidth(lastText), 16, bg); 
    tft.setTextColor(fg, bg);
    tft.drawString(current, x, y);
    lastText = current;
  }
}

void drawConnectionStatus(bool isConnected, String& prevStatus) {
  String statusStr = isConnected ? "NTP: Synced!" : "NTP: Waiting";
  uint16_t textColor = isConnected ? 0x1FEA : 0xFD61; 
  uint16_t iconColor = isConnected ? 0x77E0 : 0xF800;

  if (prevStatus != statusStr) {
    tft.fillRect(3, 16, 150, 16, TFT_BLACK);
    tft.setTextColor(textColor, TFT_BLACK);
    tft.drawString(statusStr, 3, 16);
    prevStatus = statusStr;
  }

  tft.drawBitmap(218, 11, image_network_4_bars_bits, 15, 16, iconColor);
}
/** Draw simple UI */
void drawGUI(){
      tft.setTextColor(0x1FEA);
      tft.setTextSize(2);
      tft.setFreeFont();

      drawConnectionStatus(Utils::connectFlag, previousStatus);

      tft.setTextColor(0x5FA);
      tft.drawString(Time::currentTime, 5, 49);
      removeText(Time::currentTime, 5, 49, TFT_BLACK, 0x5FA, previousTime);
    

      tft.setTextColor(0xE8EC);
      tft.setFreeFont(&FreeMonoBold9pt7b);
      tft.setTextSize(2);
      String t = "Temp:" + String(Weather::temperature, 1) + "C";
      tft.drawString(t, 3, 113);
      removeText(t, 3, 113, TFT_BLACK, 0xE8EC, previousTemp);

      tft.setTextColor(0xABF);
      tft.setTextSize(2);
      String h = "Humi: " + String(Weather::humidity) + " %";
      tft.drawString(h, 5, 165);
      removeText(h, 5, 165, TFT_BLACK, 0xABF, previousHumi);

      tft.setTextColor(0xD05F);
      tft.setTextSize(1);
      String p = "Pressure: " + String(Weather::pressure, 1) + " hPa";
      tft.drawString(p, 5, 218);
      removeText(p, 5, 218, TFT_BLACK, 0xD05F, previousPressure);

      tft.setTextColor(0xABF);
      tft.setTextSize(3);
      tft.setFreeFont();
      tft.drawString("", 57, 233);

      tft.setTextColor(0xFA60);
      tft.setTextSize(1);
      tft.setFreeFont(&FreeMonoBold9pt7b);
      String wea = "Weather: " + String(Weather::weatherMain);
      tft.drawString(wea, 7, 255);
      removeText(wea, 7, 255, TFT_BLACK,0xFA60, previousWeather);

      tft.setTextColor(0x50BF);
      String wind = "Windspeed: " + String(Weather::windSpeed, 1) + "m/s";
      tft.drawString(wind, 8, 290);
      removeText(wind, 8, 290, TFT_BLACK, 0x50BF, previousWind);


      tft.setTextColor(0x43A);
      tft.drawString("Ho Chi Minh", 116, 80);
      
      tft.drawBitmap(95, 79, image_location_bits, 13, 16, 0x21F);
}
void setup() {

  Serial.begin(115200);

  Utils::startWiFiAndTimeTasks(); // Start WiFi and time tasks
  Time::startTask(); 
  Weather::startTask();

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  //pinMode(TFT_BL, OUTPUT);
}

void loop() {
  // String tempStr = String(Weather::temperature) + "°C";
  // label_set_text(timeLabel, Time::currentTime.c_str());
  // label_set_text(tempLabel, String(Weather::temperature) + "°C");
  drawGUI();
}
