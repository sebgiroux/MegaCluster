/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#ifndef ProgressBar_h
#define ProgressBar_h

#include "Serial_LCD.h"
#include "Utils.h"

class ProgressBar {
  public:
    ProgressBar(Serial_LCD *p_myLCD);
    void drawProgressBar(uint8_t progressBarIndex, uint8_t x, uint8_t y, uint8_t w, uint8_t h, float minValue, float maxValue, String text);
    void updateValue(uint8_t progressBarIndex,float value);
    void updateValue(uint8_t progressBarIndex,float value, boolean forceValue);
    
  private:
    Serial_LCD *p_myLCD;
    
    uint8_t progressBarIndex[11];
    
    uint8_t progressBarX[11];
    uint8_t progressBarY[11];
    uint8_t progressBarW[11];
    uint8_t progressBarH[11];
    float progressBarValue[11];
    
    float progressBarMinValue[11];
    float progressBarMaxValue[11];
};

#endif

