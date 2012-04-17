//
// Î¼LCD-32PT(SGC) 3.2â€� Serial LCD Display Module
// Arduino & chipKIT Library
//
// May 10, 2011 release 1 - initial release
// Jun 15, 2011 release 2 - features added and bugs fixed
// Jun 29, 2011 release 3 - setBackGroundColour added and SD card
// Jul 31, 2011 release 4 - stdint.h types for chipKIT compatibility
// Aug 04, 2011 release 5 - chipKIT compatibility with external proxySerial.h
// Aug 07, 2011 release 6 - playing sounds - up to 250 mA!
// Sep 18, 2011 release 7 - dialog window with up to 3 buttons
// Sep 23, 2011 release 8 - ms monitoring to avoid RX TX collapse
// Oct 10, 2011 release 9 - Stream.h class based i2cSerial library
// Oct 14, 2011 release 10 - ellipse and detectTouchRegion from sebgiroux
// Oct 24, 2011 release 11 - serial port managed in main only - setSpeed added - proxySerial still needed
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
#include "WProgram.h"
//#include "Arduino.h"

#include "proxySerial.h"
#include "Serial_LCD.h"



// Constructor
Serial_LCD::Serial_LCD(ProxySerial * port0) {
  _port=port0;
}


// Interface
// 2.1 General Commands
// AutoBaud - 55hex 
void Serial_LCD::begin(int8_t resetPin0) {
  // default speed = 9600
    
  // reset is required for Goldelox-based screens
  if (resetPin0 > -1) {
      delay(3000);
      pinMode(resetPin0, OUTPUT);
      digitalWrite(resetPin0, LOW);
      delay(10);
      digitalWrite(resetPin0, HIGH);
      delay(10);
  }

  // LCD 500 ms power-up
  // SD card 3000 ms power-up
  delay(500);
  _port->print('U');    // connect
  while (_port->read()!=0x06)  {     
    delay(100);  
  }

  _port->print('o');    // clear touch 
  _port->print((char)0x04);   // touch state
  //  _port->flush();
  delay(100);
  while (_port->available()) _port->read();

  setBacklight(true);  // backlight on
  setDisplay(true);  // display on
  setOrientation(3);
  clear();
  setFont(1);
  _checkedSD=false;  // SD not checked

}

uint8_t Serial_LCD::setSpeed(uint16_t speed) {
  uint8_t a = 0x06;
  if (speed == (uint16_t)19200) a = 0x08;
  else if (speed == (uint16_t)38400) a = 0x0a;  // max for Arduino
  else if (speed == (uint16_t)57600) a = 0x0c;
  else if (speed == (uint16_t)115200) a = 0x0d; // ok with chipKIT

  if (a != 0x06) {
    _port->print('Q');
    _port->print((uint8_t)a); 
    while (!_port->available());
    a=_port->read();
    
  }
  a=0x06;
  return a;
}


String Serial_LCD::WhoAmI() {  
  String s="Serial uLCD-32PT ";
  _port->print('V');
  _port->print((char)0x00);
  delay(10);

  while(_port->available()!=0) {
    s += String(_port->read(), HEX);
    s += " ";
  }
  return s;
}


uint8_t Serial_LCD::clear() {
  _port->print('E');
  return nacAck();
}

void Serial_LCD::off() {
  clear();

  _port->print('o');
  _port->print((char)0x04);   // state
  _port->flush();

  setBacklight(false);  // backlight off
  clear();
  setDisplay(false);  // display off

  _port->print('Q');    // reset to default speed
  _port->print((char)0x06);    // 
  delay(10);
}


uint8_t Serial_LCD::setBacklight(bool b) {
  _port->print('Y');
  _port->print((char)0x00);
  _port->print(b ? (char)0x01 : (char)0x00);
  return nacAck();
}   

uint8_t Serial_LCD::setDisplay(bool b) {
  _port->print('Y');
  _port->print((char)0x01);
  _port->print(b ? (char)0x01 : (char)0x00);
  return nacAck();
}

uint8_t Serial_LCD::setContrast(uint8_t b) {
  if (b<=0x0f) {
    _port->print('Y');
    _port->print((char)0x02);
    _port->print(b);

    return nacAck();
  } 
  else {
    return 0x15; 
  }
}


uint8_t Serial_LCD::setOrientation(uint8_t b) {   // Display Control Functions - 59hex
  _orientation=b;
  _port->print('Y');
  _port->print((char)0x04);
  _port->print(b);  // 
  return nacAck();
}


