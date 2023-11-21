#define MOTOR_DIR 4
#define MOTOR_PWM 5

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
}

void motor_control(bool dir, int speed){
  speed = max(speed, 0);
  speed = min(speed, 255);

  digitalWrite(MOTOR_DIR, dir);
  analogWrite(MOTOR_PWM, speed);
}

void motor_stop(){
  motor_control(LOW, 0);
}

void loop() {
  motor_stop();
  delay(800);
  motor_control(HIGH, 50);
  delay(800);
}
