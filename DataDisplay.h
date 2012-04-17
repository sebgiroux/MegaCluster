/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#ifndef DataDisplay_h
#define DataDisplay_h

#include "Serial_LCD.h"
#include "Utils.h"

class DataDisplay {
  public:
    DataDisplay(Serial_LCD *p_myLCD);
    void drawDataDisplay(uint8_t barGraphIndex, float value, String units, String text);
    void updateValue(uint8_t barGraphIndex,float value,boolean force);
    
  private:
    Serial_LCD *p_myLCD;

    float value[26];
    String units[26];
    
    static const uint8_t NB_DATA_PER_ROWS;
};

#endif

