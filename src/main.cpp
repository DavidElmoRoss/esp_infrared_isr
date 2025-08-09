/**********************************************************************
  Title:    Infrared Detector triggers an Interrupt Service Routine
  Author:   David Elmo Ross
  Date:     Aug 9th,2025
  Description: This program will cause the RGB LED to come on, and
               the LCD Screen to turn RED with INTRUDER DETECT
               when motion is detected. It will also show on the 
               screen PC Screen as well
**********************************************************************/
#include <Arduino.h>              // include arduino library
#include "rgb_lcd.h"              // include rgb_lcd library
#include <Wire.h>                 // include Library for Wire.h
int sensorPin =19 ;               // Pin # of the infrared motion sensor pin
int ledPin = 25;                  // Pin # of the BUILTIN LED 
volatile int count=0;
rgb_lcd lcd;                      // declare lcd to be of class rgb_lcd

static portMUX_TYPE my_mutex = portMUX_INITIALIZER_UNLOCKED;                                   

void IRAM_ATTR isr()              // puts isr code into RAM for fast response
{
  portENTER_CRITICAL(&my_mutex);  // Block any processes accessing sensorPin
                                  // since IRQ PB_PIN access is MUTually EXclusive. 
      count=1;                    // make counter 1 to indicate isr() has executed
  portEXIT_CRITICAL(&my_mutex);   // allow other processes to access sensorPin  
}


void setup() 
{
  Wire.begin(SDA, SCL);          // attach to the IIC pin
  lcd.begin(16,2);               // start up the LCD
  Serial.begin(9600);            // initialize serial
  pinMode(sensorPin, INPUT);     // set ESP32 pin to input mode 
  pinMode(ledPin, OUTPUT);       // set BUILT IN LED to output
                                 // allows PORT Multiplexer to LOCK processes
                                 // from accessing PB_PIN at the same time


                                 // trigger isr on RISING edge from IR Sensor
  attachInterrupt(digitalPinToInterrupt(sensorPin),&isr,RISING);
                                 // print DETECTION STATUS at (10,32) on screen
  printf("\e[2J\e[10;32HDetection Status");
                                 // set future text to BOLD CYAN
  printf("\e[1;36m"); 
}

void loop() 
{
  if (count==1)                    // if motion detected
  {   
    digitalWrite(ledPin,LOW);      // turn on RED RGB LED
    lcd.setRGB(0xff,0,0);          // TURN LCD to RED
    lcd.setCursor(0,0);            // lcd to col 0 row o
    lcd.print("Intruder Detect "); // print INTRUDER DETECT on LCD
    printf("\e[11;36HDetected");   // print DETECTED at (11,36) on Screen
    fflush(stdout);                // flush output to screen
    delay(2000);                   // delay for 2sec
    count=0;                       // reset count to 0
  } 
  digitalWrite(ledPin,HIGH);       // TURN OFF BUILT IN LED
  printf("\e[11;36H        ");     // clear DETECTED message
  lcd.setRGB(0,0,0);               // turn off RGB on LCD
  lcd.setCursor(0,0);              // lcd Cursor to col 0 row 0
  lcd.print("                ");   // clear detected message
}
