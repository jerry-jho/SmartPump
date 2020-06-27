#include "sp_motor.h"

SPMotor::SPMotor(int pin) {
  _pin = pin;  
}

void SPMotor::begin(int duration) {
  pinMode(_pin,OUTPUT);
  digitalWrite(_pin,0);
  _duration = duration;    
}

void SPMotor::run() {
  digitalWrite(_pin,0);
  digitalWrite(_pin,1);
  for (int i=0;i<_duration;i++) {
    delay(1000);
  }
  digitalWrite(_pin,0);  
}
