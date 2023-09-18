초음파 값 읽어오기, DC 모터 방향 및 속도 제어, 서보모터 제어 관련 코드를 작성한 헤더파일임.

## 설치방법
1. 내 문서 폴더 아래에 Arduino 폴더로 이동
2. Arduino 폴더 아래에 libraries 폴더로 VLF 폴더를 이동

## 사용방법 
1. pinMode 등 세팅을 위해 setup 함수에서 vlf_init 함수를 1회 호출하기
2. 구현체(.cpp)를 보면서 함수 사용

## 현재까지(23.09.18) 등록된 함수
1. void vlf_init()
   * 현재 프로그램에 사용되는 pinMode 등 기본 세팅을 하는 함수

2. long get_ult(int trig, int echo)
   * 초음파 센서의 값을 가져오는 함수
   * trig, echo의 핀 번호 는 매크로 변수로 선언되어 있음
     1) 전면 초음파: FRONT_TRIG, FRONT_ECHO
     2) 좌측 초음파: LEFT_TRIG, LEFT_ECHO
     3) 우측 초음파: RIGHT_TRIG, RIGHT_ECHO
   * 단위는 함수 내에서 cm 단위로 변환하여 반환함
  
3. motor_control(unit8_t dir, int speed)
   * DC 모터를 제어하는 함수
   * dir은 전진/후진을 제어함
   * dir의 방향제어를 위한 값은 매크로 변수로 선언되어 있음
     1) FRONT_SPIN: 전진
     2) BACK_SPIN: 후진
   * speed는 0 ~ 255 사이의 값으 주어야 함 ==> 함수 내부에 max, min 함수를 활용해서 혹시나 예외값을 방어할 수 있도록 함

4. motor_stop()
   * 모터 정지는 자주 쓰이기 때문에 따로 만들어 둠
   * motor_control() 함수를 활용함

5. steering_control(int angle)
   * 서보모터의 각도를 제어함
   * 왼쪽, 오른쪽 max 각도 및 중앙 각도에 대한 매크로 변수가 정의되어 있음
     1) LEFT_MAX_STEER_ANGLE: 왼쪽 최대 각도
     2) RIGHT_MAX_STEER_ANGLE: 오른쪽 최대 각도
     3) NEURAL_ANGLE: 중앙 각도
