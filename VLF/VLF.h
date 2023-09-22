#ifndef VLF_H
#define VLF_H

// pinMode 및 변수 초기화
void vlf_init();

/// 초음파 핀 번호 ///
#define FRONT_TRIG 9
#define FRONT_ECHO 10

#define LEFT_TRIG 48
#define LEFT_ECHO 49

#define RIGHT_TRIG 46
#define RIGHT_ECHO 47
/// 초음파 핀 번호(앞, 좌, 우 순) ///

// 초음파 거리 재기
// param
// 1. trig: trig 핀 번호
// 2. echo: echo 핀 번호
long get_ult(int trig, int echo);

/// 모터(DC) 핀 번호 ///
#define MOTOR_DIR 4
#define MOTOR_PWM 5

#define FRONT_SPIN HIGH
#define BACK_SPIN LOW
/// 모터(DC) 핀 번호 ///

/// 모터 제어 함수 ///
void motor_control(uint8_t dir, int speed);
void motor_stop();
/// 모터 제어 함수 ///

/// 서보 모터 핀 번호 및 각도 ///
#define RC_SERVO_PIN 8
#define NEURAL_ANGLE 82
#define LEFT_STEER_ANGLE -75
#define RIGHT_STEER_ANGLE 55
/// 서보 모터 핀 번호 및 각도 ///

void steering_control(int angle);

#endif