#include <Arduino.h>
#include "ArduinoJson.h"
#include <HardwareSerial.h>

#define LED 27
#define buzzer 26

HardwareSerial mySerial(2);

/** Create struct for data packet */
typedef struct Data{
  uint8_t buttonstate;
  uint16_t pwmValue;
} Data;

Data myPacket;

/** Receive JSON and parse from Master */
 void RecvJsonData(){
   /** Read and check json character */
   while (mySerial.available()){ 
    char input [50]; //Buffeer lagre enough to store json data
    int len = mySerial.readBytesUntil('}', input, sizeof(input)-2); // read json data until new line
    input [len] = '}'; //close symbol json
    input [len+1] = '\0'; //null terminate

    JsonDocument RecvData;
    DeserializationError error = deserializeJson(RecvData, input);
    if (!error){
      String type = RecvData["type"]; // Checkdata type
       if (type == "button"){

          myPacket.buttonstate = RecvData["buttonstate"];
          Serial.print ("Recv Button State:"); Serial.println(myPacket.buttonstate);
          if (myPacket.buttonstate == 0) //Low state pull-up button
          {
            tone(buzzer,1000); //turn ON buzzer
            delay(500);
            noTone(buzzer); //turn OFF buzzer
          }
          
       }
       else if (type == "potentiometer"){

          myPacket.pwmValue = RecvData["pwmValue"];
          Serial.print ("Recv PWM Value:"); Serial.println(myPacket.pwmValue);
          analogWrite(LED, myPacket.pwmValue); //write pwm value to adjust LED brightness
    }
    
   }

  }  
 }
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // tx, rx

  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  RecvJsonData();
}


