/* Copyright 2011 David Irvine
 *
 * This file is part of Loguino
 *
 * Loguino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Loguino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Loguino.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Rev: 61 $
 * $Author: irvined $
 * $Date$

*/

#ifndef MegaSquirt_h
#define MegaSquirt_h

#define LAMBDA 14.7
#define MS_PORT Serial1
#define MS_TABLE_LENGTH 403
#define MS_MAX_RETRY_COUNT 5
#define MS_WAIT_TIME 120

#define MS_COMM_SUCCESS 0
#define MS_ERR_COMM_TIMEOUT 1
#define MS_ERR_COMM_OVERFLOW 2

#define MS_BAUD_RATE 115200

// Bluetooth stuff
#define USE_BLUETOOTH true
#define BLUETOOTH_BAUD_RATE 38400

#define BLUETOOTH_PIN_CODE_1 12896
#define BLUETOOTH_MS_ADDRESS_1 "12,6f,22fcd6"

#define BLUETOOTH_PIN_CODE_2 1234
#define BLUETOOTH_MS_ADDRESS_2 ""

#define BLUETOOTH_MAX_INQUERY_TIME 10000 // 10 seconds

#include "WProgram.h"

class MegaSquirtData {
  bool get_bit(byte b, int p);
  byte reg[MS_TABLE_LENGTH];

  public:  
    bool loadData(byte newReg[MS_TABLE_LENGTH]);

    uint16_t seconds();
    uint16_t pulseWidth1();
    uint16_t pulseWidth2();
    uint16_t rpm();
    int16_t advance();

    uint8_t squirt();
    bool firing1();
    bool firing2();
    bool sched1();
    bool inj1(); 
    bool sched2();
    bool inj2();
           
    uint8_t engine();
    bool ready();
    bool crank();
    bool startw();
    bool warmup(); 
    bool tpsaen();
    bool tpsden(); 
    bool mapaen();
    
    uint8_t afrtgt1();
    uint8_t afrtgt2();
    uint8_t wbo2_en1();
    uint8_t wbo2_en2();
  
    int16_t barometer();
    int16_t map();     
    int16_t mat();    
    int16_t coolant();
    int16_t tps();   
    int16_t batteryVoltage();
    int16_t afr1();          
    int16_t afr2();         
    int16_t lambda1();
    int16_t lambda2();

    int16_t knock();       
    int16_t egoCorrection1(); 
    int16_t egoCorrection();
    int16_t egoCorrection2();
    int16_t airCorrection();
    int16_t warmupEnrich();
  
    
    int16_t accelEnrich();
    int16_t tpsfuelcut();
    int16_t baroCorrection();
    int16_t gammaEnrich();
    int16_t veCurr1();
    int16_t veCurr2();
    int16_t veCurr();
    int16_t iacstep();
    int16_t coldAdvDeg();
    int16_t tpsDOT();
    int16_t mapDOT();
    int16_t rpmDOT();
    int16_t dwell();
    int16_t maf();
    int16_t fuelLoad();
    int16_t fuelCorrection();
    
    uint16_t portStatus();
    bool port0();
    bool port1();
    bool port2();
    bool port3();
    bool port4();
    bool port5();
    bool port6();
  
    
    uint8_t knockRetard();
    int16_t EAEFuelCorr1();
    int16_t egoV1();
    int16_t egoV2();
    int16_t amcUpdates();
    int16_t loopTime();
    int16_t spare1();
    int16_t spare2();
    int16_t trig_fix();
    int16_t spare4();
    int16_t spare5();
    int16_t spare6();
    int16_t spare7();
    int16_t spare8();
    int16_t spare9();
    int16_t spare10();
    int16_t fuelFlow();
    int16_t gear();
    int16_t speedo();
    uint16_t tachCount();
    uint8_t ospare();
    uint8_t cksum();
    uint32_t deltaT();
};

class MegaSquirt {
  public:
    static byte begin();
    static byte runCommand(byte cmd[], byte cmdLength, byte data[], uint16_t dataLength);
    static byte signature(String *sig);
    static byte seconds(uint16_t *secs);
    static byte revision(String *rev);
    static byte getData(byte table[MS_TABLE_LENGTH]);
};
    

#endif

