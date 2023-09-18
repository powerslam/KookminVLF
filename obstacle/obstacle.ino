////// 초음파 //////
#define FRONT_TRIG 9
#define FRONT_ECHO 10

#define LEFT_TRIG 48
#define LEFT_ECHO 49

#define RIGHT_TRIG 46
#define RIGHT_ECHO 47
/////////////////////

////// 모터 //////
#define MOTOR_DIR 4
#define MOTOR_PWM 5
//////////////////

////// steer //////
#include <Servo.h>
#define RC_SERVO_PIN 8
#define NEURAL_ANGLE 82
#define LEFT_STEER_ANGLE -55
#define RIGHT_STEER_ANGLE 35

Servo Steeringservo;
int Steering_Angle = NEURAL_ANGLE;
//////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);

  Steeringservo.attach(RC_SERVO_PIN);
  Steeringservo.write(NEURAL_ANGLE);
  delay(500);

  // 전면 초음파
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  
  // 좌측 초음파
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  // 우측 초음파
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

}
//////////////////////////////////////////////////////////초음파 거리 함수
int get_ult(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  return pulseIn(echo, HIGH) * 17 / 1000; 
}
///////////////////////////////////////////////////////////////
void steering_control(int steer_angle)//steer 각도 계산
{
  Steeringservo.write(NEURAL_ANGLE+steer_angle);
}
//////////////////////////////////////////////////////////////// 모터

void motor_control(bool dir, int speed){
  speed = max(speed, 0);
  speed = min(speed, 255);

  digitalWrite(MOTOR_DIR, dir);
  analogWrite(MOTOR_PWM, speed);
}

void motor_stop() {
  motor_control(LOW, 0);
}
///////////////////////////////////////////////////////////////////

unsigned long second=0;
bool flag = true;
void loop() {
  if (get_ult(FRONT_TRIG, FRONT_ECHO)<30) {
    steering_control(LEFT_STEER_ANGLE);
    delay(100);

    second=millis();
    while (millis() - second < 1500) {
      Serial.print("first message : ");
      Serial.println(second);
    } // 1.5 초 동안 왼쪽 방향으로 전진
  
    steering_control(0);
    delay(100);

    steering_control(RIGHT_STEER_ANGLE);
    second=millis();
    while (millis() - second < 1500) {
      Serial.print("first message : ");
      Serial.println(second);
    }

    steering_control(0);
    delay(100);
    /*steering_control(RIGHT_STEER_ANGLE);
    second=0;
    while (second<1500) {
      second=millis();
    }
    steering_control(RIGHT_STEER_ANGLE);
    //라인 복귀
    */
    //flag = false;
  } else {
    motor_control(HIGH, 80);
  }    
}
