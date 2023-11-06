#ifndef LineScan_H
#define LineScan_H

//// 카메라 핀번호 ////
#define AOpin A0
#define SIpin 22
#define CLKpin 23
//// 카메라 핀번호 ////

/// 입력 데이터 수 == 1Line 이고, 128 pixel을 받아옴 ///
#define NPIXELS 113 // 15 pixel이 쓰레기임
#define MID_PIXEL_NUM (NPIXELS >> 1)

//// 빠른 아날로그 입력 ////
#define FASTADC 1
// defines for setting and clearing register bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//// 빠른 아날로그 입력 ////

class LineScan {
  public:
    int leftPos, rightPos, midPos, modes;
  
  public:
    LineScan();

    void init();
    void read_cam();
    bool chk_line();
    void calc_centroid(int threshold);
    void print_pixels();

  private:
    int Pixel[NPIXELS], diffPixel[NPIXELS];
    int countPixels;
    
};

#endif