/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#ifndef Utils_h
#define Utils_h

#include <WString.h>
#include "WProgram.h"

class Utils {
  public:
    String ftoa(float number, uint8_t precision);
    uint16_t getGradiantGreenToRed(double power);
    boolean isInRect(uint8_t x,uint8_t y,uint8_t xR,uint8_t yR,uint8_t wR,uint8_t hR);
};

#endif

