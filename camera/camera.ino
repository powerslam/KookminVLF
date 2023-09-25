#include<LineScan.h>

LineScan cam;

void setup() {
  Serial.begin(115200);
  cam.init();
}

void loop() {
  cam.read_cam();
  cam.calc_centroid(50);
  
  for(int i = 0; i < NPIXELS; i++){
    Serial.print(cam.get_pixel(i));
    Serial.print(" ");
  }
  
  Serial.println();
  delay(100);
}
