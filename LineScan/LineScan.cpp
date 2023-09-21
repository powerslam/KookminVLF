#include "Arduino.h"
#include "LineScan.h"

LineScan::LineScan(int thresholdValue): thresholdValue(thresholdValue) {}

void LineScan::init() {
  for(int i = 0; i < NPIXELS; i++){
    this->LineSensor_Data[i] = 0;
    this->LineSensor_Data_Adaption[i] = 0;
  }

  pinMode(SIpin, OUTPUT);
  pinMode(CLKpin, OUTPUT);
  pinMode(AOpin, INPUT);

  digitalWrite(SIpin, LOW);
  digitalWrite(CLKpin, LOW);

#if FASTADC
  // set prescale to 16
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
#endif
}

void LineScan::read_line_sensor() {
  this->MaxPixel = 0;
  this->MinPixel = 1023;

  delayMicroseconds(1);  /* Integration time in microseconds */
  delay(10);              /* Integration time in miliseconds  */

  digitalWrite(CLKpin, LOW);
  digitalWrite(SIpin, HIGH);

  digitalWrite(CLKpin, HIGH);
  digitalWrite(SIpin, LOW);

  delayMicroseconds(1);

  for (int i = 0; i < NPIXELS; i++) {
    Pixel[i] = analogRead(AOpin) / 4;
    
    MaxPixel = max(MaxPixel, Pixel[i]);
    MinPixel = min(MinPixel, Pixel[i]);

    digitalWrite (CLKpin, LOW);
    delayMicroseconds (1);
    digitalWrite (CLKpin, HIGH);
  }

  for (i = 0; i < NPIXELS; i++)
    LineSensor_Data_Adaption[i] = map(Pixel[i], MinPixel, MaxPixel, 0, 255);
}

void LineScan::calcCentroid() {
  
}
