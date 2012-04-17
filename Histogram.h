/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#ifndef Histogram_h
#define Histogram_h

#include "Serial_LCD.h"
#include "Utils.h"

class Histogram {
  public:
    Histogram(Serial_LCD *p_myLCD);
    void drawHistogram(uint16_t x, uint16_t y, uint16_t w, uint16_t h, float minXValue, float maxXValue);
    void addValue(float value);
    
  private:
    Serial_LCD *p_myLCD;
    
    uint16_t histogramX[1];
    uint16_t histogramY[1];
    uint16_t histogramW[1];
    uint16_t histogramH[1];
    
    float histogramMinYValue[1];
    float histogramMaxYValue[1];
    
    float histogramValue[15];
    uint8_t nbValueInArray;
    
    static const uint8_t NB_VALUES_TO_DISPLAY;
};

#endif

