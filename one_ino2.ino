#include <glcd.h>
#include "fonts/allFonts.h"         // Fonts here
#include <stdio.h>
#include <DS1302.h>
#include <EEPROM.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* день 1
добавить:
-прервывание при нажатии кнопок.
-добавить  alarm
*/
#define alarmMinute_EERPROM_ADDRESS 0
#define alarmHour_EERPROM_ADDRESS 1
#define alarmEnable_EERPROM_ADDRESS 2

#define interval_draw 10000
unsigned long lastTime = 0;

const int leafingMenuPin = 2; // рыжий
const int changeValuePin = 3; // красный
const int kCePin   = 13;  // Chip Enable
const int kIoPin   = 12;  // Input/Output
const int kSclkPin = 11;  // Serial Clock
int menuState = 200;
//Time t(2015, 03, 28, 21, 50, 50, Time::kSunday); // Set time 
volatile int value = 0; // для прерывания

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(0,INPUT);
  pinMode(1,INPUT);
  GLCD.Init();
  GLCD.SelectFont(fixednums8x16, BLACK);   //Font
  rtc.writeProtect(false);
  rtc.halt(false);

  // Set the time and date on the chip.
  //rtc.time(t);
}


void loop()
{
  unsigned long nowTime = millis();
  //menuState = 200;
  if(nowTime - lastTime > interval_draw && (menuState >= 200)){
     lastTime = nowTime;
     GLCD.ClearScreen();
     menuState = random(211,215);
     //if (menuState == 216) menuState = 200;
  }else if((nowTime  > (lastTime + 2000)) && (menuState > 200)) {
    menuState = 201;
  }
/*
  if (buttonState(leafingMenuPin) && (menuState >= 200)){
    menuState = 101;
    GLCD.ClearScreen(); 
   
  }
*/  
  draw(menuState);
  
  Serial.println(menuState);
}

void draw( int menuState_){
  switch(menuState_){
    case 200:
      digitalClockDisplay();
      break;
    case 201:
      GLCD.ClearScreen();
      digitalClockDisplay();
      menuState = 200;
      break;
   case 101:
      setAlarmHour();
      break;  
  
    case 102:
      setAlarmMinute();
      break;  
    
    case 103:
      menuSetHour();
      break;
      
    case 104:
      menuSetMinute();
      break;  
  
    case 105:
      menuSetSecond();
      break;  
    
    case 106:
      menuSetDay();
      break;  
    
    case 107:
      menuSetMon();
     break;  
    
    case 108:
      menuSetYear();
      break;  
    
    case 211:
      drawSmileFaceWithOpenEyes();
      break;
  
    case 212:
      drawSmileFaceWithCloseEyes();
      break;    
    
    case 213:
      drawPockerFaceWithCloseEyes();
      break;  
  
    case 214:
      drawPockerFace();
      break;
      
    case 215:
      drawSurprisedFace();
      
      break;
  }
}

int buttonState (int button){
  if (digitalRead(button) == LOW)   {
    delay(20);
    if (digitalRead(button) == LOW) {
      return 1;
    }else{ 
      return 0;
    }
  } else  {
  return 0; }
}

void setAlarmHour()
{
  Time m(m.yr, m.mon, m.date, m.hr, m.min, m.sec, Time::kSunday);
  GLCD.CursorToXY(25,15);
  GLCD.SelectFont(fixednums8x16, WHITE);
  GLCD.print(m.hr);
  GLCD.SelectFont(fixednums8x16, BLACK);
  printDigits(m.min);
      for(;buttonState(changeValuePin);m.hr++){
        if(m.hr>23) m.hr=0;
        delay(200);
      }
  EEPROM.write(alarmHour_EERPROM_ADDRESS,m.hr);
}  

void setAlarmMinute()
{
  Time m(m.yr, m.mon, m.date, m.hr, m.min, m.sec, Time::kSunday);
  GLCD.CursorToXY(25,15);
  GLCD.print(m.hr);
  GLCD.SelectFont(fixednums8x16, WHITE);
  printDigits(m.min);
  GLCD.SelectFont(fixednums8x16, BLACK);
      for(;buttonState(changeValuePin);m.min++){
        if(m.min>59) m.min=0;
        delay(200);
        //Time m(m.yr, m.mon, m.date, m.hr, m.min, m.sec, Time::kSunday);
      }
  EEPROM.write(alarmMinute_EERPROM_ADDRESS,m.min);
}  

void menuSetYear()
{
  Time t = rtc.time();
  GLCD.CursorToXY(20,35);
  GLCD.print(t.date);
  printDateTime(t.mon);
  GLCD.SelectFont(fixednums8x16, WHITE);
  printDateTime(t.yr);
  GLCD.SelectFont(fixednums8x16, BLACK);
      for(;buttonState(changeValuePin);t.yr++){
        if(t.yr>2024) t.yr=2014;
        delay(200);
        Time t(t.yr, t.mon, t.date, t.hr, t.min, t.sec, Time::kSunday); // Set time
      }
  rtc.time(t);
}


