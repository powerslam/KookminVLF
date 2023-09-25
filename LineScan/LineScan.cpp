#include "Arduino.h"
#include "LineScan.h"

LineScan::LineScan(){}

void LineScan::init() {
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

void LineScan::read_cam() {
  delayMicroseconds(1);  /* Integration time in microseconds */
  delay(10);              /* Integration time in miliseconds  */

  digitalWrite(CLKpin, LOW);
  digitalWrite(SIpin, HIGH);

  digitalWrite(CLKpin, HIGH);
  digitalWrite(SIpin, LOW);

  delayMicroseconds(1);

  for (int i = 0; i < NPIXELS; i++) {
    Pixel[i] = analogRead(AOpin) / 4;
    digitalWrite (CLKpin, LOW);
    delayMicroseconds (1);
    digitalWrite (CLKpin, HIGH);
  }
}

void LineScan::calc_centroid(short threshold) {
  short cnt = 0;
  this->rightPos = this->leftPos = this->midPos = 0;

  // 오른쪽 평균 좌표 구하기
  for(int i = ROI; i < MID_PIXEL_NUM; i++){
    this->diffPixel[i] = abs(this->Pixel[i + 1] - this->Pixel[i]);
    if(this->diffPixel[i] < threshold) 
      // 시각화용 값 할당
      diffPixel[i] = 0;

    else {
      // 시각화용 값 할당
      this->diffPixel[i] = 255;
      this->rightPos += i;
      cnt++;
    }
  }

  this->rightPos /= cnt;

  // 왼쪽 평균 좌표 구하기
  cnt = 0;
  for(int i = MID_PIXEL_NUM; i < NPIXELS - 1; i++){
    this->diffPixel[i] = abs(this->Pixel[i + 1] - this->Pixel[i]);
    if(this->diffPixel[i] < threshold) 
      // 시각화용 값 할당
      this->diffPixel[i] = 0;

    else {
      // 시각화용 값 할당
      this->diffPixel[i] = 255;
      this->leftPos += i;
      cnt++;
    }
  }

  this->leftPos /= cnt;

  // 중앙 좌표 구하기
  this->midPos = this->leftPos + this->rightPos >> 1;

  // 시각화용 값 할당
  this->diffPixel[this->leftPos] = 127;
  this->diffPixel[this->rightPos] = 127;
  this->diffPixel[this->midPos] = 64;
}