uint8_t Serial_LCD::setTouch(bool b) {
  if (b) {
    _port->print('Y');
    _port->print((char)0x05);
    _port->print((char)0x00);  // enable touch
    char c=nacAck();

    if (c=0x06) {
      _port->print('Y');
      _port->print((char)0x05);  // full screen active
      _port->print((char)0x00);
    }
  } 
  else {
    _port->print('Y');
    _port->print((char)0x05);  // disable touch
    _port->print((char)0x01);
  }
  return nacAck();
}

uint8_t Serial_LCD::setVolume(uint8_t percent) { // Set Volume - 76hex 
  uint8_t b=0x08 + percent;
  if (b>0x7f) b=0x7f;
  _port->print('v');
  _port->print(b);

  return nacAck();
}

// Graphics
uint8_t Serial_LCD::circle(uint16_t x1, uint16_t y1, uint16_t radius, uint16_t colour) {
  _port->print('C');

  _port->print(x1);
  _port->print(y1);
  _port->print(radius);
  _port->print(colour);

  return nacAck();
}


uint8_t Serial_LCD::rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour) {
  _port->print('r');

  _port->print(x1);
  _port->print(y1);
  _port->print(x2);
  _port->print(y2);
  _port->print(colour);

  return nacAck();
}  

uint8_t Serial_LCD::ellipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t colour) {
  _port->print('e');

  _port->print(x);
  _port->print(y);
  _port->print(rx);
  _port->print(ry);
  _port->print(colour);

  return nacAck();
}  

uint8_t Serial_LCD::line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour) {
  _port->print('L');

  _port->print(x1);
  _port->print(y1);
  _port->print(x2);
  _port->print(y2);
  _port->print(colour);

  return nacAck();
}  

// 2011-06-24 release 3 - setBackGroundColour added
uint8_t Serial_LCD::setBackGroundColour(uint16_t colour) { 
  _port->print('K');

  _port->print(colour);

  return nacAck();
}

uint8_t Serial_LCD::point(uint16_t x1, uint16_t y1, uint16_t colour) {
  _port->print('P');

  _port->print(x1);
  _port->print(y1);
  _port->print(colour);

  return nacAck();
}  


uint8_t Serial_LCD::triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t colour) {
  bool b=true;

  // Graham Scan + Andrew's Monotone Chain Algorithm
  // 1. Sort by ascending x

  while (b) {  // required x2 < x1 : x3 > x2 : y2 > y1 : y3 > y1
    b=false;
    if (!b && (x1>x2)) { 
      _swap(x1, x2);
      _swap(y1, y2);
      b=true; 
    }
    if (!b && (x2>x3)) { 
      _swap(x3, x2);
      _swap(y3, y2);
      b=true; 
    }
  }

  // Graham Scan + Andrew's Monotone Chain Algorithm
  // 2. Sort by ascending y
  while (b) {  // required x2 < x1 : x3 > x2 : y2 > y1 : y3 > y1
    if (!b && (y1>y2)) { 
      _swap(x1, x2);
      _swap(y1, y2);
      b=true; 
    }
    if (!b && (y3>y2)) { 
      _swap(x3, x2);
      _swap(y3, y2);
      b=true; 
    }
  }

  // Graham Scan + Andrew's Monotone Chain Algorithm
  // 3. check counter-clockwise, clockwise, collinear
  long l= (x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1);

  if (l==0)   return line(x1, y1, x3, y3, colour);

  if (l>0) {
    _swap(x1, x2);
    _swap(y1, y2);
  }
  l= (x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1);

  _port->print('G');

  _port->print(x1);
  _port->print(y1);
  _port->print(x2);
  _port->print(y2);
  _port->print(x3);
  _port->print(y3);
  _port->print(colour);

  return nacAck();
}  



uint8_t Serial_LCD::setPenSolid(bool b) {
  // 00hex : All graphics objects are drawn solid 
  // 01hex : All graphics objects are drawn wire-frame
  _port->print('p');
  _port->print(b ? (char)0x00 : (char)0x01);
  return nacAck();
}  


// Text
// 2011-06-15 release 2
//   bug fixed, break added!
uint8_t Serial_LCD::setFont(uint8_t b) {
  // 00hex : 6x8 (5x7 false) small size font set 
  // 01hex : 8x8 medium size font set 
  // 02hex : 8x12 large size font set
  // 03hex : 12x16 largest size font set
  _port->print('F');
  _port->print(b);
  _font=b;
  switch (b) {
  case 0:
    _fontX=6; 
    _fontY=8;
    break;
  case 1:
    _fontX=8; 
    _fontY=8;
    break;
  case 2:
    _fontX=8; 
    _fontY=12;
    break;
  case 3:
    _fontX=12; 
    _fontY=16;
  } 
  return nacAck();
}  

