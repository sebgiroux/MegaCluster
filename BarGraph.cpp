/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#include "BarGraph.h"
#include "Serial_LCD.h"

const uint8_t BarGraph::BAR_WIDTH = 6;
const uint8_t BarGraph::BAR_SPACING = 5;

/*
  Constructor
  
  @param myLCD An instance of the LCD object
*/
BarGraph::BarGraph(Serial_LCD *myLCD) {
  p_myLCD = myLCD;
}

/*
  Draw a new DataDisplay object

  @param barGraphIndex  The index of the progress bar (The first one MUST be 0!)
  @param x              x position on the screen
  @param y              y position on the screen
  @param w              width of the bar graph
  @param h              height of the bar graph
*/
void BarGraph::drawBarGraph(uint8_t barGraphIndex, uint16_t x,uint16_t y, uint16_t w, uint16_t h) {
  barGraphX[barGraphIndex] = x;
  barGraphY[barGraphIndex] = y;
  barGraphW[barGraphIndex] = w;
  barGraphH[barGraphIndex] = h;
  barGraphValue[barGraphIndex] = 0;
  
  uint8_t nbBars = w / (BAR_WIDTH + BAR_SPACING);
  
  uint8_t barHeight = 5;
  uint8_t barHeightInc = (h - barHeight) / nbBars;
  
  p_myLCD->setPenSolid(true);
  
  for (uint8_t i = 0; i < nbBars; i++) {
    p_myLCD->rectangle(x + (i * (BAR_WIDTH + BAR_SPACING)),y + h - barHeight,x + (i * (BAR_WIDTH + BAR_SPACING)) + BAR_WIDTH,y + h,0xffffff);  
    
    barHeight += barHeightInc;
  }
}

/*
  Update the value of an existing BarGraph object

  @param barGraphIndex      The index of the bar graph
  @param value              The value to be updated with
*/
void BarGraph::updateValue(uint8_t barGraphIndex, float value){
  uint16_t x = barGraphX[barGraphIndex];
  uint16_t y = barGraphY[barGraphIndex];
  uint16_t w = barGraphW[barGraphIndex];
  uint16_t h = barGraphH[barGraphIndex];
  
  float oldValue = barGraphValue[barGraphIndex];
  
  uint8_t nbBars = w / (BAR_WIDTH + BAR_SPACING);  
  uint8_t barHeight = 5;
  uint8_t barHeightInc = (h - barHeight) / nbBars;  
  uint8_t nbBarsColoredForValue = nbBars * value;
  
  p_myLCD->setPenSolid(true);
  
  Utils utils;
  
  uint8_t i;
  for (i = 0; i < nbBarsColoredForValue; i++) {   
    // Don't draw outside the specified space!
    if (i * (BAR_WIDTH + BAR_SPACING) + BAR_WIDTH > w) break;
     
     p_myLCD->rectangle(x + (i * (BAR_WIDTH + BAR_SPACING)),y + h - barHeight,x + (i * (BAR_WIDTH + BAR_SPACING)) + BAR_WIDTH,y + h,utils.getGradiantGreenToRed(1 - ((float)i / (float)nbBars)));
     
     barHeight += barHeightInc;
  } 
  
  // If old value was higher then the new one, we need to put some bars to original color
  if (oldValue > value) {
    for (i = nbBarsColoredForValue; i < nbBars; i++) {
      p_myLCD->rectangle(x + (i * (BAR_WIDTH + BAR_SPACING)),y + h - barHeight,x + (i * (BAR_WIDTH + BAR_SPACING)) + BAR_WIDTH,y + h,0xffffff);
      
      barHeight += barHeightInc;
    }
  }
  
  barGraphValue[barGraphIndex] = value;
}

