#include <VLF.h>
#include <LineScan.h>

#define MOVE 1

#define MOTOR_SPEED 150

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
  cam.read_cam();
  cam.calc_centroid(50); // @param: threshold 값임

  angle_error_prev = angle_error;
  angle_error = cam.midPos - MID_PIXEL_NUM;

  angle = P_TERM * angle_error + D_TERM * (angle_error - angle_error_prev);
  angle = constrain(angle, LEFT_STEER_ANGLE, RIGHT_STEER_ANGLE);
  
  return angle;
}

void loop(){switch(robot_mode){
case NORMAL_MODE:
  // 한 줄이 전부 흰색이면
  // while(cam.chk_line()) change_mode = true;
  // if(change_mode){
  //   change_mode = false;
  //   robot_mode = MAZE_MODE;
  //   // motor_stop(0); // @have to test
  //   break;
  // } // 미로 진입 시

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

  av_sensor = get_ult(RIGHT);
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
  // 장애물이 생기면
  if (get_ult(FRONT) < 45) {
    motor_stop();
    delay(1000);
    
    // @@@@ 장애물 탈출 @@@@ //
    steering_control(LEFT_STEER_ANGLE + 20);
     
    // 살짝 살짝 돌아서 체크하도록 끊어서 돌게 함
    // 정면이 비었고, 우측면은 장애물이 감지 될 때까지 회전
    while(get_ult(FRONT) < 100 || get_ult(RIGHT) > 30){
      motor_control(HIGH, 150);
      delay(100);
      
      motor_stop();
      delay(50);
    }
    
    // 부딪힐 수도 있으니까 약간 더 돌림
    // motor_control(HIGH, 150);
    // delay(400);

    // 안정성을 위해 잠깐 멈춤
    steering_control(0);
    motor_stop();
    delay(1000);

    // 약간 앞으로 감
    motor_control(HIGH, 100);
    delay(300);

    // 이제 속도 150 해서 장애물 타기
    motor_control(HIGH, 150);

    // 라인을 체크 했더니 오른쪽으로 가야하는 상황이면 ==> 다시 실행해보면서 확인해야 함
    while(get_angle() > 0) {
      av_sensor = get_ult(RIGHT);
      
      long error = av_sensor - 20; // 0 ~ 10
      steering_control(error * 4);
      
      delay(500);
    }

    // 장애물 벗어나면 원래 모드로
    robot_mode = NORMAL_MODE;
  }

  // @@@@@ 앞에 장애물이 없는 경우 @@@@@ //
  else if(MOVE) steering_control(get_angle());
  break;
}}