#include <VLF.h>
#include <Servo.h>

Servo Steeringservo;
int Steering_Angle = NEURAL_ANGLE;

void setup() {
  vlf_init();
}

unsigned long second = 0;

void loop() {
  // 장애물이 30cm 안으로 들어오면
  if (get_ult(FRONT_TRIG, FRONT_ECHO) < 30) {
    //// 1.5초간 왼쪽 방향으로 전진 ////
    steering_control(LEFT_STEER_ANGLE);
    delay(100);

    second=millis();
    while (millis() - second < 1500) {
      // Serial.print("first message : ");
      // Serial.println(second);
    }
    //// 1.5초간 왼쪽 방향으로 전진 ////
  
    //// 장애물 구간 탈출을 위해 전진 ////
    steering_control(0);
    delay(100);
    //// 장애물 구간 탈출을 위해 전진 ////
    
    //// 라인 복귀를 위해 오른쪽으로 꺾어 진입 ////
    steering_control(RIGHT_STEER_ANGLE);
    second=millis();
    while (millis() - second < 1500) {
    //   Serial.print("first message : ");
    //   Serial.println(second);
    }
    //// 라인 복귀를 위해 오른쪽으로 꺾어 진입 ////
    
    steering_control(0);
    delay(100);
  }
  
  else motor_control(HIGH, 80); 
}
