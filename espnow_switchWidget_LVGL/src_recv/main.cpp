#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define LED 4  // Define your LED pin here, assuming GPIO2

typedef struct {
  bool switch_states;  /** Create switch state **/ 
} sw_state;

sw_state state;

/** MAC Address of the receiver => Master **/
uint8_t MasterAddress[] = {0xD4, 0x8A, 0xFC, 0xC8, 0xE7, 0x64};

void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
        memcpy(&state, incomingData, sizeof(state));
        Serial.print("Switch state: ");
        Serial.println(state.switch_states ? "ON" : "OFF");
        digitalWrite(LED, state.switch_states ? HIGH : LOW);
}

void setup() {

  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for serial to initialize
  
  Serial.println("ESP-NOW Receiver Starting...");
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // Initialize LED state

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
  memcpy(master.peer_addr, MasterAddress, 6); 
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

void loop() {}