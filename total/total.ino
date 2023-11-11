#include <VLF.h>
#include <LineScan.h>

#define NORMAL_MODE 0
#define MAZE_MODE 1
#define OBSTACLE_MODE 2

#define P_TERM 1.6
#define D_TERM 0.5

LineScan cam;

int mode = 0;                // 주행 모드
bool change_mode;            // 주행 모드 변경 flag

// 각도 구할 때 사용되는 변수
double angle_error = 0;      // 차선 좌표 - 중심 좌표
double angle_error_prev = 0; // 업데이트 전 angle_error

long right_obstacle = 0;     // 오른쪽 초음파 센서 값

void setup() {
  Serial.begin(115200);

  vlf_init();
  cam.init();

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  motor_control(FRONT_SPIN, 100);

  // 첫 시작은 NORMAL_MODE(라인 주행)
  mode = OBSTACLE_MODE; // NORMAL_MODE;
  change_mode = false;
}


long distance = 0;
long get_ult(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  distance = pulseIn(echo, HIGH) * 17 / 1000;
  delay(10);
  return distance;
}

// 각도 구하기
int get_angle(){
  cam.read_cam();
  cam.calc_centroid(50);

  angle_error_prev = angle_error;
  angle_error = cam.midPos - MID_PIXEL_NUM;

  long angle = P_TERM * angle_error + D_TERM * (angle_error - angle_error_prev);
  angle = constrain(angle, -82, 92);
  return angle;
}

void loop(){switch(mode){
case NORMAL_MODE: // 일반 주행 모드
  steering_control(get_angle());
  digitalWrite(13, HIGH);
  // 한 줄이 전부 흰색이면
  // while(cam.chk_line()) change_mode = true;
  // if(change_mode){
  //   change_mode = false;
  //   robot_mode = MAZE_MODE;
  //   // motor_stop(0); // @have to test
  //   break;
  // } // 미로 진입 시
  
  delay(50);
  break;

case MAZE_MODE:
  // 미로의 경우 항상 흰색 라인에 간다는 보장이 없음...
  // while(cam.chk_line()) change_mode = true;
  // if(change_mode){
  //   steering_control(0); // 중심 맞추고 전진해야 함
  //   change_mode = false;
  //   robot_mode = OBSTACLE_MODE;
  //   break;
  // } // 탈출 시
  // // 그 pwa 적용하기(라이브러리)

  right_obstacle = get_ult(RIGHT_TRIG, RIGHT_ECHO);
  
  //////////////// 미로 주행하기 ////////////////////
  if (right_obstacle > 15) {
    // Serial.println("Case 1");
    steering_control(RIGHT_STEER_ANGLE - 10);
    delay(200);
  }

  else if (right_obstacle < 10) {
    // Serial.println("Case 2");
    steering_control(LEFT_STEER_ANGLE + 40);
    delay(200);
  }

  // ==> 해보고 되면 테스트 해보기
  /*
  long error = right_obstacle - 10; // 0 ~ 10
  steering_control(error * 6);
  delay(100);
  */
  //////////////// 미로 주행하기 ////////////////////
  break;

case OBSTACLE_MODE:
  if (get_ult(FRONT_TRIG, FRONT_ECHO) < 35) { // 25cm 안쪽으로 장애물이 있으면
    // 안정성을 위해 1초간 정지
    motor_stop();
    delay(1000);

    // @@@@ 장애물 탈출 @@@@ //
    steering_control(LEFT_STEER_ANGLE + 20);
  
    // 우측에 장애물이 없는 동안 회전
    // <=> 우측에 장애물이 감지될 때까지 회전
    while(get_ult(RIGHT_TRIG, RIGHT_ECHO) > 30){
      // 빨리 돌면 초음파가 체크를 못 할 수도 있어서 천천히 돔
      motor_control(HIGH, 120);
      delay(100);
    }

    motor_control(HIGH, 130);
    delay(250);

    // 안정성을 위해 잠깐 멈춤
    steering_control(0);
    motor_stop();
    delay(1000);

    motor_control(HIGH, 110);

    bool sign_prev = false;
    bool sign = false;
    
    do {
      right_obstacle = get_ult(RIGHT_TRIG, RIGHT_ECHO);
      
      // 오른쪽 장애물 간 거리에 따라 회전각이 다름
      long error = right_obstacle - 10; // 0 ~ 10
      steering_control(error * 3);
      delay(50);

      // @@@ 라인을 체크 했더니 오른쪽으로 가야하는 상황이면 @@@
      sign_prev = sign;
      sign = get_angle() > 8;
      if(sign && !sign_prev) break;
      // @@@ //
      
      delay(50);
    } while(1); // 위 조건을 만족할 때까지 반복

    mode = NORMAL_MODE; // 장애물이 종료되면 NORMAL_MODE로 전환
  } else {
    steering_control(get_angle()); // 앞에 장애물이 없는 경우 
    delay(50);
  }
  break;
}}