#include <VLF.h>
#include <LineScan.h>

#define MOVE 1

#define MOTOR_SPEED 100

#define NORMAL_MODE 0
#define MAZE_MODE 1
#define OBSTACLE_MODE 2

#define P_TERM 1.75
#define D_TERM 0.8

LineScan cam;

double angle_error = 0;
double angle_error_prev = 0;
int angle = 0;

long av_sensor = 0;

int robot_mode = 0;
bool change_mode;

// 실제 픽셀은 113개임!!!!
void setup() {
  Serial.begin(115200);

  vlf_init();
  cam.init();

  if(MOVE) motor_control(FRONT_SPIN, MOTOR_SPEED);

  robot_mode = OBSTACLE_MODE;
  change_mode = false;
}

int get_angle(){
  angle_error_prev = angle_error;
  angle_error = cam.midPos - MID_PIXEL_NUM;

  angle = P_TERM * angle_error + D_TERM * (angle_error - angle_error_prev);
  angle = constrain(angle, LEFT_STEER_ANGLE, RIGHT_STEER_ANGLE);
  
  return angle;
}

void loop(){
  cam.read_cam();
  cam.calc_centroid(50); // @param: threshold 값임

switch(robot_mode){
case NORMAL_MODE:
  // 한 줄이 전부 흰색이면
  while(cam.chk_line()) change_mode = true;
  if(change_mode){
    change_mode = false;
    robot_mode = MAZE_MODE;
    // motor_stop(0); // @have to test
    break;
  } // 미로 진입 시

  // PID control or Pure Pursuit 로 대체하기
  if(MOVE) steering_control(get_angle());
  break;

case MAZE_MODE:
  while(cam.chk_line()) change_mode = true;
  if(change_mode){
    steering_control(0); // 중심 맞추고 전진해야 함
    change_mode = false;
    robot_mode = OBSTACLE_MODE;
    break;
  } // 탈출 시
  // 그 pwa 적용하기(라이브러리)

  av_sensor = get_ult(RIGHT_TRIG, RIGHT_ECHO);
  //Serial.println(av_sensor);
  
  //////////////// 미로 주행하기 ////////////////////
  if (av_sensor > 15) {
    // Serial.println("Case 1");
    steering_control(RIGHT_STEER_ANGLE - 10);
    delay(200);
  }

  else if (av_sensor < 10) {
    // Serial.println("Case 2");
    steering_control(LEFT_STEER_ANGLE + 40);
    delay(200);
  }
  //////////////// 미로 주행하기 ////////////////////
  break;

case OBSTACLE_MODE:
  // 앞에 장애물이 있는 경우(25cm 미만)
  if (get_ult(FRONT_TRIG, FRONT_ECHO) < 25 /*&& 현재 라인검출이 정상적으로 되고있을 때*/) {
    //// 1.5초간 왼쪽 방향으로 전진 ////
    motor_stop();
    delay(10);
    
    // @@@@@ 각도 확인 필요 @@@@@ //
    steering_control(LEFT_STEER_ANGLE + 90);
    motor_control(HIGH, 100);
    delay(100);

    /// 장애물과 일정 거리를 두며 이동 ///
    /*

      라인에 어떻게 복귀할 지 생각해보기

    */

    while(/* 라인에 복귀할 때 까지 */1) {
      av_sensor = get_ult(RIGHT_TRIG, RIGHT_ECHO);
      //Serial.println(av_sensor);
      if (av_sensor > 15) {
        Serial.println("Case 1");
        steering_control(RIGHT_STEER_ANGLE - 10);
        delay(200);
      }
      else if (av_sensor < 10) {
        Serial.println("Case 2");
        steering_control(LEFT_STEER_ANGLE + 40);
        delay(200);
      }
    }
  } else { // 그렇지 않은 경우 == 라인 타기
    if(MOVE) steering_control(get_angle());
  }
  break;
}}