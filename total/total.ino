#include <VLF.h>
#include <LineScan.h>

#define NORMAL_MODE 0
#define MAZE_MODE 1
#define OBSTACLE_MODE 2

#define THRESHOLD 50

// 원래는 차례대로 1.6, 0, 0.5 임
#define P_TERM 1.5
#define I_TERM 0
#define D_TERM 0.6

// ==> 근데 이 친구가 영점이 왼쪽으로 가 있음 ==> 어떻게 해결할까?
// 생각에는 중심점에 대해 bias를 줘야 할 거 같은데 어렵다
// 라인이 쓸데없는 걸 체크하는 것 같기도? ==> processing 찍어봐야 함

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
  mode = NORMAL_MODE;
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

void blink(int dt){
  digitalWrite(13, HIGH);
  delay(dt);
  digitalWrite(13, LOW);
  delay(dt);
}

// 각도 구하기
int get_angle(){
  cam.read_cam();
  cam.calc_centroid(THRESHOLD);

  angle_error_prev = angle_error;
  angle_error = cam.midPos - MID_PIXEL_NUM;

  long p_value = P_TERM * angle_error;
  long i_value = I_TERM * angle_error;
  long d_value = D_TERM * (angle_error - angle_error_prev);

  long angle = p_value + i_value + d_value;
  angle = constrain(angle, -82, 98); // 중앙 각도가 82 임
  return angle;
}

void loop(){switch(mode){
case NORMAL_MODE: // 일반 주행 모드
  // 만약에 오른쪽 센서에 40cm 미만이 체크되면
  if(get_ult(RIGHT_TRIG, RIGHT_ECHO) < 40){
    mode = MAZE_MODE;
    steering_control(0);
    break;
  } // 미로 진입

  steering_control(get_angle());
  delay(10);
  break;

case MAZE_MODE:
  // //////////////// 미로 주행하기 ////////////////////
  digitalWrite(13, HIGH); // 미로 들어왔다는 신호
  motor_control(HIGH, 180);

  int mz_cnt = 0;

  bool mz_flag = false;
  bool mz_flag_prev = false;

  while(mz_cnt < 2){
    long right = get_ult(RIGHT_TRIG, RIGHT_ECHO);
    
    // mz_flag: 현재 오른쪽이 90 이상인 경우 ==> 코너에 도달한 경우
    mz_flag_prev = mz_flag;
    mz_flag = right >= 90;

    // 생각해보니까 !mz_flag_prev && mz_flag 가 맞는 조건인 거 같음
    // 이제 막 코너에 도달한 경우
    if(mz_flag_prev != mz_flag) {      
      motor_control(HIGH, 130);

      // 여기는 고정 각도로 꺾음 ==> 라인 진입을 수월하게 하기 위해서
      while(get_ult(RIGHT_TRIG, RIGHT_ECHO) > 50) 
        steering_control(RIGHT_STEER_ANGLE);
      
      mz_cnt++; // 코너 카운트 1 증가
      mz_flag_prev = mz_flag = false; // 위에 조건이 채택되면 이 친구는 없어도 됨
    }

    // 일반적인 상황에서는 미로 벽과 20cm 간격을 두고 주행
    else {
      long error = get_ult(RIGHT_TRIG, RIGHT_ECHO) - 20; // 0 ~ 10
      steering_control(constrain(error * 6, -82, 98));
      delay(50);
    }
  }
  
  // 코너를 2번 주행하면 LED를 끔
  digitalWrite(13, LOW);

  // LED 깜빡이면서 무한대기
  while(1){
    motor_stop();
    steering_control(0);

    digitalWrite(13, HIGH);
    delay(500);

    digitalWrite(13, LOW);
    delay(500);
  }

  // 만약에 된다면 //
  motor_control(HIGH, 100);
  delay(500);

  mode = OBSTACLE_MODE;
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