/**********************************************************************
  Title:    Infrared Detector triggered on Interrupt
  Author:   David Elmo Ross
  Date:     March 9th,2025
  Description: This program will cause the BUILT IN LED to come on
               when motion is detected. It will also show on the 
               screen when motion is detected.
**********************************************************************/
#include <Arduino.h>
int sensorPin =18 ;               // Pin # of the infrared motion sensor pin
int ledPin = 2;                   // Pin # of the BUILTIN LED 
volatile int count=0;

static portMUX_TYPE my_mutex = portMUX_INITIALIZER_UNLOCKED;                                   

void IRAM_ATTR isr()              // puts isr code into RAM for fast response
{
  portENTER_CRITICAL(&my_mutex);  // Block any processes accessing PB_PIN
                                  // since IRQ PB_PIN access is MUTually EXclusive. 
      count=1;                    // make counter 1 to indicate isr() has executed
  portEXIT_CRITICAL(&my_mutex);   // allow other processes to access PB_PIN  
}


void setup() {
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
  if (count==1)                  // if motion detected
  {   
    digitalWrite(ledPin,LOW);    // turn on BUILTIN LED
    printf("\e[11;36HDetected"); // print DETECTED at (11,36)
    fflush(stdout);              //flush output to screen
    delay(2000);                 // delay for 2sec
    count=0;                     // reset count to 0
  } 
  digitalWrite(ledPin,HIGH);     // TURN OFF BUILT IN LED
  printf("\e[11;36H        ");   // clear DETECTED message
}