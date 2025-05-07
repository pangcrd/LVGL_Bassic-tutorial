#include <Arduino.h>
#include <TFT_eSPI.h> 
#include "esp_now.h"
#include "WiFi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/** Change to your screen resolution */
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 320;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); 

typedef struct {
  float temp; 
  float accAngleX; 
  float accAngleY; 
  float gyroAngleX; 
  float gyroAngleY;
  float gyroAngleZ;
  float angleX;
  float angleY; 
  float angleZ;
} Data; 
Data data;

String lastTextTemp = "";
String lastTextAccAngleX = "";
String lastTextAccAngleY = "";
String lastTextGyroAngleX = "";
String lastTextGyroAngleY = "";
String lastTextGyroAngleZ = "";
String lastTextAngleX = "";
String lastTextAngleY = "";
String lastTextAngleZ = "";

/** ==================== ESP-NOW Config ====================== */
uint8_t SenderMAC[] = {0xFC, 0xE8, 0xC0, 0x75, 0x99, 0xD0};

void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Recv Data: ");
  Serial.println(data.temp);
  Serial.println(data.accAngleX);
  Serial.println(data.accAngleY);
  Serial.println(data.gyroAngleX);
  Serial.println(data.gyroAngleY);
  Serial.println(data.gyroAngleZ);
  Serial.println(data.angleX);
  Serial.println(data.angleY);
  Serial.println(data.angleZ);
  Serial.println("=======================================================\n");
 
}
/** ==================== ESP-NOW Config END ====================== */

/** Delete old value every time you update new value to avoid overlapping or screen flickering */
void UpdateText(String current, int x, int y, uint16_t bg, uint16_t fg, String &lastText) {
  if (current != lastText) {
    tft.fillRect(x, y, tft.textWidth(lastText), 16, bg); 
    tft.setTextColor(fg, bg);
    tft.drawString(current, x, y);
    lastText = current;
  }
}

/** Draw simple UI */
void drawGUI(){
            
      tft.setTextColor(0x6FE1);
      tft.setTextSize(2);
      tft.setFreeFont();
      tft.drawString("MPU6050 REVC", 53, 25);

      tft.setTextColor(0xE8EC);
      String temp = String(data.temp, 2);
      UpdateText(temp, 80, 55, TFT_BLACK, 0xE8EC, lastTextTemp);
      tft.drawString("Temp:", 3, 57);

      tft.setTextColor(0x24BE);
      tft.drawString("accAngleX:", 2, 84);
      String accAngleX = String(data.accAngleX, 2);
      UpdateText(accAngleX, 150, 81, TFT_BLACK, 0x24BE, lastTextAccAngleX);

      tft.setTextColor(0x24BE);
      String accAngleY = String(data.accAngleY, 2);
      UpdateText(accAngleY, 150, 113, TFT_BLACK, 0x24BE, lastTextAccAngleY);
      tft.drawString("accAngleY:", 4, 115);


      tft.setTextColor(0xFFFF);
      String gyroAngleX = String(data.gyroAngleX, 2);
      UpdateText(gyroAngleX, 150, 148, TFT_BLACK, 0xFFFF, lastTextGyroAngleX);
      tft.drawString("gyroAngleX:", 3, 148);

      tft.setTextColor(0xEF7D);
      String gyroAngleY = String(data.gyroAngleY, 2);
      UpdateText(gyroAngleY, 150, 179, TFT_BLACK, 0xEF7D, lastTextGyroAngleY);
      tft.drawString(" gyroAngleY:", -7, 180);

      
      tft.setTextColor(0xFFFF);
      String gyroAngleZ = String(data.gyroAngleZ, 2);
      UpdateText(gyroAngleZ, 150, 210, TFT_BLACK, 0xFFFF, lastTextGyroAngleZ);
      tft.drawString(" gyroAngleZ: ", -7, 212);

      tft.setTextColor(0x67E0);
      String angleX = String(data.angleX, 2);
      UpdateText(angleX, 150, 240, TFT_BLACK, 0x67E0, lastTextAngleX);
      tft.drawString("angleX:", 5, 240);
      
      tft.setTextColor(0x67E0);
      String angleY = String(data.angleY, 2);
      UpdateText(angleY, 150, 266, TFT_BLACK, 0x67E0, lastTextAngleY);
      tft.drawString("angleY:", 5, 266);

      tft.setTextColor(0x67E0);
      String angleZ = String(data.angleZ, 2);
      UpdateText(angleZ, 150, 292, TFT_BLACK, 0x67E0, lastTextAngleZ);
      tft.drawString("angleZ: ", 6, 294);
}
void setup() {

  Serial.begin(115200);
  while (!Serial) delay(10); 
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  //pinMode(TFT_BL, OUTPUT);
  Serial.println("ESP-NOW Receiver Starting...");
  

  WiFi.mode(WIFI_STA);
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
    
  }
  Serial.println("ESP-NOW initialized successfully");

  // Register peer master (Receiver)
  esp_now_peer_info_t master;
  memset(&master, 0, sizeof(master));
  memcpy(master.peer_addr,SenderMAC, 6); 
  master.channel = 0;  // Choose channel for peer (0 is automatic selection)
  master.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&master) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer added successfully");

  // Register callback function for received data
  esp_now_register_recv_cb(onDataRecv);
  Serial.println("Callback registered, waiting for data...");
}

void loop() {
  drawGUI();
}