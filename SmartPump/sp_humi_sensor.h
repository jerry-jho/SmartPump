#ifndef _SP_HUMI_SENSOR_H__
#define _SP_HUMI_SENSOR_H__

#include "Arduino.h"

typedef 

class SPHumiSensor {
  private:
    int _pin;
    uint16_t regular_data(uint16_t s);
  public:
    SPHumiSensor(int pin);
    uint16_t get_data(bool fast);
};

#endif
