/* 
  MegaCluster
  By Sébastien Giroux
  
  Copyright - (c) 2011-2012
*/

#include "Serial_LCD.h"
#include "proxySerial.h"
#include "MegaSquirt.h"
#include "Gauge.h"
#include "Histogram.h"
#include "ProgressBar.h"
#include "BarGraph.h"
#include "DataDisplay.h"
#include <Wire.h>
#include "I2C_Serial.h"
#include "Button.h"

I2C_Serial myI2C;
ProxySerial mySerial(&myI2C);
Serial_LCD myLCD(&mySerial); 

Gauge Gauge(&myLCD);
BarGraph BarGraph(&myLCD);
ProgressBar ProgressBar(&myLCD);
Histogram Histogram(&myLCD);
DataDisplay DataDisplay(&myLCD);

Button btnStartDatalogging(&myLCD);

const uint8_t MAX_PAGE = 5;
uint8_t currentPage = 1;
uint16_t x, y, oldX, oldY;
uint8_t currentTouchActivity;

boolean pageAlreadyChanged = false;
boolean deviceReady = false;
boolean lastECUOnline = false;
int lastBatteryPercent = 0;
boolean ecuOnline = false;
boolean forceRefreshAfterPageChange = false;
boolean dataloggingActive = false;

MegaSquirtData msDataObject;
byte msDataTable[MS_TABLE_LENGTH];

#define SHOW_BATTERY_PERCENT false
#define DEMO_MODE false

const uint8_t batteryPin = 0; // +V from battery is connected to analog pin 0
const float batteryLowVoltage = 2.3;
const float batteryFullChargeVoltage = 5.0;

long loopTimeBegin = 0;
long loopTimeEnd = 0;

void setup() {  
  Serial.begin(19200);
  Serial.print("\n\n\n***\n");

  Wire.begin();  
  myI2C.begin(9600);
  
  myLCD.begin();
  delay(10);

  myLCD.setSpeed(57600); // 115200
  myI2C.begin(57600);

  myLCD.setTouch(true);
  
  /*
    Splash Screen
  */
  
  myLCD.initSD();
  myLCD.readScreenSD("splash.gci");
  
  myLCD.setFont(0x03);
  myLCD.gText(12,12,0,"MegaCluster");
  myLCD.gText(10,10,0xffffff,"MegaCluster");
  
  myLCD.setFont(0x02);
  myLCD.gText(12,30,0,"By Sebastien Giroux");  
  myLCD.gText(10,28,0xffffff,"By Sebastien Giroux");  
  
  myLCD.gText(32,192,0,"Display for the Megasquirt-3 ECU");
  myLCD.gText(30,190,0xffffff,"Display for the Megasquirt-3 ECU");
  
  myLCD.setFont(0x01);
  myLCD.gText(12,222,0,"Copyright - 2012");
  myLCD.gText(10,220,0xffffff,"Copyright - 2012");
  myLCD.gText(162,222,0,"All Rights Reserved");
  myLCD.gText(160,220,0xffffff,"All Rights Reserved");
  
  // Begin of Megasquirt Comm
  MegaSquirt::begin();

  //currentPage = 4;
  //myLCD.readTextFile("ms.ini",10,callbackReadIni);
   
  renderPage();
  
  megasquirtCheckOnline();
  
  deviceReady = true; 

  Serial.print("\n ");
}

void callbackReadIni(String text) {
  //Serial.print(text);
}

/*
  Megasquirt Communication
*/
void megasquirtCheckOnline() {
  String sig;

  // Megasquirt is online, update global variable if it wasn't before
  if (MegaSquirt::signature(&sig) == MS_COMM_SUCCESS) {
    if (!ecuOnline) {
      ecuOnline = true;
    }
  }  
  else {
    if (ecuOnline) {
      ecuOnline = false;
    } 
  }
}

void megasquirtUpdateStatus() { 
  myLCD.setPenSolid(true); 
  myLCD.setFont(0x00);
  
  String _status = "";
  
  // Megasquirt is currently connected to the device
  if (ecuOnline) {
    myLCD.rectangle(10,220,310,235,0);
    myLCD.line(10,220,310,220,52825);
     
    _status = "Megasquirt connected, currently online";
  }
  // No Megasquirt device connected, display message
  else {    
    myLCD.rectangle(10,220,310,235,0);
    myLCD.line(10,220,310,220,52825);
    
    _status = "No Megasquirt connected, currently offline";
  }
  
   if (SHOW_BATTERY_PERCENT) {
     _status += String(getBatteryPercent()) + "%";
   }
    
   myLCD.gText(14,226,0xffffff,_status);
}

