#ifndef OPENWEATHER_GET_H
#define OPENWEATHER_GET_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

/**
 * Set up wifi and time 
 */
namespace Utils {

  const char* ssid = "your_wifi_ssid";
  const char* password = "your_wifi_password";
  
  const char* ntpServer = "pool.ntp.org";

  const long gmtOffset_sec = 7 * 3600; // GMT+7
  const int daylightOffset_sec = 0;

  bool connectFlag = false; // Flag to check wifi connected or not

/** Let check wifi connected or not */
void startWiFiTask(void* pvParams) {

    WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      }
    Serial.println("WiFi connected.");
    vTaskDelete(NULL);
}
/** Let check NTP status */ 
void startNTPTask(void* pvParams) {

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
      while (!getLocalTime(&timeinfo)) {
      Serial.println("Waiting for NTP...");
      delay(1000);
      }
    Serial.println("NTP synced.");
    connectFlag = true;
    vTaskDelete(NULL);
}

void startWiFiAndTimeTasks() {
xTaskCreatePinnedToCore(startWiFiTask, "WiFi Task", 2048, NULL, 3, NULL, 0);
xTaskCreatePinnedToCore(startNTPTask, "NTP Task", 2048, NULL, 2, NULL, 0);
}

/** Check HTTP Status */
  String httpGETRequest(const char* Url) {
    HTTPClient http;
    if (!http.begin(Url)) {
      Serial.println("HTTP begin failed");
      return "{}";
    }

    int responseCode = http.GET();
    String responseBody = "{}";
    if (responseCode > 0) {
      responseBody = http.getString();
    } else {
      Serial.print("Error Code: ");
      Serial.println(responseCode);
    }

    http.end();
    return responseBody;
  }
}
/** Update time per minute */
namespace Time {

    String currentTime = "Unknown";
    TaskHandle_t timeTaskHandle = NULL;
  
    void updateTimeTask(void* pvParams) {
      while (1) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
          char buffer[64];
          strftime(buffer, sizeof(buffer), "%H:%M %d/%m/%Y", &timeinfo);//%H:%M:%S
          currentTime = String(buffer);
          Serial.print("Time now: ");
          Serial.println(currentTime);
        } else {
          Serial.println("Failed to get time in update task");
        }
  
        vTaskDelay(pdMS_TO_TICKS(60000)); 
      }
    }
  
    void startTask() {
      xTaskCreate(&updateTimeTask, "Time Task", 2048, NULL, 3, &timeTaskHandle);
    }
  }
  
/** Get weather data from OpenWeather */
namespace Weather {

  String weatherMain = "";
  String weatherDescription = "";
  String city = "";
  float temperature = 0;
  int humidity = 0;
  float pressure = 0;
  float windSpeed = 0;

/** Add you City and API key:: Ex: Soc Trang => Soc%20Trang Ho%20Chi%20Minh%20City || appid = your_api_key*/
  String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q= Ho%20Chi%20Minh%20City&appid=your_api_key";
  TaskHandle_t weatherTaskHandle = NULL;

/** Parse JSON data */
bool ParsehWeatherData() {
    String weatherJson = Utils::httpGETRequest(weatherUrl.c_str());

    Serial.print(weatherJson);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, weatherJson);
    if (error) {
      Serial.print("JSON error: ");
      Serial.println(error.c_str());
      return false;
    }
    temperature = doc["main"]["temp"].as<float>() - 273.15;
    humidity = doc["main"]["humidity"];
    pressure = doc["main"]["pressure"];
    windSpeed = doc["wind"]["speed"];
    weatherMain = doc["weather"][0]["main"].as<String>();
    weatherDescription = doc["weather"][0]["description"].as<String>();
  
    Serial.printf("Temp: %.1f°C | Humidity: %d%% | Pressure: %.1f hPa\n", temperature, humidity, pressure);
    Serial.printf("Weather: %s (%s) | Wind: %.1f m/s\n", weatherMain.c_str(), weatherDescription.c_str(), windSpeed);
    return true;
  }

void weatherTask(void* pvParams) {
      while (WiFi.status() != WL_CONNECTED) {
            Serial.println("Waiting for WiFi...");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        Serial.println("Initial fetch weather...");
            ParsehWeatherData();
       while (1) {
            if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Fetching weather...");
            ParsehWeatherData();
            } else {
            Serial.println("WiFi disconnected");
            }

            Serial.println("-------------------------");
        vTaskDelay(pdMS_TO_TICKS(300000));  // 5 phút
    }
}

  void startTask() {
    xTaskCreatePinnedToCore(&weatherTask, "Weather Task", 4096, NULL,4, &weatherTaskHandle,0);
  }
}
#endif