uint8_t Serial_LCD::setFontSolid(uint8_t b) {
  // 00hex : Transparent, objects behind text are visible. 
  // 01hex : Opaque, objects behind text blocked by background.
  _port->print('O');
  _port->print(b);
  return nacAck();
}  


uint8_t Serial_LCD::tText(uint8_t x, uint8_t y, uint16_t colour, String s) {
  _port->print('s');
  _port->print(x);     // in character units
  _port->print(y);
  _port->print(_font);
  _port->print(colour);
  _port->print(s);
  _port->print((char)0x00);
  return nacAck();
}


uint8_t Serial_LCD::gText(uint16_t x, uint16_t y, uint16_t colour, String s) {
  _port->print('S');
  _port->print(x);    // in graphic units
  _port->print(y);
  _port->print(_font);
  _port->print(colour);
  _port->print((char)0x01);   // multiplier
  _port->print((char)0x01);
  _port->print(s);
  _port->print((char)0x00);
  return nacAck();
}


// Touch
// 2011-06-15 release 2
//   +2 features
//   case 1: touch down added
//   return value 
//     0 : No Touch Activity 
//     1 : Touch Press 
//     2 : Touch Release 
//     3 : Touch Moving
uint8_t Serial_LCD::getTouchActivity() {
  _port->print('o');
  _port->print((char)0x04);   // state
  
  uint8_t i = 0; 
  
  while (_port->available() != 4);
  while (i < 4) {
    _touch_buffer[i] = _port->read();
     i++; 
  }
  
  switch (_touch_buffer[1]) {
    case 1:
    case 2:
    case 3:
      return _touch_buffer[1];
    default:
      return false;
  }
}

uint8_t Serial_LCD::getTouchXY(uint16_t &x, uint16_t &y) {
  _port->print('o');
  _port->print((char)0x05);   // coordinates

  uint8_t i = 0; 
  while (_port->available() != 4);
  while (i < 4) {
    _touch_buffer[i] = _port->read();
     i++; 
  }

  if (_touch_buffer[0]!=0x15) {
    x = (uint16_t)(_touch_buffer[0] << 8) + _touch_buffer[1];
    y = (uint16_t)(_touch_buffer[2] << 8) + _touch_buffer[3];
    return 0x06;
  } 
  else {
    return 0x15;
  }
}


// Oct 14, 2011 release 10 - ellipse and detectTouchRegion from sebgiroux
// filters only events 1=press and 3=move
uint8_t Serial_LCD::detectTouchRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  _port->print('u');

  _port->print(x1);
  _port->print(y1);
  _port->print(x2);
  _port->print(y2);

  return nacAck();
}

// 2011-06-29 release 3
// 2.6 SD Memory Card Commands (FAT16-Level/DOS)
// Initialise Memory Card - @69hex 
uint8_t Serial_LCD::initSD() {
  // SD card 3000 ms power-up
  delay(3000);
  _port->print('@');
  _port->print('i');   

  // answer = 210 ms
  delay(210);
  char a = nacAck();
  _checkedSD = (boolean)(a==0x06);

  Serial.print("SDinit \t");
  Serial.println(_checkedSD, DEC);

  return a;
}

/*
  Read File from Card (FAT) - @61hex 
  
  filename   - The filename of the file to read
  bytes      - The number of bytes to get each time (0=all, not a good idea when filesize > 512b)
  cbReadFile - Callback function that is called every time data is received so a parser do its job.
*/
uint8_t Serial_LCD::readTextFile(String filename, uint8_t bytes, void (*cbReadFile)(String text)) {  
  if (_checkedSD == 1) {
    _port->print('@');
    _port->print('a');
    _port->print(bytes);
    _port->print(filename);
    _port->print((char) 0x00);    
    
    uint8_t i = 0;
    char c = 0;
    String s = "";
    boolean done = false;
    boolean fileSizeRead = false;
    
    do {
      _port->print((char) 0x06);     
        
      i = 0;
      s = "";
      
      if (!fileSizeRead && _port->available() > 0) {
        uint8_t j = 0;
        while (j < 4) {
          _port->read();
          j++;
        }
        
        fileSizeRead = true;
      }
      else {
        do {
           c = _port->read();  
           if (c == 0x06) {
             done = true;
             break;
           }
           else {
             s = s + String(c);
             i++;
           }
        } while (_port->available() && i < bytes);
  
        if (fileSizeRead) cbReadFile(s);
      }
    } while (!done && c != 0x06);
    
    _port->flush();
    
    return nacAck();
  } 
  else { 
    return 0x15; 
  }
}

