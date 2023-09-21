#ifndef LineScan_H
#define LineScan_H

//// 카메라 핀번호 ////
#define AOpin A0
#define SIpin 22
#define CLKpin 23
//// 카메라 핀번호 ////

/// 입력 데이터 수 == 1Line 이고, 128 pixel을 받아옴 ///
#define NPIXELS 128

//// 빠른 아날로그 입력 ////
#define FASTADC 1
// defines for setting and clearing register bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//// 빠른 아날로그 입력 ////

class LineScan {
  public:
    int LineSensor_Data_Adaption[NPIXELS];

    LineScan(int thresholdValue);

    void init();
    void read_line_sensor();
    void calcCentroid();

  private:
    byte Pixel[NPIXELS];
    int LineSensor_Data[NPIXELS];
    int thresholdValue, MaxPixel, MinPixel;
};

#endif