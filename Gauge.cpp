/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#include "Gauge.h"

/*
  Constructor
  
  @param myLCD An instance of the Serial_LCD object
*/
Gauge::Gauge(Serial_LCD *myLCD) {
  p_myLCD = myLCD;
}

/*
  Main public function to draw a gauge
  
  @param gaugeIndex
  @param x
  @param y
  @param radius
  @param meterFromAngle
  @param meterToAngle
  @param minValue
  @param maxValue
  @param text
  @param units
*/
void Gauge::drawGauge(uint8_t gaugeIndex,uint16_t x,uint16_t y,uint16_t _radius,int16_t _meterFromAngle,int16_t _meterToAngle,int16_t _minValue,int16_t _maxValue,String text, String _units) {  
  gaugeX[gaugeIndex] = x;
  gaugeY[gaugeIndex] = y;
  
  needleX[gaugeIndex] = -1;
  needleY[gaugeIndex] = -1;
  
  units[gaugeIndex] = _units;
  
  int16_t _totalAngle = _meterToAngle - _meterFromAngle;
  
  meterFromAngle[gaugeIndex] = _meterFromAngle;
  totalAngle[gaugeIndex] = _totalAngle;
  minValue[gaugeIndex] = _minValue;
  maxValue[gaugeIndex] = _maxValue;
  
  radius[gaugeIndex] = _radius;
  
  // Draw gauge circle
  p_myLCD->setPenSolid(false);
  p_myLCD->circle(x,y, _radius,52825);
  
  // Draw text
  p_myLCD->setFont(0x00);  
  p_myLCD->gText(x - (text.length() * 5 / 2), y + _radius - 35, 0xffffff, text);
  
  float currentAngle = _meterFromAngle * 3.1416 / 180;  
  uint16_t _size = _radius * 2;
  float gap = (_size * 0.6 * 0.03);  
  const uint8_t nbSteps = 10;  
  uint16_t incValue = (_maxValue - _minValue) / nbSteps;
 
  float angleIncr = (_totalAngle / nbSteps) * 3.1416 / 180;
 
  for (uint8_t i = 1; i <= nbSteps; i++) {
    drawMark(currentAngle,x,y,_radius,_size / 20);
    //drawString(currentAngle,x,y,radius,_size / 23,minValue + (incValue * i),_size / 10,gap);
    
    currentAngle += angleIncr;
  } 
}

/*
  Draw all the marks around the gauge
  
  @param currentAngle
  @param x
  @param y
  @param radius
  @param _size
*/
void Gauge::drawMark(float currentAngle, uint16_t x, uint16_t y, uint16_t radius, uint16_t _size) {
  uint16_t x0 = (x + radius * cos(currentAngle));
  uint16_t y0 = (y + radius * sin(currentAngle));
  uint16_t x1 = (x + (radius - _size) * cos(currentAngle));
  uint16_t y1 = (y + (radius - _size) * sin(currentAngle));
  
  p_myLCD->line(x0,y0,x1,y1,52825);  
}

/*
  Draw number around the gauge
  
  @param currentAngle
  @param x
  @param y
  @param radius
  @param _size
  @param value
  @param offset
  @param gap
*/
void Gauge::drawString(float currentAngle, uint16_t x, uint16_t y, uint16_t radius, uint16_t _size, String value,uint16_t offset, float gap) {
  p_myLCD->gText(x + (radius - offset) * cos(currentAngle),y + gap / 2 + (radius - offset) * sin(currentAngle),0xffffff,value);
}

/*
  Draw the needle of the gauge for the specified angle value
  
  @param gaugeIndex
  @param _value
*/
void Gauge::drawNeedle(uint8_t gaugeIndex, float _value) {
  drawNeedle(gaugeIndex,_value,false);
}

void Gauge::drawNeedle(uint8_t gaugeIndex, float _value, boolean forceValue) {    
  /*
    Don't do anything if the value is the same as last time
  */
  if (_value != value[gaugeIndex] || forceValue) {
    uint16_t x = gaugeX[gaugeIndex];
    uint16_t y = gaugeY[gaugeIndex];
    uint16_t _radius = radius[gaugeIndex];
    String _units = units[gaugeIndex];

    /*
      Remove old needle if needed
    */
    if (needleX[gaugeIndex] > -1 && needleX[gaugeIndex] != 65535 && needleY[gaugeIndex] > -1 && needleY[gaugeIndex] != 65535) {
      p_myLCD->line(x,y,needleX[gaugeIndex],needleY[gaugeIndex],0);
    }
    
    /*
      Draw gauge center
    */
    p_myLCD->setPenSolid(true);
    
    float shift = 0.2 * (_radius / 2);
    uint16_t rX = x - (shift / 2);
    uint16_t rY = y - (shift / 2);  
    
    p_myLCD->circle(rX + 3,rY + 3,shift,52825);  
        
    /*
      Draw needle
    */
    value[gaugeIndex] = _value;
    _value = (meterFromAngle[gaugeIndex] + (totalAngle[gaugeIndex] * (maxValue[gaugeIndex] * (_value - minValue[gaugeIndex])) / (maxValue[gaugeIndex] - minValue[gaugeIndex])) / maxValue[gaugeIndex]) * 3.1416 / 180;
    
    rX = (x + (_radius - 10) * cos(_value));
    rY = (y + (_radius - 10) * sin(_value));
    
    p_myLCD->line(x,y,rX,rY,52825);
    
    /*
      Clear old value
    */
    p_myLCD->rectangle(x - _radius + 25,y + _radius - 22,x + _radius - 25,y + _radius - 10,0x000000);
    
    /*
      Draw value
    */
    Utils utils;
    p_myLCD->gText(x - ((utils.ftoa(value[gaugeIndex],2).length() + _units.length()) * 5 / 2), y + _radius - 20, 0xffffff, utils.ftoa(value[gaugeIndex],2) + _units);
    
    /*
      Remember needle position for next time
    */    
    needleX[gaugeIndex] = rX;
    needleY[gaugeIndex] = rY;
  }
}

