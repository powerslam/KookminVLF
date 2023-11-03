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

  // 쓰레기라서 그냥 대입
  for(int i = 0; i < 15; i++) {
    Pixel[i] = analogRead(AOpin) / 4;
    digitalWrite (CLKpin, LOW);
    delayMicroseconds (1);
    digitalWrite (CLKpin, HIGH);
  }

  // 처음 부터 다시 대입
  for (int i = 0; i < NPIXELS; i++) {
    Pixel[i] = analogRead(AOpin) / 4;
    digitalWrite (CLKpin, LOW);
    delayMicroseconds (1);
    digitalWrite (CLKpin, HIGH);
  }
}

bool LineScan::chk_line(){
  this->countPixels = 0;
  for(int i = 0; i < NPIXELS; i++) this->countPixels++;
}

void LineScan::calc_centroid(int threshold) {
  int cnt = 0;
  this->rightPos = this->leftPos = this->midPos = 0;
  
  for(int i = 0; i < MID_PIXEL_NUM; i++){
    if(Pixel[i] >= threshold) {
      this->rightPos += i;
      cnt++;
    }
  }
  cnt = cnt == 0 ? 1 : cnt;
  this->rightPos /= cnt;

  cnt = 0;
  for(int i = MID_PIXEL_NUM; i < NPIXELS; i++){
    if(this->Pixel[i] >= threshold) {
      this->leftPos += i;
      cnt++;
    }
  }

  cnt = cnt == 0 ? 1 : cnt;

  this->leftPos = this->leftPos <= cnt ? NPIXELS : this->leftPos / cnt;

  // 중앙 좌표 구하기
  this->midPos = this->leftPos + this->rightPos >> 1;

  // 시각화용 값 할당
  this->Pixel[this->leftPos] = 127;
  this->Pixel[this->rightPos] = 127;
  this->Pixel[this->midPos] = 64;
}

void LineScan::print_pixels(){
  for(int i = 0; i < 128; i++){
    Serial.print(this->Pixel[i]);
    Serial.print(" ");
  }
  Serial.println();
}