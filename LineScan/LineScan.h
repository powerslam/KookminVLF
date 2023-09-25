#ifndef LineScan_H
#define LineScan_H

//// 카메라 핀번호 ////
#define AOpin A0
#define SIpin 22
#define CLKpin 23
//// 카메라 핀번호 ////

/// 입력 데이터 수 == 1Line 이고, 128 pixel을 받아옴 ///
#define ROI 14
#define NPIXELS 128
#define MID_PIXEL_NUM (NPIXELS + ROI) >> 1

//// 빠른 아날로그 입력 ////
#define FASTADC 1
// defines for setting and clearing register bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//// 빠른 아날로그 입력 ////

class LineScan {
  public:
    short midPos;
  
  public:
    LineScan();

    void init();
    void read_cam();
    void calc_centroid(short threshold);

    short get_pixel(int i){
      return Pixel[i];
    }

    short get_diff_pixel(int i){
      return diffPixel[i];
    }

  private:
    short Pixel[NPIXELS], diffPixel[NPIXELS];
    short leftPos, rightPos;
};

#endif