void menuSetMon()
{
  Time t = rtc.time();
  GLCD.CursorToXY(20,35);
  GLCD.print(t.date);
  GLCD.SelectFont(fixednums8x16, WHITE);
  printDateTime(t.mon);
  GLCD.SelectFont(fixednums8x16, BLACK);
  printDateTime(t.yr);
      for(;buttonState(changeValuePin);t.mon++){
        if(t.mon>11) t.mon=1;
        delay(200);
        Time t(t.yr, t.mon, t.date, t.hr, t.min, t.sec, Time::kSunday); // Set time
      }
  rtc.time(t);
}

void menuSetDay()
{
  Time t = rtc.time();
  GLCD.CursorToXY(20,35);
  GLCD.SelectFont(fixednums8x16, WHITE);
  GLCD.print(t.date);
  GLCD.SelectFont(fixednums8x16, BLACK);
  printDateTime(t.mon);
  printDateTime(t.yr);
      for(;buttonState(changeValuePin);t.date++){
        if(t.date>30) t.date=1;
        delay(200);
        Time t(t.yr, t.mon, t.date, t.hr, t.min, t.sec, Time::kSunday); // Set time
      }
  rtc.time(t);
}

void menuSetSecond()
{
  Time t = rtc.time();
  GLCD.CursorToXY(25,15);
  GLCD.print(t.hr);
  printDigits(t.min);
  GLCD.SelectFont(fixednums8x16, WHITE);
  printDigits(t.sec);
  GLCD.SelectFont(fixednums8x16, BLACK);
      for(;buttonState(changeValuePin);t.sec++){
        if(t.sec>58) t.sec=0;
        delay(200);
        Time t(t.yr, t.mon, t.date, t.hr, t.min, t.sec, Time::kSunday); // Set time
      }
  rtc.time(t);
}

void menuSetMinute()
{
  Time t = rtc.time();
  GLCD.CursorToXY(25,15);
  GLCD.print(t.hr);
  GLCD.SelectFont(fixednums8x16, WHITE);
  printDigits(t.min);
  GLCD.SelectFont(fixednums8x16, BLACK);
  printDigits(t.sec);
      for(;buttonState(changeValuePin);t.min++){
        if(t.min>58) t.min=0;
        delay(200);
        Time t(t.yr, t.mon, t.date, t.hr, t.min, t.sec, Time::kSunday); // Set time
      }
  rtc.time(t);
}

void menuSetHour()
{
  Time t = rtc.time();
  GLCD.CursorToXY(25,15);
  GLCD.SelectFont(fixednums8x16, WHITE);
  GLCD.print(t.hr);
  GLCD.SelectFont(fixednums8x16, BLACK);
  printDigits(t.min);
  printDigits(t.sec);
      for(;buttonState(changeValuePin);t.hr++){
        if(t.hr>23) t.hr=0;
        delay(200);
        Time t(t.yr, t.mon, t.date, t.hr, t.min, t.sec, Time::kSunday);
      }
  rtc.time(t);
}


void digitalClockDisplay()
{
  Time t = rtc.time();
  GLCD.SelectFont(fixednums8x16, BLACK);
  GLCD.CursorToXY(25,15);
  GLCD.print(t.hr);
  printDigits(t.min);
  printDigits(t.sec);
  GLCD.CursorToXY(20,35);
  GLCD.print(t.date);
  printDateTime(t.mon);
  printDateTime(t.yr);
/*  //// Добавил Илья
  GLCD.CursorToXY(20,50);
  GLCD.SelectFont(SystemFont5x7, BLACK);
  GLCD.print("Alarm: ");
  GLCD.print(EEPROM.read(alarmHour_EERPROM_ADDRESS));
  GLCD.print(":");
  GLCD.print(EEPROM.read(alarmMinute_EERPROM_ADDRESS));
  /////----- */
}

void printDigits(int digits)
{
  GLCD.print(":");
  if(digits < 10)
    GLCD.print('0');
  GLCD.print(digits);
}

void printDateTime(int datetime)
{
  GLCD.print("/");
  if(datetime < 10)
    GLCD.print('0');
  GLCD.print(datetime);
} 

void drawPockerFace()
{
  GLCD.FillCircle(30,25,5,BLACK);  
  GLCD.FillCircle(98,25,5,BLACK);
  GLCD.DrawLine(43,40,85,40,BLACK);  
}

void drawSmileFaceWithOpenEyes()
{
  GLCD.FillCircle(30,25,5,BLACK);
  GLCD.FillCircle(98,25,5,BLACK);
  GLCD.FillHalfCircle(64,40,21,BLACK);
}

void drawPockerFaceWithCloseEyes()
{
  GLCD.DrawLine(25,25,35,25,BLACK);  
  GLCD.DrawLine(93,25,103,25,BLACK);
  GLCD.DrawLine(43,40,85,40,BLACK);  
}  
void drawSmileFaceWithCloseEyes()
{
  GLCD.DrawLine(25,25,35,25,BLACK);  
  GLCD.DrawLine(93,25,103,25,BLACK);
  GLCD.FillHalfCircle(64,40,21,BLACK);
}

void drawSurprisedFace()
{
  GLCD.FillCircle(30,25,5,BLACK);
  GLCD.FillCircle(98,25,5,BLACK);
  GLCD.FillCircle(64,45,12,BLACK);
}  
  
