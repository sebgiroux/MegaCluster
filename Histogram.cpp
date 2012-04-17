/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#include "Histogram.h"
#include "Serial_LCD.h"

const uint8_t Histogram::NB_VALUES_TO_DISPLAY = 15;


/*
  Constructor
  
  @param myLCD An instance of the Serial_LCD object
*/
Histogram::Histogram(Serial_LCD *myLCD) {
  p_myLCD = myLCD;
  
  nbValueInArray = 0;
}

/*

  @param x
  @param y
  @param w
  @param h
  @param minValue
  @param maxValue
*/
void Histogram::drawHistogram(uint16_t x,uint16_t y, uint16_t w, uint16_t h, float minYValue, float maxYValue) {
  histogramX[0] = x;
  histogramY[0] = y;
  histogramW[0] = w;
  histogramH[0] = h;
  
  histogramMinYValue[0] = minYValue;
  histogramMaxYValue[0] = maxYValue;
  
  // Draw X axis
  p_myLCD->line(x,y + h,x + w,y + h,p_myLCD->rgb16(200,200,200));
  
  // Draw Y axis
  p_myLCD->line(x,y,x,y + h,p_myLCD->rgb16(200,200,200));
  
  Utils utils;
  
  // Draw max value
  p_myLCD->setFont(0x00);
  p_myLCD->gText(x - 16,y,0xffffff,utils.ftoa(maxYValue,2));  
  
  // Draw middle value
  p_myLCD->gText(x - 16,y + (h / 2) - 6,0xffffff,utils.ftoa((maxYValue - minYValue) / 2 + minYValue,2));
  
  // Draw min value
  p_myLCD->gText(x - 16,y + h - 8,0xffffff,utils.ftoa(minYValue,2));  
}

/*
  
  @param value
*/
void Histogram::addValue(float value) {
  float minXValue = histogramMinYValue[0];
  float maxXValue = histogramMaxYValue[0];
  
  // Make sure we are not going over the limit
  if (value > maxXValue) {
    value = maxXValue; 
  }
  
  // Make sure we are not going under the limit
  if (value < minXValue) {
    value = minXValue; 
  }
  
  // If we haven't reach the limit of the array
  if (nbValueInArray < NB_VALUES_TO_DISPLAY) {  
    histogramValue[nbValueInArray++] = value;
  }
  // Otherwise we shift all the value and replace the last one with the new value
  else {
    uint16_t i;
    for (i = 1; i < NB_VALUES_TO_DISPLAY - 1; i++) {
      histogramValue[i] = histogramValue[i + 1];
    }
    
    histogramValue[i] = value;
  }
  
  // We need at least two pair of coords to draw a line
  if (nbValueInArray > 1) {
    uint16_t x = histogramX[0];
    uint16_t y = histogramY[0];
    uint16_t w = histogramW[0];
    uint16_t h = histogramH[0];
    
    // Clear everything that is there
    p_myLCD->rectangle(x + 1,y,x + w,y + h - 1,0);  
    
    uint16_t pixelsBetweenValue = w / NB_VALUES_TO_DISPLAY;
    
    float currentValue;
    uint16_t currentY,oldX,oldY;
    
    float oldValue = histogramValue[nbValueInArray - 1];
    
    // Draw the value
    for (int16_t i = nbValueInArray - 2; i >= 0; i--) {
      currentValue = histogramValue[i];
      
      currentY = y + h * (1 - (currentValue - minXValue) / (maxXValue - minXValue));
      
      oldX = x + (pixelsBetweenValue * (nbValueInArray - 2 - i)) + 1;
      oldY = y + h * (1 - (oldValue - minXValue) / (maxXValue - minXValue));     
      
      // Draw new value
      p_myLCD->line(oldX,oldY,oldX + pixelsBetweenValue,currentY,52825);
      
      oldValue = currentValue;
    }
  }
}

