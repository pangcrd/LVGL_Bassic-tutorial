#include <MPU6050_tockn.h>
#include <Wire.h>
#include "esp_now.h"
#include "WiFi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

MPU6050 mpu6050(Wire);


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

/** REPLACE WITH YOUR RECEIVER MAC Address **/

uint8_t MACAddress[] = {0xD4, 0x8A, 0xFC, 0xC8, 0xE7, 0x64};

/** ==================== ESP-NOW Config ====================== */
esp_now_peer_info_t slave;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void espnowInitTask(void* pvParams){
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      vTaskDelete(NULL);
      return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(slave.peer_addr, MACAddress, 6);
  slave.channel = 0;  
  slave.encrypt = false;

  bool peerAdded = false;
  int retry = 0;
  while (!peerAdded && retry < 10) {
      if (esp_now_add_peer(&slave) == ESP_OK){
          Serial.println("Peer added successfully");
          peerAdded = true;
      } else {
          Serial.println("Failed to add peer. Retrying...");
          vTaskDelay(pdMS_TO_TICKS(500)); 
          retry++;
      }
  }

  if (!peerAdded) {
      Serial.println("Failed to add peer after retries");
  }

  vTaskDelete(NULL);  
}

void sendDataTask(void* pvParams) {
  while (1) {
      esp_now_send(MACAddress, (uint8_t *) &data, sizeof(data));
      vTaskDelay(pdMS_TO_TICKS(500)); 
  }  
}


/** ==================== ESP-NOW Config END ====================== */
void mpu6050Run(void *pvParameters) {
  
  while (1)
  {
        mpu6050.update();
        data.temp = mpu6050.getTemp();
        data.accAngleX = mpu6050.getAccAngleX();
        data.accAngleY = mpu6050.getAccAngleY();
        data.gyroAngleX = mpu6050.getGyroAngleX();
        data.gyroAngleY = mpu6050.getGyroAngleY();
        data.gyroAngleZ = mpu6050.getGyroAngleZ();
        data.angleX = mpu6050.getAngleX();
        data.angleY = mpu6050.getAngleY();
        data.angleZ = mpu6050.getAngleZ();

        // Serial.println("=======================================================");
        // Serial.print("temp : ");Serial.println(mpu6050.getTemp());
        // Serial.print("accX : ");Serial.print(mpu6050.getAccX());
        // Serial.print("\taccY : ");Serial.print(mpu6050.getAccY());
        // Serial.print("\taccZ : ");Serial.println(mpu6050.getAccZ());
      
        // Serial.print("gyroX : ");Serial.print(mpu6050.getGyroX());
        // Serial.print("\tgyroY : ");Serial.print(mpu6050.getGyroY());
        // Serial.print("\tgyroZ : ");Serial.println(mpu6050.getGyroZ());
      
        // Serial.print("accAngleX : ");Serial.print(mpu6050.getAccAngleX());
        // Serial.print("\taccAngleY : ");Serial.println(mpu6050.getAccAngleY());
      
        // Serial.print("gyroAngleX : ");Serial.print(mpu6050.getGyroAngleX());
        // Serial.print("\tgyroAngleY : ");Serial.print(mpu6050.getGyroAngleY());
        // Serial.print("\tgyroAngleZ : ");Serial.println(mpu6050.getGyroAngleZ());
        
        // Serial.print("angleX : ");Serial.print(mpu6050.getAngleX());
        // Serial.print("\tangleY : ");Serial.print(mpu6050.getAngleY());
        // Serial.print("\tangleZ : ");Serial.println(mpu6050.getAngleZ());
        // Serial.println("=======================================================\n");

  }
  vTaskDelay(pdMS_TO_TICKS(10));  
}
void setup(){
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  // pinMode(MPU_INT_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), onMotionDetected, RISING);
  xTaskCreatePinnedToCore(espnowInitTask, "espnow start", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(mpu6050Run, "mpu6050Run", 4046, NULL, 4, NULL, 0);
  xTaskCreatePinnedToCore(sendDataTask, "send data",4046, NULL, 3, NULL, 0);
}

void loop(){}