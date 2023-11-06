#include <VLF.h>
#include <LineScan.h>

#define MOVE 0

#define NORMAL_MODE 0
#define MAZE_MODE 1
#define OBSTACLE_MODE 2
3

#define P_term 0
#define I_term 0
#define D_term 0 

LineScan cam;
int line_angle = 0;
int robot_mode = 0;

bool change_mode;

// 실제 픽셀은 113개임!!!!
void setup() {
  Serial.begin(115200);

  vlf_init();
  cam.init();

  if(MOVE) motor_control(FRONT_SPIN, 70);

  robot_mode = NORMAL_STATE;

  changeMode = false;
}

void print_pixels(){
  for(int i = 0; i < 128; i++){
    Serial.print(get_pixel(i));
    Serial.print(" ");
  }
  Serial.println();
}

void loop(){
  cam.read_cam();
  cam.calc_centroid(50); // @param: threshold 값임

switch(robot_mode){
case NORMAL_MODE:
  // 한 줄이 전부 흰색이면
  while(cam.chk_line()) changeMode = true;
  if(changeMode){
    changMode = false;
    robot_mode = MAZE_MODE;
    // motor_stop(0); // @have to test
    break;
  } // 미로 진입 시

  // PID control or Pure Pursuit 로 대체하기
  if(MOVE) steering_control(0);
  break;

case MAZE_MODE:
  while(cam.chk_line()) changeMode = true;
  if(changeMode){
    steering_control(0); // 중심 맞추고 전진해야 함
    changMode = false;
    robot_mode = OBSTACLE_MODE;
    break;
  } // 탈출 시
  // 그 pwa 적용하기(라이브러리)

  av_senser=(0.55*get_ult(RIGHT_TRIG, RIGHT_ECHO))+0.45*av_senser;
  //Serial.println(av_senser);
  
  //////////////// 미로 주행하기 ////////////////////
  if (av_senser > 15) {
    // Serial.println("Case 1");
    steering_control(RIGHT_STEER_ANGLE - 10);
    delay(200);
  }

  else if (av_senser < 10) {
    // Serial.println("Case 2");
    steering_control(LEFT_STEER_ANGLE + 40);
    delay(200);
  }
  //////////////// 미로 주행하기 ////////////////////
  break;

case OBSTACLE_MODE:
  // 코드 나오는 거 보고 결정
  // 장애물 탐지 코드 ==> NORMAL_MODE 랑 동일함
  // 장애물 회피코드 ==> ex) 초음파 < 15
    // 장애물 회피
    // 장애물 회피 완료 후 changeMode = NORMAL_MODE;
  break;
}}