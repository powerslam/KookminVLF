#define FRONT_TRIG 2
#define FRONT_ECHO 3

#define LEFT_TRIG 48
#define LEFT_ECHO 49

#define RIGHT_TRIG 46
#define RIGHT_ECHO 47

void setup() {
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
}

void loop() {
  long duration, distance;

  // 전면 초음파 /////////////////////////////////////////////////////
  digitalWrite(FRONT_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(FRONT_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(FRONT_TRIG, LOW);

  duration = pulseIn(FRONT_ECHO, HIGH);

  distance = duration * 17 / 1000; 
  Serial.print("Distance 1 : ");
  Serial.print(distance);
  Serial.print(" Cm, ");

  // 좌측 ///////////////////////////////////////////////////////////////
  digitalWrite(LEFT_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(LEFT_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(LEFT_TRIG, LOW);

  duration = pulseIn(LEFT_ECHO, HIGH);

  distance = duration * 17 / 1000; 
  Serial.print("Distance 2 : ");
  Serial.print(distance);
  Serial.print(" Cm, ");

  // 우측 ///////////////////////////////////////////////////////////////
  digitalWrite(RIGHT_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(RIGHT_TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(RIGHT_TRIG, LOW);

  duration = pulseIn(RIGHT_ECHO, HIGH);

  distance = duration * 17 / 1000; 
  Serial.print("Distance 3 : ");
  Serial.print(distance);
  Serial.println(" Cm, ");
  delay(1000);
}