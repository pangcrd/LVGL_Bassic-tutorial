#ifndef XYM02_CONFIG_H
#define XYM02_CONFIG_H

#include <Arduino.h>
#include <ModbusMaster.h>

// Instantiate ModbusMaster object
ModbusMaster node;
unsigned long lastUpdate = 0;

namespace XYMD02 {
    
    void setupModbus(HardwareSerial &rs485Serial, 
                     uint8_t slaveID,
                     int txPin, int rxPin) {
        // Initialize RS485 serial communication
        rs485Serial.begin(9600,SERIAL_8N1, rxPin, txPin);
        // Initialize Modbus communication
        node.begin(slaveID, rs485Serial);
    }

    void GetSensorData(ModbusMaster &node) {
        uint8_t result;
        uint16_t data[2];

        // Query the SHT20 sensor: Read 2 registers starting at address 0x0001 (Datasheet)
        result = node.readInputRegisters(0x0001, 2);

        if (result == node.ku8MBSuccess) {
            // Extract data from response buffer
            data[0] = node.getResponseBuffer(0); // First register: Temperature
            data[1] = node.getResponseBuffer(1); // Second register: Humidity

            // Convert raw data to human-readable values
            float temperature = data[0] / 10.0; // Divide by 10 for actual temperature
            float humidity = data[1] / 10.0;    // Divide by 10 for actual humidity

            // Print values to Serial Monitor
            Serial.print("Temperature: ");
            Serial.print(temperature);
            Serial.println(" °C");

            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println(" %");
        } else {
            Serial.print("Failed to read from RS485 sensor. Error Code: ");
            Serial.println(result);
    }
}   

    void UpdateValue(unsigned long updateTime) {
        if (millis() - lastUpdate >= updateTime) {
            lastUpdate = millis();
            GetSensorData(node);
    }
}
}
#endif