int getBatteryPercent() {
  float batteryPackVoltage = analogRead(batteryPin) * 0.0049;
  return (batteryPackVoltage - batteryLowVoltage) / (batteryFullChargeVoltage - batteryLowVoltage) * 100;
}

void pageOneGauges() {  
  /*
    Draw gauges
  */
  Gauge.drawGauge(0,55,55,50,-225,70,10,20,"AFR",""); 
  Gauge.drawGauge(1,160,55,50,-225,70,0,250,"MAP","kPa");
  Gauge.drawGauge(2,265,55,50,-225,70,0,10000,"RPM","");
  Gauge.drawGauge(3,55,160,50,-225,70,0,30,"PW","ms"); 
  Gauge.drawGauge(4,160,160,50,-225,70,0,250,"CLT","F");
  Gauge.drawGauge(5,265,160,50,-225,70,0,100,"TPS","%");
}

void pageTwoDash() {    
  /*
    Draw progress bars
  */
  ProgressBar.drawProgressBar(0,10,75,80,14,0,250,"MAP"); 
  ProgressBar.drawProgressBar(1,10,113,80,14,0,100,"TPS"); 
  ProgressBar.drawProgressBar(2,10,151,80,14,-40,250,"CLT"); 
  ProgressBar.drawProgressBar(3,10,189,80,14,-40,200,"MAT");
  
  ProgressBar.drawProgressBar(4,120,151,80,14,0,50,"PW"); 
  ProgressBar.drawProgressBar(5,120,189,80,14,0,150,"IAC Steps");   
  
  ProgressBar.drawProgressBar(6,230,75,80,14,0,50,"MPG"); 
  ProgressBar.drawProgressBar(7,230,113,80,14,0,50,"Spk. Adv."); 
  ProgressBar.drawProgressBar(8,230,151,80,14,10,20,"AFR"); 
  ProgressBar.drawProgressBar(9,230,189,80,14,6,15,"Volts");
 
  /*
    Draw Tachometer Bar Graph
  */    
  BarGraph.drawBarGraph(0,8,0,310,70);
}

void pageThreeHistogram() {  
  myLCD.setFont(0x02);
  myLCD.gText(5,5,0xffffff,"AFR Histogram");
  
  Histogram.drawHistogram(20,25,290,185,10,20);
}

void pageFourAllData() {
  /*
    Draw data display
  */
  DataDisplay.drawDataDisplay(0,2850,"","RPM");
  DataDisplay.drawDataDisplay(1,56,"%","TPS");
  DataDisplay.drawDataDisplay(2,76,"kPa","MAP");
  DataDisplay.drawDataDisplay(3,12.8,"","AFR");
  DataDisplay.drawDataDisplay(4,13.2,"","AFR Target");
  
  DataDisplay.drawDataDisplay(5,3.8,"ms","PW");
  DataDisplay.drawDataDisplay(6,22,"%","Inc.DC");
  DataDisplay.drawDataDisplay(7,76,"kPa","MAP");
  DataDisplay.drawDataDisplay(8,100,"%","WUE");
  DataDisplay.drawDataDisplay(9,16,"","IAC Steps");

  DataDisplay.drawDataDisplay(10,13.6,"ms","Volts");
  DataDisplay.drawDataDisplay(11,80,"F","MAT");
  DataDisplay.drawDataDisplay(12,192,"F","CLT");
  DataDisplay.drawDataDisplay(13,98,"%","EGO");
  DataDisplay.drawDataDisplay(14,23.4,"ms","Loop Time");
  
  DataDisplay.drawDataDisplay(15,3,"","TPSDot");
  DataDisplay.drawDataDisplay(16,300,"","RPMDot");
  DataDisplay.drawDataDisplay(17,5,"","MAPDot");
  DataDisplay.drawDataDisplay(18,2,"","Gear");
  DataDisplay.drawDataDisplay(19,36,"KM/H","VSS");
  
  DataDisplay.drawDataDisplay(20,108,"%","Gair");
  DataDisplay.drawDataDisplay(21,96,"%","Gbaro");
  DataDisplay.drawDataDisplay(22,102,"%","Gwarm");
  DataDisplay.drawDataDisplay(23,2.4,"ms","Dwell");
  DataDisplay.drawDataDisplay(24,18,"","Current");
 
  DataDisplay.drawDataDisplay(25,28.6,"","MPG");
}

