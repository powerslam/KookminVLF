#include <VLF.h>
#include <Servo.h>

Servo Steeringservo;
int Steering_Angle = NEURAL_ANGLE;

void setup() {
  vlf_init();
}
int key=1;
long sum=0;
int cnt=0;
int past=0;
float av_senser=0;
void loop() {
  motor_control(HIGH, 80);
  if (key==1/*get_ult(FRONT_TRIG, FRONT_ECHO)>30 && 라인을 벗어났을 때*/) {
    while(/*라인에 복귀할 때 까지*/1) {
      av_senser=(0.55*get_ult(RIGHT_TRIG, RIGHT_ECHO))+0.45*av_senser;
      //Serial.println(av_senser);
      if (av_senser>15) {
        Serial.println("Case 1");
        steering_control(RIGHT_STEER_ANGLE-10);
        delay(200);
      }
      else if (av_senser<10) {
        Serial.println("Case 2");
        steering_control(LEFT_STEER_ANGLE+40);
        delay(200);
      }
    }
  }
  else {
    motor_control(HIGH, 80);
    steering_control(0);
  }
}
