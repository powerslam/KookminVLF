#include <VLF.h>
#include <LineScan.h>

#define NORMAL_MODE 0
#define MAZE_MODE 1
#define OBSTACLE_MODE 2
#define FINAL_MODE 3

#define LINE_SPEED 120

// 원래는 차례대로 1.6, 0, 0.5 임
// 1.5 -> 2 / 0.6 -> 0.8 // 231115 / 100 -> 120
#define P_TERM 2
#define I_TERM 0
#define D_TERM 0.8

LineScan cam;

int mode = NORMAL_MODE;      // 주행 모드

// 각도 구할 때 사용되는 변수
double angle_error = 0;       // 차선 좌표 - 중심 좌표
double angle_error_prev = 0;  // 업데이트 전 angle_error
int angle = 0;

double ult_error = 0;
double ult_error_prev = 0;

long right_obstacle = 0;  // 오른쪽 초음파 센서 값

// 이동평균 필터를 사용한 초음파 값 얻어오기
long get_ult(int trig, int echo) {
  int buffer_size = 1;

  long dist = 0;
  long tmp; 
  for (int i = 0; i < buffer_size; i++) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);

    digitalWrite(trig, HIGH);
    delayMicroseconds(10);

    digitalWrite(trig, LOW);

    tmp = pulseIn(echo, HIGH, 25000);
    if(tmp == 0) dist += 400;
    else dist += tmp * 17 / 1000;

    delayMicroseconds(1);
  }

  return dist / buffer_size;
}

// 라인을 체크하는 함수 (미로 구간에서 사용함)
bool checking_line(){
  cam.read_cam();
  cam.calc_centroid(80);
  return abs(cam.midPos - MID_PIXEL_NUM) > 10;
}

// 불빛 깜빡깜빡
void blink(int dt) {
  digitalWrite(13, HIGH);
  delay(dt);
  digitalWrite(13, LOW);
  delay(dt);
}

// 각도를 구하는 함수
int get_angle(){
  cam.read_cam();
  cam.calc_centroid(80);

  angle_error_prev = angle_error;
  angle_error = cam.midPos - MID_PIXEL_NUM;

  angle = P_TERM * angle_error + D_TERM * (angle_error - angle_error_prev);
  angle = constrain(angle, LEFT_STEER_ANGLE, RIGHT_STEER_ANGLE);
  
  return angle;
}

void setup() {
  Serial.begin(115200);

  vlf_init();
  cam.init();

  pinMode(13, OUTPUT);

  // 첫 시작은 NORMAL_MODE(라인 주행)

  // NORMAL_MODE
  // MAZE_MODE
  // OBSTACLE_MODE
  mode = NORMAL_MODE;

  // 재시작 테스트용
  // for(int i = 0; i < 20; i++) blink(100);
  // motor_stop();
  // delay(5000);

  motor_control(FRONT_SPIN, 120);
}

void loop() {
// 라인 모드

if(mode == NORMAL_MODE){
  if(get_ult(RIGHT_TRIG, RIGHT_ECHO) < 40) {
    Serial.println(get_ult(RIGHT_TRIG, RIGHT_ECHO));
    mode = MAZE_MODE;
    return;
  } 
  
  else {
    steering_control(get_angle());
    delay(40);
  }

  delay(20);
}


/// 미로 모드
else if(mode == MAZE_MODE){
  digitalWrite(13, HIGH);
  motor_control(HIGH, 150);

  unsigned long start = millis();
  
  // // part 1 -- 처음 진입했을 때 라인 탈출
  while(millis() - start < 5000){
    ult_error_prev = ult_error;
    ult_error = get_ult(RIGHT_TRIG, RIGHT_ECHO) - 30; // 0 ~ 10
    steering_control(constrain(ult_error * 2, -82, 98));
    delay(50);
  }

  motor_stop();
  // 가끔 값이 튈 때가 있음 ㅇㅇ
  for(int i = 0; i < 50; i++){
    cam.read_cam();
    // cam.print_pixels();
    delay(1);
  }
  
  motor_control(HIGH, 150);

  // 미로 주행
  while(true){
    if(checking_line()){
      motor_stop();
      delay(100);

      if(checking_line()) break;

      motor_control(HIGH, 150);
    }
    
    // @change
    ult_error = get_ult(RIGHT_TRIG, RIGHT_ECHO) - 30; // 0 ~ 10
    steering_control(constrain(ult_error * 2, -82, 98));
    delay(50);
  }
  
  // 코너를 2번 주행하면 LED를 끔
  digitalWrite(13, LOW);

  // motor_control(HIGH, 100);
  // steering_control(30);
  // delay(300);

  // @change
  motor_control(HIGH, 120);
  while(get_ult(RIGHT_TRIG, RIGHT_ECHO) > 37){
    digitalWrite(13, HIGH);
    steering_control(30);
    delay(25);
    digitalWrite(13, LOW);
    delay(25);
    
  }

  motor_stop();
  steering_control(0);
  delay(100);

  mode = OBSTACLE_MODE;
}

else if(mode == OBSTACLE_MODE){
  digitalWrite(13, HIGH);
  
  motor_control(FRONT_SPIN, LINE_SPEED);

  // 25cm 안쪽으로 장애물이 있으면
  if(get_ult(FRONT_TRIG, FRONT_ECHO) < 35) {
    // 안정성을 위해 1초간 정지
    motor_stop();
    delay(1000);

    // @@@@ 장애물 탈출 @@@@ //
    steering_control(LEFT_STEER_ANGLE + 20);

    // 우측에 장애물이 없는 동안 회전
    // <=> 우측에 장애물이 감지될 때까지 회전
    while(get_ult(RIGHT_TRIG, RIGHT_ECHO) > 30) {
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
      long error = right_obstacle - 10;  // 0 ~ 10
      steering_control(error * 3);
      delay(50);

      // @@@ 라인을 체크 했더니 오른쪽으로 가야하는 상황이면 @@@
      if(checking_line()){
        motor_stop();
        delay(100);

        if(checking_line()) break;
      }

      motor_control(HIGH, 130);
      // sign_prev = sign;
      // sign = get_angle() > 8;
      // if (sign && !sign_prev) break;
      // @@@ //

      delay(50);
    } while (1);  // 위 조건을 만족할 때까지 반복

    mode = FINAL_MODE;  // 장애물이 종료되면 NORMAL_MODE로 전환
    return;
  }

  // 앞에 장애물이 없는 경우
  else {
    steering_control(get_angle());
    delay(40);
  }
}

else {
  motor_control(HIGH, 120);
  steering_control(get_angle());
  delay(40);
}}