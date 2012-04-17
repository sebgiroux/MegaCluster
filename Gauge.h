/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#ifndef Gauge_h
#define Gauge_h

#include "Serial_LCD.h"
#include "Utils.h"

class Gauge {
  public:
    Gauge(Serial_LCD *p_myLCD);
    
    void drawGauge(uint8_t gaugeIndex,uint16_t x,uint16_t y,uint16_t _radius,int16_t meterFromAngle,int16_t meterToAngle,int16_t minValue,int16_t maxValue,String text,String units);
    void drawNeedle(uint8_t gaugeIndex,float _angleValue);
    void drawNeedle(uint8_t gaugeIndex,float _angleValue, boolean forceValue);
    
  private:
    Serial_LCD *p_myLCD;
    
    uint16_t gaugeX[6];
    uint16_t gaugeY[6];
    
    uint16_t needleX[6];
    uint16_t needleY[6];
    
    uint16_t radius[6];
    
    int16_t meterFromAngle[6];
    int16_t totalAngle[6];
    int16_t minValue[6];
    int16_t maxValue[6];
    float value[6];
    String units[6];
    
    void drawMark(float currentAngle, uint16_t x, uint16_t y,uint16_t radius,uint16_t _size);
    void drawString(float currentAngle, uint16_t x, uint16_t y,uint16_t radius,uint16_t _size, String value,uint16_t offset, float gap);    
};

#endif

