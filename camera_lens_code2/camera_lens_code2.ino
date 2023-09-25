#define AOpin  A0     // Analog output - yellow
#define SIpin  22   // Start Integration - orange
#define CLKpin 23    // Clock - red
// Vcc - brown
// GND - black

#define NPIXELS 128  // No. of pixels in array
#define ROI 14 // 14번째 픽셀부터 라인을 탐색하겠다는 뜻 ==> 근데 이 부분은 다시 픽셀 수를 계산해야 함
#define MID_PIXEL_IDX (NPIXELS + ROI) / 2

short Pixel[NPIXELS];
short leftPos, rightPos, midPos;
short diffPixel[NPIXELS];

#define FASTADC 1
// defines for setting and clearing register bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void setup() {
  Serial.begin(115200);

  // put your setup code here, to run once:
  int i;

  pinMode(SIpin, OUTPUT);
  pinMode(CLKpin, OUTPUT);
  pinMode (AOpin, INPUT);

  digitalWrite(SIpin, LOW);   // IDLE state
  digitalWrite(CLKpin, LOW);  // IDLE state

#if FASTADC
  // set prescale to 16
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
#endif
}

void get_line_data() {
  int i;

  delayMicroseconds (1);  /* Integration time in microseconds */
  delay(10);              /* Integration time in miliseconds  */

  digitalWrite (CLKpin, LOW);
  digitalWrite (SIpin, HIGH);

  digitalWrite (CLKpin, HIGH);
  digitalWrite (SIpin, LOW);

  delayMicroseconds (1);

  for (i = 0; i < NPIXELS; i++) {
    // analogRead는 max가 1023이기 때문
    Pixel[i] = analogRead (AOpin) / 4; // 8-bit is enough
    digitalWrite (CLKpin, LOW);
    delayMicroseconds (1);
    digitalWrite (CLKpin, HIGH);
  }
}

void get_mid_pos(short threshold) {
  int cnt = 0;
  rightPos = leftPos = midPos = 0;

  // Serial.print("left : ");
  for(int i = ROI; i < MID_PIXEL_IDX; i++){
    diffPixel[i] = abs(Pixel[i + 1] - Pixel[i]);
    if(diffPixel[i] < threshold) diffPixel[i] = 0;
    else {
      // Serial.print(i);
      // Serial.print(" ");
      diffPixel[i] = 255;
      rightPos += i;
      cnt += 1;
    }
  }

  rightPos /= cnt;

  cnt = 0;
  // Serial.print("right : ");
  for(int i = MID_PIXEL_IDX; i < NPIXELS - 1; i++){
    diffPixel[i] = abs(Pixel[i + 1] - Pixel[i]);
    if(diffPixel[i] < threshold) diffPixel[i] = 0;
    else {
      // Serial.print(i);
      // Serial.print(" ");
      diffPixel[i] = 255;
      leftPos += i;
      cnt += 1;
    }
  }

  leftPos /= cnt;
  midPos = leftPos + rightPos >> 1;

  diffPixel[leftPos] = 127;
  diffPixel[rightPos] = 127;

  diffPixel[midPos] = 64;
}

// 1pixel 당 1.45mm ~ 1.5mm 정도 됨

void loop() {
  int i;

  get_line_data();
  get_mid_pos(50);

  for(int i = 0; i < NPIXELS; i++){
    Serial.print(diffPixel[i]);
    Serial.print(" ");
  }

  Serial.println();
  delay(100);
}
