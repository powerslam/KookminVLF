#include <VLF.h>
#include <Servo.h>

Servo Steeringservo;
int Steering_Angle = NEURAL_ANGLE;

void setup() {
  vlf_init();
}

int key=1;
unsigned long second = 0;

void loop() {
  // 장애물이 30cm 안으로 들어오면
  if (get_ult(FRONT_TRIG, FRONT_ECHO) < 25 /*&& 현재 라인검출이 정상적으로 되고있을 때*/) {
    //// 1.5초간 왼쪽 방향으로 전진 ////
    moter_stop();
    //motor_control(HIGH, 100);

    steering_control(LEFT_STEER_ANGLE+90);
    
    delay(100);

    /// 장애물과 일정 거리를 두며 이동 ///

    moter_control(HIGH, 100);
    while(/*라인에 복귀할 때 까지*/1) {
      av_senser=get_ult(RIGHT_TRIG, RIGHT_ECHO);
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
}
