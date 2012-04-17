//
// SC16IS750 I2C slave bridge to serial
// Arduino Library
//
// Oct 06, 2011 release 1 - initial release
// Oct 10, 2011 release 2 - Stream.h class based
// Oct 25, 2011 release 3 - I2C address selection through parameter default=0, 1, 2 or 12
//
//
// CC = BY NC SA
// http://sites.google.com/site/vilorei/
//

#ifndef I2C_Serial_h
#define I2C_Serial_h

#include "WProgram.h"
#include <Stream.h>
#include <Wire.h>

// I2C_Serial() or 
// I2C_Serial(0)  : _address=0x48; : a0/a1=+/+ : default I2C serial port
// I2C_Serial(1)  : _address=0x49; : a0/a1=-/+ : secondary I2C serial port
// I2C_Serial(2)  : _address=0x4c; : a0/a1=+/- : RFID ID-2 sensor board
// I2C_Serial(12) : _address=0x4d; : a0/a1=-/- : RFID ID-12 sensor board


class I2C_Serial : public Stream
{
public:
  I2C_Serial(); // constructor
  I2C_Serial(uint8_t b); // 2 for ID2 or 12 for ID12 or 0 or 1 otherwise
  String WhoAmI();
  void begin(long b=9600);
  boolean test();

  virtual void write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();

  virtual int peek(); // !
  int free();       // TX

private:
  int8_t _address;
};

#endif





