#include "Arduino.h"
#include "Servo.h"
#include "VLF.h"

Servo SteeringServo;

void vlf_init(){
  Serial.begin(115200);

  // 전면 초음파
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  
  // 좌측 초음파
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  // 우측 초음파
  pinMode(RIGHT_TRIG, OUTPUT);
  
  pinMode(RIGHT_ECHO, INPUT);

  // 모터(DC)
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);

  // 모터(서보)
  SteeringServo.attach(RC_SERVO_PIN);
  // 서보 정중앙 설정
  SteeringServo.write(NEURAL_ANGLE);
  delay(500);
}

long get_ult(int trig, int echo){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  return pulseIn(echo, HIGH) * 17 / 1000; 
}

void motor_control(uint8_t dir, int speed){
  speed = max(speed, 0);
  speed = min(speed, 255);

  digitalWrite(MOTOR_DIR, dir);
  analogWrite(MOTOR_PWM, speed);
}

void motor_stop(){
  motor_control(FRONT_SPIN, 0);
}

void steering_control(int angle){
  if(angle < 0) angle = max(angle, LEFT_STEER_ANGLE);
  else if(angle > 0) angle = min(angle, RIGHT_STEER_ANGLE);

  SteeringServo.write(NEURAL_ANGLE + angle);
}
