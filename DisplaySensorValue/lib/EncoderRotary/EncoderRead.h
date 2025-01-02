#ifndef ENCODER_READ_H
#define ENCODER_READ_H

#include <Arduino.h>


class EncoderRead {
  public:
    EncoderRead(uint8_t pinA, uint8_t pinB, uint8_t btnPin);
    void begin();                            
    int getCounter();                       
    void resetCounter();                     
    static void readEncoder();  
    bool encBtn();                  

  private:
    uint8_t _pinA, _pinB, _btnPin;    
    volatile int _counter;                   
    unsigned long _lastIncReadTime;          
    unsigned long _lastDecReadTime;     
    int _pauseLength;                        
    int _fastIncrement;                 

    static EncoderRead* instance; 
    static const int8_t _encStates[16];
};

#endif
