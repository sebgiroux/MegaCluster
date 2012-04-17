/* 
  Mega Cluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#include "Utils.h"

/*
  Function that take a float and return a String
  
  @param number - The number to convert
  @param precision - The precision needed for the float
*/

String Utils::ftoa(float number, uint8_t precision) {
  String s = "";

  // Negative 
  if (number < 0.0)  {
    s = "-";
    number = -number;
  }  

  double rounding = 0.5;
  for (uint8_t i = 0; i < precision; i++)    rounding /= 10.0;

  number += rounding;
  s += String(int16_t(number));  // prints the integer part

  if (precision > 0) {
    uint32_t frac;
    uint32_t mult = 1;
    while (precision--) {
      mult *= 10;
    }

    frac = (number - uint16_t(number)) * mult;

    if (frac > 0) {
      s += ".";       // prints the decimal point
      s += frac;      // prints the fractional part
    }
  }
  
  return s;
}

/*
  Function that return a gradiant from green to red depending of the percent specified

  @param 0 to 1 value reprensenting the percent
*/
uint16_t Utils::getGradiantGreenToRed(double power) {
  double h = power * 0.4; // Hue (0.4 = Green)
  double s = 0.9;         // Saturation
  double v = 0.9;         // Brightness

  double r, g, b;

  int i = int(h * 6);
  
  double f = h * 6 - i;
  double p = v * (1 - s);
  double q = v * (1 - f * s);
  double t = v * (1 - (1 - f) * s);

  switch (i % 6) {
    case 0: 
      r = v;
      g = t;
      b = p; 
      break;
    case 1: 
      r = q;
      g = v;
      b = p; 
      break;
    case 2: 
      r = p;
      g = v;
      b = t; 
      break;
    case 3:
      r = p;
      g = q;
      b = v; 
      break;
    case 4: 
      r = t;
      g = p;
      b = v; 
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }
  
  r = r * 255;
  g = g * 255;
  b = b * 255;
  
  return ((uint8_t) r >> 3) << 11 | ((uint8_t) g >> 2) << 5 | ((uint8_t) b >> 3);
}


/*
  Return true if coords(x,y) is in the rectangle (x,y,w,h), false otherwise
*/
boolean Utils::isInRect(uint8_t x,uint8_t y,uint8_t xR,uint8_t yR,uint8_t wR,uint8_t hR) {
  if (x >= xR && y >= yR && x <= xR+wR && y <= yR+hR) {
    return true; 
  }
  
  return false;
}


