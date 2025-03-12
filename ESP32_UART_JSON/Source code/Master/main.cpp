#include <Arduino.h>
#include "ArduinoJson.h"
#include <HardwareSerial.h>

#define buttonPin 35
#define potentiometer 33

HardwareSerial mySerial(2); //uart2

/** Create struct for data packet */
typedef struct Data{
  uint8_t buttonstate;
  uint16_t pwmValue;
} Data;

Data myPacket;

bool lastbuttonstate = false;
uint16_t lastValue = 0;

/** Create JSON Function */

void CreateJsonData(const String &type){

   JsonDocument mydata; //create json document

   mydata["type"] = type;
    
   if(type == "button"){

     mydata["buttonstate"] = myPacket.buttonstate;

   }else if(type == "potentiometer"){

      mydata["pwmValue"] = myPacket.pwmValue;
      
    }
    String output;
    serializeJson(mydata, output); // add mydata to output send to serial
    mySerial.println(output);
    Serial.println(output); // Print json data to serial monitor
}

/** Read button and potentiometer data and add to JSON function */
void dataPacketCreate(){
  myPacket.buttonstate = digitalRead(buttonPin); // read button state
   if (myPacket.buttonstate != lastbuttonstate){
      if(myPacket.buttonstate == LOW){
        CreateJsonData("button"); //Add to JSON
      }
      lastbuttonstate = myPacket.buttonstate; //update when button state changes
      Serial.println(myPacket.buttonstate);
   }
  // Potentiometer read
  uint16_t potentiometerValue = analogRead(potentiometer);
     myPacket.pwmValue = map(potentiometerValue, 0, 4095, 0, 255); // map potentiometer value to 0-255
     
     if (abs(myPacket.pwmValue - lastValue)>2){
      lastValue = myPacket.pwmValue;
      Serial.println(myPacket.pwmValue);
      CreateJsonData("potentiometer"); // add to JSON
     }
     
}
void setup() {

  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // tx, rx
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  pinMode(potentiometer, INPUT);

  
}

void loop() {
// put your main code here, to run repeatedly:
dataPacketCreate(); // update/send data packet

}

