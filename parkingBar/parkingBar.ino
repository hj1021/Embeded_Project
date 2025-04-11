#include <Servo.h> //서보 모터 라이브러리 추가

#define ECHO 6 //2번 핀을 Echo로 설정
#define TRIG 7 //3번 핀을 Trig로 설정

Servo myservo;  // 서보 모터 제어를 위한 객체 생성
int pos = 0;
// int pos1 = 0;    // 서보 모터 위치 값을 저장하는 변수 선언
// int pos2 = 120;

void setup() {
  pinMode(TRIG, OUTPUT);  //Trig핀 출력용으로 설정
  pinMode(ECHO, INPUT);   //Echo핀 입력용으로 설정
  Serial.begin(9600); //시리얼 통신 초기화

  myservo.attach(13);  // 서보 모터 제어를 위한 핀 설정
  myservo.write(pos);
}

void loop() {

  digitalWrite(TRIG, HIGH); //펄스파 발생
  delayMicroseconds(10); //10마이크로초 지연
  digitalWrite(TRIG, LOW);  //펄스파 종료
  long duration = pulseIn(ECHO, HIGH); //Echo핀이 HIGH가 된 이후 지연된 펄스의 시간을 duration 변수에 저장

  if(duration == 0) //시간이 0이면 종료 
  {
    return;
  }

  // 6cm가 적당한듯
  long distance = duration / 58; //총 걸리는 시간을 58로 나눠 거리를 측정
  Serial.print("Distance : "); //시리얼 모니터로 거리 데이터 출력
  Serial.print(distance);  
  Serial.println("cm");
  delay(500); //거리 데이터를 1초간 마다 출력
  
  if(distance <= 6) {
    if (pos < 50) {
      for (pos = 0; pos <= 120; pos += 1) { // 0 ~ 120까지 1씩 증가시켜 루프를 돌림 
      myservo.write(pos);              // pos 변수 값의 위치로 회전(1도 씩 회전)
      delay(15);                       // 서보 모터가 위치까지 도달할 수 있도록 15ms 대기
      }
    }
  }
  else {
    if (pos> 100) {
      for (pos = 120; pos >= 0; pos -= 1) { // 120 ~ 0까지 1씩 감소시켜 루프를 돌림
      myservo.write(pos);              
      delay(15);
      }                       
    }
  }
  
  /*
  for (pos = 0; pos <= 120; pos += 1) { // 0 ~ 120까지 1씩 증가시켜 루프를 돌림 
    myservo.write(pos);              // pos 변수 값의 위치로 회전(1도 씩 회전)
    delay(5);                       // 서보 모터가 위치까지 도달할 수 있도록 15ms 대기
  }
  for (pos = 120; pos >= 0; pos -= 1) { // 120 ~ 0까지 1씩 감소시켜 루프를 돌림
    myservo.write(pos);              
    delay(5);                       
  }
  */
  
  
}