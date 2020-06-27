#include "sp_humi_sensor.h"

SPHumiSensor::SPHumiSensor (int pin) {
  _pin = pin;  
}

uint16_t SPHumiSensor::regular_data(uint16_t s) {
    uint16_t max_s = 1024;//0.0
    uint16_t min_s = 320;// 1.0
    if (s < min_s) {
      s = min_s;
    }
    if (s > max_s) {
      s = max_s;
    }    
    float fp = 1.0 - (((float)(s - min_s)) / (float)(max_s - min_s));
    return (uint16_t)(fp*100);    
}

uint16_t SPHumiSensor::get_data(bool fast) {
  uint16_t s = 0;
  if (fast) {
    s = analogRead(A0);
  } else {
    for (int i=0;i<10;i++) {
      s += analogRead(A0);
      delay(100);
    }
    s = s / 10;   
  }
  return regular_data(s);
}
