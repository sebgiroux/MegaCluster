//
// Î¼LCD-32PT(SGC) 3.2" Serial LCD Display Module
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

#ifndef Serial_LCD_h
#define Serial_LCD_h

#include "WProgram.h"
//#include "Arduino.h"
// #include <inttypes.h>
#include "proxySerial.h"

// Units

class Serial_LCD
{
public:
  Serial_LCD(ProxySerial * port0); // uint8_t receivePin, uint8_t transmitPin); // constructor

  // 2.1 General Commands
  void begin(int8_t resetPin0 = -1); // AutoBaud - 55hex 
  uint8_t setSpeed(uint16_t speed); // Set new Baud-Rate - 51hex 
  String WhoAmI(); // Version-Device Info Request - 56hex 
  // Replace Background Colour - 42hex 
  uint8_t clear(); // Clear Screen - 45hex
  uint8_t setBacklight(bool b);   // Display Control Functions - 59hex 
  uint8_t setDisplay(bool b);   // Display Control Functions - 59hex
  uint8_t setContrast(uint8_t b);   // Display Control Functions – 59hex
  uint8_t setOrientation(uint8_t b);   // Display Control Functions – 59hex
  uint8_t setTouch(bool b);   // Display Control Functions – 59hex
  uint8_t setVolume(uint8_t percent); // Set Volume - 76hex 
  // Sleep - 5Ahex 
  // Read GPIO Pin - 61hex
  // Write GPIO Pin - 69hex 
  // Read GPIO Bus - 79hex 
  // Write GPIO Bus - 57hex  
  void off(); // Highly recommended at the end

  // 2.2 Graphics Commands
  // Add User Bitmap Character - 41hex 
  // Draw User Bitmap Character - 44hex 
  uint8_t circle(uint16_t x1, uint16_t y1, uint16_t radius, uint16_t colour);  // Draw Circle - 43hex 
  uint8_t triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t colour);  // Draw Triangle – 47hex 
  // Draw Image-Icon - 49hex 
  uint8_t setBackGroundColour(uint16_t colour);   // Set Background colour - 4Bhex 
  uint8_t line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);  // Draw Line - 4Chex 
  // Draw Polygon - 67hex 
  uint8_t rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);  // Draw Rectangle - 72hex 
  uint8_t ellipse(uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, uint16_t colour); // Draw Ellipse - 65hex 
  uint8_t point(uint16_t x1, uint16_t y1, uint16_t colour);   // Draw Pixel - 50hex 
  // Read Pixel - 52hex 
  // Screen Copy-Paste - 63hex 
  // Replace colour - 6Bhex 
  uint8_t setPenSolid(bool b);    // Set Pen Size 1=solid; 0=wire frame - 70hex

  // 2.3 Text Commands
  uint8_t setFont(uint8_t b);  // Set Font - 46hex 
  uint8_t setFontSolid(uint8_t b);  // Set 0=Transparent-1=Opaque Text - 4Fhex 
  // Draw ASCII Character (text format) - 54hex 
  // Draw ASCII Character (graphics format) - 74hex 
  uint8_t tText(uint8_t x, uint8_t y, uint16_t colour, String s);  // Draw "String" of ASCII Text (text format) - 73hex 
  uint8_t gText(uint16_t x, uint16_t y, uint16_t colour, String s);    // Draw "String" of ASCII Text (graphics format) - 53hex 
  // Draw Text Button - 62hex

  // 2.4 Touch Screen Commands
  // Touch screen must be enabled to be able to use the touch commands. 
  uint8_t getTouchActivity();   // Get Touch Coordinates - 6Fhex - 0 : No Touch Activity 1 : Touch Press 2 : Touch Release 3 : Touch Moving
  uint8_t getTouchXY(uint16_t &x, uint16_t &y);   // Get Touch Coordinates - 6Fhex 
  // Wait Until Touch - 77hex 
  uint8_t detectTouchRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // Detect Touch Region - 75hex

  // 2.5 SD Memory Card Commands (Low-Level/RAW)
  // Initialise Memory Card - @69hex 
  // Set Address Pointer of Card (RAW) - @41hex 
  // Read uint8_t Data from Card (RAW) - @72hex 
  // Write uint8_t Data to Card (RAW) - @77hex 
  // Read Sector Block Data from Card (RAW) - @52hex 
  // Write Sector Block Data to Card (RAW) - @57hex 
  // Screen Copy-Save to Card (RAW) - @43hex 
  // Display Image-Icon from Card (RAW) - @49hex 
  // Display Object from Card (RAW) - @4Fhex 
  // Display Video-Animation Clip from Card (RAW) - @56hex 
  // Run Script (4DSL) Program from Card (RAW) - @50hex

  // 2011-06-29 release 3
  // 2.6 SD Memory Card Commands (FAT16-Level/DOS)
  uint8_t initSD();   // Initialise Memory Card - @69hex 
  uint8_t readTextFile(String filename, uint8_t bytes, void (*cbReadFile)(String text));  // Read File from Card (FAT) - @61hex 
  // Write File to Card (FAT) - @74hex 
  uint8_t writeString2File(String filename, String text, uint8_t option=0x00);   
  uint8_t appendString2File(String filename, String text);
  uint8_t eraseFile(String filename);   // Erase file from Card (FAT) - @65hex 
  // List Directory from Card (FAT) - @64hex 
  uint8_t findFile(String filename);
  uint8_t saveScreenSD(String filename);   // Screen Copy-Save to Card (FAT) - @63hex 
  uint8_t readScreenSD(String filename);   // Display Image-Icon from Card (FAT) - @6Dhex     
  uint8_t playSoundSD(String filename, uint8_t option0=0);  // Play Audio WAV file from Card (FAT) - @6Chex 
  // Run Script (4DSL) Program from Card (FAT) - @70hex

  // Utilities
  uint16_t rgb16(uint8_t red, uint8_t green, uint8_t blue);
  uint8_t nacAck(); // 0x06=success
  uint8_t fontX() {     return _fontX;   };	
  uint8_t fontY() {     return _fontY;   };	


private:
  ProxySerial * _port;

  bool _checkedSD;
  bool _serialSoftFlag;
  uint8_t _font, _fontX, _fontY;
  uint8_t _touch_buffer[4];
  uint8_t _orientation;

  void _swap(uint16_t &a, uint16_t &b);
};


#endif



















