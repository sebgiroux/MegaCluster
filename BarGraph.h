/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#ifndef BarGraph_h
#define BarGraph_h

#include "Serial_LCD.h"
#include "Utils.h"

class BarGraph {
  public:
    BarGraph(Serial_LCD *p_myLCD);
    void drawBarGraph(uint8_t barGraphIndex, uint16_t x,uint16_t y, uint16_t w, uint16_t h);
    void updateValue(uint8_t barGraphIndex, float value);
    
  private:
    Serial_LCD *p_myLCD;
    
    uint16_t barGraphX[1];
    uint16_t barGraphY[1];
    uint16_t barGraphW[1];
    uint16_t barGraphH[1];
    
    float barGraphValue[1];
    
    static const uint8_t BAR_WIDTH;
    static const uint8_t BAR_SPACING;
    
};

#endif

