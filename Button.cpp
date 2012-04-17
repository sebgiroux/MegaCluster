//
// �LCD-32PT(SGC) 3.2� Serial LCD Display Module
// Arduino & chipKIT Library
//
// May 10, 2011 release 1 - initial release
// Jun 15, 2011 release 2 - features added and bugs fixed
// Jun 29, 2011 release 3 - setBackGroundColour added and SD card
// Jul 31, 2011 release 4 - stdint.h types for chipKIT compatibility
// Aug 04, 2011 release 5 - chipKIT compatibility with external proxySerial.h
// Aug 07, 2011 release 6 - playing sounds - up to 250 mA!
// Sep 18, 2011 release 7 - dialog window with up to 3 Buttons
// Sep 23, 2011 release 8 - ms monitoring to avoid RX TX collapse
// Oct 10, 2011 release 9 - Stream.h class based i2cSerial library
// Oct 14, 2011 release 10 - ellipse and detectTouchRegion from sebgiroux
// Oct 24, 2011 release 11 - serial port managed in main only - setSpeed added - proxySerial still needed
// Oct 27, 2011 release 12 - setSpeed fixed for 155200 
// Nov 02, 2011 release 13 - HardwareSerial derived from Stream on chipKIT platform by msproul
// Nov 09, 2011 release 14 - proxySerial as autonomous project with ftoa utility
// Nov 25, 2011 release 15 - faster dialog show/hide and optional area for screen copy to/read from SD
// Nov 29, 2011 release 16 - read pixel colour and new colour functions
//
//
// CC = BY NC SA
// http://sites.google.com/site/vilorei/
// http://github.com/rei-vilo/Serial_LCD
//
// Based on
// 4D LABS PICASO-SGC Command Set
// Software Interface Specification
// Document Date: 1st March 2011 
// Document Revision: 6.0
// http://www.4d-Labs.com
//
//

#include "Serial_LCD.h"
#include "WProgram.h"
#include "Button.h"

Button::Button(Serial_LCD * pscreen0)   {   
  _pscreen = pscreen0;
}

void Button::define(uint16_t x1, uint16_t y1, uint16_t dx1, uint16_t dy1, String text0, uint16_t textColour1, uint16_t highColour1, uint16_t lowColour1) {
  define( x1, y1, dx1, dy1, text0, textColour1, highColour1, lowColour1, 9);
}

void Button::define(uint16_t x1, uint16_t y1, uint16_t dx1, uint16_t dy1, String text0, uint16_t textColour1, uint16_t highColour1, uint16_t lowColour1, uint8_t size0) {

  _x1 = x1;
  _y1 = y1;
  _x2 = _x1 + dx1;
  _y2 = _y1 + dy1;

  _textColour = textColour1;
  _highColour = highColour1;
  _lowColour = lowColour1;
  _enable = false;  


  if (size0 == 9) {
    uint16_t i=4;
    do {
      i--;
      _pscreen->setFont(i);
    } 
    while ((text0.length() * _pscreen->fontX() > dx1 ) && ( i > 0));
    _size=i;
  } 
  else {
    _size = size0;
  }
  _pscreen->setFont(_size);
  _text = text0.substring(0, min(text0.length(), dx1 / _pscreen->fontX()));
  _text = _text.trim();
  _xt = _x1 + ( dx1 - (_text.length() * _pscreen->fontX()) ) /2;
  _yt = _y1 + ( dy1 - _pscreen->fontY() ) /2;

}

void Button::draw(bool b1) {
  if (_enable) {
    if (b1) {
      _pscreen->setPenSolid(true);
      _pscreen->rectangle(_x1, _y1, _x2, _y2, _highColour);
      delay(10);    // delay required by chipKIT
      _pscreen->setPenSolid(false);
      _pscreen->rectangle(_x1+2, _y1+2, _x2-2, _y2-2, _lowColour); 

      _pscreen->setFont(_size);
      _pscreen->setFontSolid(false);
      _pscreen->gText(_xt, _yt, _textColour, _text);
    } 
    else {
      _pscreen->setPenSolid(true);
      _pscreen->rectangle(_x1, _y1, _x2, _y2, _lowColour); 
      delay(10);    // delay required by chipKIT
      _pscreen->setPenSolid(false);
      _pscreen->rectangle(_x1+2, _y1+2, _x2-2, _y2-2, _highColour); 

      _pscreen->setFontSolid(false);
      _pscreen->setFont(_size);
      _pscreen->gText(_xt, _yt, _textColour, _text);
    }
  }  
  else {
    _pscreen->setPenSolid(true);
    _pscreen->rectangle(_x1, _y1, _x2, _y2, _pscreen->rgb16(0x44, 0x44, 0x44)); 
    delay(10);    // delay required by chipKIT
    _pscreen->setPenSolid(false);
    _pscreen->rectangle(_x1+2, _y1+2, _x2-2, _y2-2, _textColour); 

    _pscreen->setFontSolid(false);
    _pscreen->setFont(_size);
    _pscreen->gText(_xt, _yt, _textColour, _text);
  }
}

void Button::enable(bool b1) {
  _enable = b1;
}

bool Button::check() {
  if (!_enable) return false;

  uint16_t x0, y0, z0;

  // down
  if ((_pscreen->getTouchActivity() == 1) || (_pscreen->getTouchActivity() == 3)) {
    _pscreen->getTouchXY(x0, y0);

    // pressed
    if ((x0>=_x1) && (x0<=_x2) && (y0>=_y1) && (y0<=_y2)) {
      draw(true); 

      // debounce
      do {   
        z0 = _pscreen->getTouchActivity();
        // Serial.pruint16_t ln(z0);
        delay(10); 
      }
      while (z0 != 2);

      // released
      delay(50);
      draw();        

      // up
      if (z0 == 2)  {
        _pscreen->getTouchXY(x0, y0);
        if ((x0>=_x1) && (x0<=_x2) && (y0>=_y1) && (y0<=_y2)) {
          return true;
        } 
      } // if up
    } // if pressed
  } // if down
  return false; 
}