// Write File to Card (FAT) - @74hex 
// default option = 0
uint8_t Serial_LCD::writeString2File(String filename, String text, uint8_t option) { 
  String s;
  uint8_t a;
  uint8_t j;

  if (_checkedSD==1) {
    j=text.length() >>4;

    // 16-uint8_t blocks
    if (j>0) {
      _port->print('@');
      _port->print('t');
      _port->print((char)(0x10 + option)); // hand-shaking
      _port->print(filename);
      _port->print((char)0x00);
      _port->print((uint16_t)0x00);
      _port->print((uint16_t)(j <<4));
      a=nacAck();

      for (uint8_t i=0; i<j; i++) {
        s=text.substring(i <<4, (i+1) <<4);
        _port->print(s);
        a=nacAck();
      }
    }

    // remaining bytes
    j=text.length() % 0x10;

    if   (j > 0) {
      _port->print('@');
      _port->print('t');
      _port->print((uint8_t)(0x00 + option));   // no hand-shaking
      _port->print(filename);
      _port->print((char)0x00);
      _port->print((uint16_t)0x00);
      _port->print((uint16_t)(j));
      a=nacAck();

      s=text.substring(text.length()-j, text.length());
      _port->print(s);
      a=nacAck();
    }

    return a;
  } 
  else { 
    return 0x15; 
  }
}

uint8_t Serial_LCD::appendString2File(String filename, String text) { 
  return writeString2File(filename, text, 0x80);  // append option
}

// Erase file from Card (FAT) - @65hex 
uint8_t Serial_LCD::eraseFile(String filename) {  
  if (_checkedSD==1) {
    _port->print('@');
    _port->print('e');
    _port->print(filename);
    _port->print((char)0x00);

    return nacAck();
  } 
  else { 
    return 0x15; 
  }
}

// List Directory from Card (FAT) - @64hex
uint8_t Serial_LCD::findFile(String filename) {  
  if (_checkedSD==1) {
    _port->print('@');
    _port->print('d');
    _port->print(filename);
    _port->print((char)0x00);

    String s="";  
    char c=0;

    do {
      if (_port->available()) {
        c = _port->read();  
        s = s + String(c);
      }
    } while (c != 0x06 && c != 0x15 && c != 0x0a);
    
    _port->flush();

    if ((c==0x15) || (c==0x06)) return 0x15;
    if (s.length()==0) return 0x15;
    if (filename.equalsIgnoreCase(s.substring(0, (s.indexOf(c))))) return 0x06;
    
    return 0x15;
  }
  else { 
    return 0x15; 
  }
}


// Screen Copy-Save to Card (FAT) - @63hex 
uint8_t Serial_LCD::saveScreenSD(String filename) {    
  if (_checkedSD==1) {
    _port->print('@');
    _port->print('c');
    _port->print((uint16_t)0);
    _port->print((uint16_t)0);
    _port->print((uint16_t)319);
    _port->print((uint16_t)239);
    _port->print(filename);
    _port->print((char)0x00);

    return nacAck();
  }
  else { 
    return 0x15; 
  }
}

// Display Image-Icon from Card (FAT) - @6Dhex 
uint8_t Serial_LCD::readScreenSD(String filename) {   
  if (_checkedSD==1) {
    _port->print('@');
    _port->print('m');
    _port->print(filename);
    _port->print((char)0x00);
    _port->print((uint16_t)0x00);
    _port->print((uint16_t)0x00);
    _port->print((uint16_t)0x00);
    _port->print((uint16_t)0x00);

    return nacAck();
  }
  else { 
    return 0x15; 
  }
}

// Aug 07, 2011 release 6 - playing sounds - up to 250 mA!
// Play Audio WAV file from Card (FAT) - @6Chex 
uint8_t Serial_LCD::playSoundSD(String filename, uint8_t option0) {   
  if (_checkedSD==1) {
    _port->print((uint8_t)0x6c);
    _port->print((uint8_t)option0);
    _port->print(filename);
    _port->print((char)0x00);

    return nacAck();
  }
  else { 
    return 0x15; 
  }
}

// Run Script (4DSL) Program from Card (FAT) - @70hex


// Utilities


uint16_t Serial_LCD::rgb16(uint8_t red8, uint8_t green8, uint8_t blue8) {
  // rgb16 = red5 green6 blue5
  return (red8 >> 3) << 11 | (green8 >> 2) << 5 | (blue8 >> 3);
}


uint8_t Serial_LCD::nacAck() {
  uint8_t b=0x00; // 0x06;
  while (!(_port->available())) {     
    delay(2);   
  }
  b = _port->read();
  return b;
}


void Serial_LCD::_swap(uint16_t &a, uint16_t &b) {
  uint16_t w=a;
  a=b;
  b=w;
}
































