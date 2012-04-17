//
// SC16IS750 I2C slave bridge to serial
// Arduino + chipKIT Library
//
// Oct 06, 2011 release 1 - initial release
// Oct 10, 2011 release 2 - Stream.h class based
// Oct 25, 2011 release 3 - I2C address selection through parameter default=0, 1, 2 or 12
//
//
// CC = BY NC SA
// http://sites.google.com/site/vilorei/
//

#include "WProgram.h"
#include <Wire.h>
#include "I2C_Serial.h"
#include "Stream.h"

// ---------------- Functions

// Writes value to address register on device
static void _writeTo(uint8_t device, uint8_t address, uint8_t value) {
  Wire.beginTransmission(device); //start transmission to ACC 
  Wire.send(address);             // send register address
  Wire.send(value);               // send value to write
  Wire.endTransmission();         //end transmission
}


// Reads number uint8_ts starting from address register on device uint8_to buffer array
static void _readFrom(uint8_t device, uint8_t address, uint8_t number, uint8_t buffer[]) {
  Wire.beginTransmission(device); //start transmission to ACC 
  Wire.send(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to ACC
  Wire.requestFrom(device, number);    // request 6 uint8_ts from ACC

  uint8_t i = 0;
  while(Wire.available())    //ACC may send less than requested (abnormal)
  { 
    buffer[i] = Wire.receive(); // receive a uint8_t
    i++;
  }
  Wire.endTransmission(); //end transmission
}



static uint8_t _readByteFrom(int8_t device, uint8_t address) {
  Wire.beginTransmission(device); //start transmission to ACC 
  Wire.send(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to ACC
  Wire.requestFrom(device, 1);    // request 1 uint8_t from ACC

  while(!Wire.available());    //ACC may send less than requested (abnormal)
  uint8_t b = Wire.receive(); // receive a uint8_t
  Wire.endTransmission(); //end transmission
  return b;
}

// ---------------- Class

I2C_Serial::I2C_Serial() // constructor
// as per http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1250997678
{
  _address=0x48; 
}

I2C_Serial::I2C_Serial(uint8_t n) // constructor
{
  _address=0x48;                  // a0/a1=+/+ : default I2C serial port
  if (n==1) _address=0x49;        // a0/a1=-/+ : secondary I2C serial port
  else if (n==2) _address=0x4c;   // a0/a1=+/- : RFID ID-2 sensor board
  else if (n==12) _address=0x4d;  // a0/a1=-/- : RFID ID-12 sensor board
}

String I2C_Serial::WhoAmI() {
  String s="";
  if (_address < 0x10) s="0";
  s = s + String(_address, 0x10) +"h ";
  s = s + "SC16IS750 I2C Serial (actuator/sensor)";
  return s;
}


void I2C_Serial::begin(long b) {

  _writeTo(_address, 0x0e << 3, 0x01 << 3); // software reset

  // See datasheet section 7.8 for configuring the
  // "Programmable baud rate generator"
  uint16_t divisor = (uint16_t)(14745600/b/16);

  _writeTo(_address, 0x03 << 3, 0b10000000); // Line Control Register - divisor latch enable
  _writeTo(_address, 0x00 << 3, lowByte(divisor));  // Divisor Latch LSB
  _writeTo(_address, 0x01 << 3, highByte(divisor)); // Divisor Latch MSB


  //  _writeTo(_address, 0x03 << 3, 0xbf); // Line Control Register - access EFR register
  //  _writeTo(_address, 0x02 << 3, 0x00); // enable enhanced registers
  _writeTo(_address, 0x03 << 3, 0b00000011); // Line Control Register - 8 data bit, 1 stop bits, no parity
  _writeTo(_address, 0x02 << 3, 0x06); // reset TXFIFO, reset RXFIFO, non FIFO mode
  _writeTo(_address, 0x02 << 3, 0x01); // enable FIFO mode   
}


void I2C_Serial::write(uint8_t byte) {
  //  //                                   start I2C    
  //  Wire.beginTransmission(_address); // talk to device at _address
  //  Wire.send(0x00);                   // command 
  //  Wire.send(i);                     // value  
  //  Wire.endTransmission();           // end I2C  
  _writeTo(_address, 0x00 << 3, byte);
}


int I2C_Serial::read() {
  //  Wire.beginTransmission(_address); //start transmission to ACC 
  //  Wire.send(0x00);                   // command 
  //  Wire.endTransmission(); //end transmission
  //
  //    Wire.beginTransmission(_address); //start transmission to ACC
  //  Wire.requestFrom(_address, 1);    // request 6 uint8_ts from ACC
  //
  //  while (!Wire.available())    //ACC may send less than requested (abnormal)
  //    return  Wire.receive(); // receive a uint8_t
  //  Wire.endTransmission(); //end transmission

  return _readByteFrom(_address, 0x00 << 3);
}


boolean I2C_Serial::test() {
  char a = (char)random(0x00, 0xff);
  _writeTo(_address, 0x07 << 3, a); // Scratch Pad Register
  delay(3);
  char b=_readByteFrom(_address, 0x07 << 3); // Scratch Pad Register
  return (a==b);
}


int I2C_Serial::available() {
  return _readByteFrom(_address, 0x09 << 3); // Receiver FIFO Level register
}

int I2C_Serial::peek() {
  if (available()==0) return -1;
  else return 1; // ?
}

int I2C_Serial::free() {
  return _readByteFrom(_address, 0x08 << 3); // Transmitter FIFO Level register
}


void I2C_Serial::flush() {
  _writeTo(_address, 0x02 << 3, 0x06); // reset TXFIFO, reset RXFIFO, non FIFO mode
  _writeTo(_address, 0x02 << 3, 0x01); // enable FIFO mode   
}