void pageFiveDatalogging() {
  myLCD.setFont(0x03);
  myLCD.gText(10,10,0xffffff,"Datalogging on SD Card");
  
  myLCD.setFont(0x02);
  myLCD.gText(10,35,0xffffff,"Touch the button to start datalogging");
  myLCD.gText(10,50,0xffffff,"on the SD Card. When you are done,");
  myLCD.gText(10,65,0xffffff,"either touch the button again or");
  myLCD.gText(10,80,0xffffff,"disconnect the device.");
  
  dataloggingDrawStartButton();
  dataloggingUpdateStatus();
}

void dataloggingDrawStartButton() {
  btnStartDatalogging.define(230,110,80,50,"Start",myLCD.rgb16(0xff, 0xff, 0xff),myLCD.rgb16(0x00,0x9C,0xEB),myLCD.rgb16(0x4E,0x00,0xEB),9);
  
  btnStartDatalogging.enable(true);
  btnStartDatalogging.draw();
}

void dataloggingDrawStopButton() {
  btnStartDatalogging.define(230,110,80,50,"Stop",myLCD.rgb16(0xff, 0xff, 0xff),myLCD.rgb16(0xff, 0x00, 0x00),myLCD.rgb16(0x88, 0x00, 0x00),9);
  
  btnStartDatalogging.enable(true);
  btnStartDatalogging.draw();
}

void dataloggingUpdateStatus() {
  myLCD.setPenSolid(true);  
  myLCD.setFont(0x00);
  if (dataloggingActive) {
    myLCD.rectangle(10,195,310,210,52825);
      
    myLCD.gText(20,201,0xffffff,"Currently datalogging!"); 
    myLCD.gText(19,200,0,"Currently datalogging!");
  }
  else {
    myLCD.rectangle(10,195,310,210,myLCD.rgb16(200,0,0));    
    
    myLCD.gText(20,201,0,"Currently not datalogging.");
    myLCD.gText(19,200,0xffffff,"Currently not datalogging.");
  }
}

void renderPage() {
  myLCD.clear();
  
  switch (currentPage) {
    case 1:
      pageOneGauges();
      break;
    case 2:
      pageTwoDash();
      break;
    case 3:
      pageThreeHistogram();
      break;  
    case 4:
      pageFourAllData();
      break;
    case 5:
      pageFiveDatalogging();
      break;
  }
  
  megasquirtUpdateStatus();
  
  forceRefreshAfterPageChange = true;
}

void loop() {     
  loopTimeBegin = millis();
  currentTouchActivity = myLCD.getTouchActivity();

  if (currentTouchActivity) { 
    // When touch is released, no page has been changed yet    
    if (currentTouchActivity == 2) {
      pageAlreadyChanged = false;
    }
    
    // Don't change the page twice in the same touch event
    if (!pageAlreadyChanged) {
      currentTouchActivity = myLCD.getTouchXY(x, y);

      if (x != oldX && y != oldY) {
        Serial.print(x,DEC);
        Serial.print("\t");
        Serial.print(y,DEC);      
        Serial.println();
        
        if (x > 200 && currentPage < MAX_PAGE) {
          pageAlreadyChanged = true;
          currentPage++;
          renderPage();
        }
        else if (x < 120 && currentPage >= 2) {
          pageAlreadyChanged = true;
          
          currentPage--;
          renderPage();  
        }
        
        oldX = x;
        oldY = y;
      }
    }
  }

  if (deviceReady) {
    refreshCurrentPage();
  }  

  loopTimeEnd = millis();

  if ((SHOW_BATTERY_PERCENT && lastBatteryPercent != getBatteryPercent()) || lastECUOnline != ecuOnline) {
    lastBatteryPercent = getBatteryPercent();
    lastECUOnline = ecuOnline;
    
    megasquirtUpdateStatus();
  } 
  
}

