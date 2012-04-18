/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#include "DataDisplay.h"
#include "Serial_LCD.h"

const uint8_t DataDisplay::NB_DATA_PER_ROWS = 5;

/*
  Constructor
  
  @param myLCD An instance of the Serial_LCD object
*/
DataDisplay::DataDisplay(Serial_LCD *myLCD) {
  p_myLCD = myLCD;
}

/*
  Draw a new DataDisplay object

  @param dataDisplayIndex The index of the progress bar (The first one MUST be 0!)
*/
void DataDisplay::drawDataDisplay(uint8_t dataDisplayIndex, String _units, String text) {
  units[dataDisplayIndex] = _units;
  
  // Draw text
  p_myLCD->setFont(0x01);
  p_myLCD->gText((dataDisplayIndex % NB_DATA_PER_ROWS * 58) + 5,(dataDisplayIndex / NB_DATA_PER_ROWS * 35) + 25,0xffffff,text);
}

/*
  Update the value of an existing DataDisplay object

  @param dataDisplayIndex   The index of the progress bar
  @param value              The value to be updated with
*/
void DataDisplay::updateValue(uint8_t dataDisplayIndex,float _value, boolean force) {
  if (_value != value[dataDisplayIndex] || force) {
    value[dataDisplayIndex] = _value;
    
    Utils utils;
    String _svalue = utils.ftoa(_value,2);
    
    uint16_t x = (dataDisplayIndex % NB_DATA_PER_ROWS * 58);
    uint16_t y = (dataDisplayIndex / NB_DATA_PER_ROWS * 35);
    
    p_myLCD->rectangle(x,y,x + 58,y + 20,0);
    p_myLCD->setFont(0x02);
    p_myLCD->gText(x + 5,y + 10,0xffffff,_svalue);
  }
}

