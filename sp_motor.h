#ifndef _SP_MOTOR_H__
#define _SP_MOTOR_H__

#include "Arduino.h"  

class SPMotor {
  private:
    int _pin;
    int _duration;
  public:
    SPMotor(int pin);
    void begin(int duration);
    void run();
    
};

#endif