void refreshCurrentPage() {
  if (currentPage == 1) { 
    float afr;
    float mapKPa;
    uint16_t rpm;
    float pulseWidth;
    float coolant;
    float tps;
    byte _status = MegaSquirt::getData(msDataTable);
    
    // Megasquirt is online
    if (_status == MS_COMM_SUCCESS) {
      // Megasquirt just got connected, update status
      if (!ecuOnline) {
        megasquirtCheckOnline();
      }
      
      msDataObject.loadData(msDataTable);
      
      afr = (float) msDataObject.afr1() / 10;
      mapKPa = msDataObject.map() / 10;
      rpm = msDataObject.rpm();
      pulseWidth = msDataObject.pulseWidth1() * 0.001;
      coolant = (float) msDataObject.coolant() / 10;
      tps = (float) msDataObject.tps() / 10;
    }
    else {
      if (DEMO_MODE) {
        afr = (float) rand() / (float) RAND_MAX + random(10,20);
        mapKPa = random(0,250);
        rpm = random(0,8000);
        pulseWidth = random(0,20);
        coolant = random(0,250);
        tps = random(0,100);
      }
      else {
        afr = 10;
        mapKPa = 0;
        rpm = 0;
        pulseWidth = 0;
        coolant = 0;
        tps = 0;
      }
    }

    Gauge.drawNeedle(0,afr,forceRefreshAfterPageChange);
    Gauge.drawNeedle(1,mapKPa,forceRefreshAfterPageChange);
    Gauge.drawNeedle(2,rpm,forceRefreshAfterPageChange);
    Gauge.drawNeedle(3,pulseWidth,forceRefreshAfterPageChange);
    Gauge.drawNeedle(4,coolant,forceRefreshAfterPageChange);
    Gauge.drawNeedle(5,tps,forceRefreshAfterPageChange);
  }
  else if (currentPage == 2) {
    byte _status = MegaSquirt::getData(msDataTable);
  
    uint16_t rpm;
    uint16_t speedo;
    float mapKPa;
    float tps;
    float coolant;
    float mat;
    float pulseWidth;
    uint16_t iacSteps;
    float MPG;
    float sparkAdvance;
    float afr;
    float batteryVoltage; 
  
    if (_status == MS_COMM_SUCCESS) {
      // Megasquirt just got connected, update status
      if (!ecuOnline) {
        megasquirtCheckOnline();
      }
      
      msDataObject.loadData(msDataTable);

      rpm = msDataObject.rpm();
      
      speedo = msDataObject.speedo();
      uint16_t VSSreal = speedo;
      speedo *= 0.36;
      
      mapKPa = (float) msDataObject.map() / 10;
      tps = (float) msDataObject.tps() / 10;
      coolant = (float) msDataObject.coolant() / 10;
      mat = (float) msDataObject.mat() / 10;
      pulseWidth = msDataObject.pulseWidth1() * 0.001;
      iacSteps = msDataObject.iacstep();
      
      MPG = 0;
      if (msDataObject.fuelFlow() > 0 && VSSreal > 0) {
        MPG = (2.361 / (msDataObject.fuelFlow() / (VSSreal * 6)));
      } 
      
      sparkAdvance = (float) msDataObject.advance() / 10;
      afr = (float) msDataObject.afr1() / 10;
      batteryVoltage = (float) msDataObject.batteryVoltage() / 10;
    }
    else {
      if (DEMO_MODE) {
        rpm = random(0,8000);
        speedo = random(0,300);
        mapKPa = random(0,250);
        tps = random(0,100);
        coolant = random(-40,250);
        mat = random(-40,200);
        pulseWidth = random(0,50);
        iacSteps = random(0,150);
        MPG = random(0,50);
        sparkAdvance = random(0,50);
        afr = random(10,20);
        batteryVoltage = random(6,15); 
      }
      else {
        rpm = 0;
        speedo = 0;
        mapKPa = 0;
        tps = 0;
        coolant = -40;
        mat = -40;
        pulseWidth = 0;
        iacSteps = 0;
        MPG = 0;
        sparkAdvance = 0;
        afr = 10;
        batteryVoltage = 6;
      }
      
      megasquirtCheckOnline();
    }
    
    float r = (float) rpm / 8000;      
      
    String rpmText = (String) rpm;
    String speedText = (String) speedo;

    myLCD.setPenSolid(true); 
 
    // Update tachometer text   
    myLCD.setFont(0x03);
    myLCD.rectangle(120,90,220,105,0);
    myLCD.gText(120 + ((4 - rpmText.length()) * 12),90,0xffffff,rpmText);    
    myLCD.gText(168,90,0xffffff,"RPM");
    
    // Update tachometer bar graph
    BarGraph.updateValue(0,r);
    
    // Update speedometer text
    myLCD.rectangle(120,130,220,145,0);
    myLCD.gText(120 + ((3 - speedText.length()) * 12),130,0xffffff,speedText);
    myLCD.gText(156,130,0xffffff,"KM/H");
    
    // Update progress bars
    ProgressBar.updateValue(0,mapKPa,forceRefreshAfterPageChange);
    ProgressBar.updateValue(1,tps,forceRefreshAfterPageChange);
    ProgressBar.updateValue(2,coolant,forceRefreshAfterPageChange);
    ProgressBar.updateValue(3,mat,forceRefreshAfterPageChange);
    ProgressBar.updateValue(4,pulseWidth,forceRefreshAfterPageChange);
    ProgressBar.updateValue(5,iacSteps,forceRefreshAfterPageChange);
    ProgressBar.updateValue(6,MPG,forceRefreshAfterPageChange);
    ProgressBar.updateValue(7,sparkAdvance,forceRefreshAfterPageChange);
    ProgressBar.updateValue(8,afr,forceRefreshAfterPageChange);
    ProgressBar.updateValue(9,batteryVoltage,forceRefreshAfterPageChange);
  }
  else if (currentPage == 3) {
    float afr;
    
    byte _status = MegaSquirt::getData(msDataTable);
  
    if (_status == MS_COMM_SUCCESS) {
      // Megasquirt just got connected, update status
      if (!ecuOnline) {
        megasquirtCheckOnline();
      }
      
      msDataObject.loadData(msDataTable);
      afr = (float) msDataObject.afr1() / 10;
      
      Histogram.addValue(afr);
    }
    else {
      if (DEMO_MODE) { 
        afr = random(10,20) + (float) rand() / (float) RAND_MAX;
      }
      else {
        afr = 14.7;
      }
      
      megasquirtCheckOnline();
    }

    Histogram.addValue(afr);
  }
  else if (currentPage == 4) {
    uint16_t rpm;
    float tps;
    float mapKPa;
    float afr;
    float afrTarget;
    float pulseWidth;
    float injectorDC;
    uint8_t warmupEnrichment;
    uint16_t iacSteps;
    float batteryVoltage;     
    float mat;
    float coolant;
    uint8_t ego;
    float loopTime;
    uint16_t tpsDOT;
    uint16_t rpmDOT;
    uint16_t mapDOT;
    uint8_t gear;
    uint16_t speedo;
    float gair;
    float gbaro;
    float gwarm;
    float dwell;
    float current;
    float MPG;
    
    byte _status = MegaSquirt::getData(msDataTable);
  
    if (_status == MS_COMM_SUCCESS) {
      // Megasquirt just got connected, update status
      if (!ecuOnline) {
        megasquirtCheckOnline();
      }
      
      msDataObject.loadData(msDataTable);
      
      rpm = msDataObject.rpm();
      tps = (float) msDataObject.tps() / 10;
      mapKPa = (float) msDataObject.map() / 10;
      afr = (float) msDataObject.afr1() / 10;
      afrTarget = (float) msDataObject.afrtgt1() / 10;
      pulseWidth = msDataObject.pulseWidth1() * 0.001;
          
      float cycleTime = 0;
      if (rpm > 0) cycleTime = 60000.0 / rpm * 2.0;
 
      injectorDC = 0;
      if (cycleTime > 0) {
          injectorDC = 100.0 * 1 * pulseWidth / cycleTime; // 1.5 = ((6 / (2 * 1.5)) / 2)
      }

      warmupEnrichment = msDataObject.warmupEnrich();
      iacSteps = msDataObject.iacstep();
      batteryVoltage = (float) msDataObject.batteryVoltage() / 10;     
      mat = (float) msDataObject.mat() / 10;
      coolant = (float) msDataObject.coolant() / 10;
      ego = (float) msDataObject.egoCorrection() / 10;
      loopTime = (float) msDataObject.loopTime() / 10;
      tpsDOT = msDataObject.tpsDOT();
      rpmDOT = msDataObject.rpmDOT();
      mapDOT = msDataObject.mapDOT();
      gear = msDataObject.gear();
      
      speedo = msDataObject.speedo();
      uint16_t VSSreal = speedo;
      speedo *= 0.36;
      
      gair = (float) msDataObject.airCorrection() / 10;
      gbaro = (float) msDataObject.baroCorrection() / 10;
      gwarm = random(80,120);
      dwell = (float) msDataObject.dwell() / 10;
      current = (float) msDataObject.veCurr() / 10;
      
      MPG = 0;
      if (msDataObject.fuelFlow() > 0 && VSSreal > 0) {
        MPG = (2.361 / (msDataObject.fuelFlow() / (VSSreal * 6)));
      } 
    }
    else {
      if (DEMO_MODE) {
        rpm = random(0,8000);
        tps = random(0,100);
        mapKPa = random(0,250);
        afr = random(10,20);
        afrTarget = random(10,20);
        
        pulseWidth = random(0,30);
        injectorDC = random(0,100);
        warmupEnrichment = random(100,200);
        iacSteps = random(0,100);
        
        batteryVoltage = random(6,14);         
        mat = random(-40,250);
        coolant = random(-40,250);
        ego = random(0,200);
        loopTime = random(15,30);
        
        tpsDOT = random(0,500);
        rpmDOT = random(0,500);
        mapDOT = random(0,100);
        gear = random(-1,5);
        speedo = random(0,300);
        
        gair = random(80,120);
        gbaro = random(80,120);
        gwarm = random(80,120);
        dwell = random(1,10);
        current = random(0,200);
        
        MPG = random(25,35);     
      }
      else {
        rpm = 0;
        tps = 0;
        mapKPa = 0;
        afr = 10;
        afrTarget = 10;
        
        pulseWidth = 0;
        injectorDC = 0;
        warmupEnrichment = 100;
        iacSteps = 0;
        
        batteryVoltage = 6;
        mat = -40;
        coolant = -40;
        ego = 0;
        loopTime = 15;
        
        tpsDOT = 0;
        rpmDOT = 0;
        mapDOT = 0;
        gear = 0;
        speedo = 0;
        
        gair = 100;
        gbaro = 100;
        gwarm = 100;
        dwell = 0;
        current = 0;
        
        MPG = 0;
      }   
      
      megasquirtCheckOnline();
    }

    DataDisplay.updateValue(0,rpm,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(1,tps,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(2,mapKPa,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(3,afr,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(4,afrTarget,forceRefreshAfterPageChange);  
        
    DataDisplay.updateValue(5,pulseWidth,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(6,injectorDC,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(7,mapKPa,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(8,warmupEnrichment,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(9,iacSteps,forceRefreshAfterPageChange);  
    
    DataDisplay.updateValue(10,batteryVoltage,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(11,mat,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(12,coolant,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(13,ego,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(14,loopTime,forceRefreshAfterPageChange);  
    
    DataDisplay.updateValue(15,tpsDOT,forceRefreshAfterPageChange);  
    DataDisplay.updateValue(16,rpmDOT,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(17,mapDOT,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(18,gear,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(19,speedo,forceRefreshAfterPageChange); 
    
    DataDisplay.updateValue(20,gair,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(21,gbaro,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(22,gwarm,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(23,dwell,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(24,current,forceRefreshAfterPageChange); 
    DataDisplay.updateValue(25,MPG,forceRefreshAfterPageChange);
  }  
  else if (currentPage == 5) {
    if (btnStartDatalogging.check()) {
      if (dataloggingActive) {
        dataloggingDrawStartButton();
      }
      else {
        dataloggingDrawStopButton();
      }
      
      dataloggingActive = !dataloggingActive;
      
      dataloggingUpdateStatus();
    }
  }
  
  forceRefreshAfterPageChange = false;
}












