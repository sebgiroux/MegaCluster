/* 
 Mega Cluster
 By Sébastien Giroux
 
 Copyright - (c) 2011-2012
 */

#include "ProgressBar.h"
#include "Serial_LCD.h"

/*
  Constructor
 */
ProgressBar::ProgressBar(Serial_LCD *myLCD) {
  p_myLCD = myLCD;
}

/*
  Draw a progress bar on the LCD to the specified location and size. Use the index to be able to update the progress bar later.
 
 @param progressBarIndex   The index of the progress bar
 @param x                  The X position of the progress bar on the LCD
 @param y                  The Y position of the progress bar on the LCD
 @param w                  The width of the progress bar on the LCD
 @param h                  The height of the progress bar on the LCD
 @param text               The text to display above the progress bar to identify it
 */
void ProgressBar::drawProgressBar(uint8_t progressBarIndex, uint8_t x, uint8_t y, uint8_t w, uint8_t h,float minValue, float maxValue, String text) {
  progressBarX[progressBarIndex] = x;
  progressBarY[progressBarIndex] = y;
  progressBarW[progressBarIndex] = w;
  progressBarH[progressBarIndex] = h;

  progressBarMinValue[progressBarIndex] = minValue;
  progressBarMaxValue[progressBarIndex] = maxValue;

  // Draw border 
  p_myLCD->setPenSolid(false);
  p_myLCD->rectangle(x,y + 12,x + w,y + h + 12,52825);

  // Draw text
  p_myLCD->setFont(0x00);
  p_myLCD->gText(x,y,0xffffff,text);
}

/*
  Update the value of the specified progress bar to the new specified value
 
 @param progressBarIndex   The index of the progress bar to update
 @param value              New value of the progress bar
 */
void ProgressBar::updateValue(uint8_t progressBarIndex,float value) {
  updateValue(progressBarIndex,value,false);
}

void ProgressBar::updateValue(uint8_t progressBarIndex,float value, boolean forceValue) {
  /*
    Don't do anything if the value is the same as last time
  */
  if (value != progressBarValue[progressBarIndex] || forceValue) {
    uint8_t x = progressBarX[progressBarIndex];
    uint8_t y = progressBarY[progressBarIndex];
    uint8_t w = progressBarW[progressBarIndex];
    uint8_t h = progressBarH[progressBarIndex];    
  
    float minValue = progressBarMinValue[progressBarIndex];
    float maxValue = progressBarMaxValue[progressBarIndex];
  
    int8_t widthProgressBar = w * (value - minValue) / (maxValue - minValue);
    int8_t oldWidthProgressBar = w * (progressBarValue[progressBarIndex] - minValue) / (maxValue - minValue);
    
    progressBarValue[progressBarIndex] = value;
    
    // We don't wanna go under or over the width limit
    if (widthProgressBar > w - 1) widthProgressBar = w - 1;
    if (widthProgressBar < 0) widthProgressBar = 0;
  
    p_myLCD->setPenSolid(true);
    // Redraw bar only if forcing or bar width is different (value can change while width stay the same)
    if (forceValue || widthProgressBar != oldWidthProgressBar) {
      // Clear old bar
      p_myLCD->rectangle(x + 1,y + 13,x+w-1,y + h + 11,0);
    
      if (widthProgressBar > 1)  {
        // Draw new bar
        p_myLCD->rectangle(x + 1,y + 13,x + widthProgressBar,y + h + 11,p_myLCD->rgb16(35,100,150)); // 0,180,0
      }
    }  
  
    // Clear old value
    p_myLCD->rectangle(x + w - 25,y,x + w + 5,y + 10,0);  
  
    // Draw updated value
    Utils utils;
    String _value = utils.ftoa(value,2);
    
    p_myLCD->setFont(0x00);
    p_myLCD->gText((uint16_t) x + w - (_value.length() * 5),y,0xffffff,_value);
  }
}


