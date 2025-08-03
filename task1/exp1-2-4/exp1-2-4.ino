#include <Servo.h>
const int ENC_A = 2;  
const int ENC_B = 3;  // Encoder Channel B (must be an interrupt pin)
const int SERVO_PIN = 9;  // Servo signal pin (can be any PWM pin)

const int ENCODER_PPR = 13;      // Pulses Per Revolution for the motor's encoder
const int GEAR_RATIO = 30;       // 1:30 gear ratio
const long COUNTS_PER_OUTPUT_REV = ENCODER_PPR * 4 * GEAR_RATIO; // 13 * 4 * 30 = 1560

long cnt = 0;
long prev_angle = 0;

void count_1(){
  if(digitalRead(ENC_B) == LOW)
  {
    cnt++;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
  else{
    cnt--;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
}

void count_2(){
  if(digitalRead(ENC_B) == LOW)
  {
    cnt--;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
  else{
    cnt++;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
}

void count_3(){
  if(digitalRead(ENC_A) == HIGH)
  {
    cnt++;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
  else{
    cnt--;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
}

void count_4(){
  if(digitalRead(ENC_A) == LOW)
  {
    cnt++;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
  else{
    cnt--;
    constrain(cnt,0,COUNTS_PER_OUTPUT_REV);
  }
}

Servo myservo;

void setup(){
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT); //可能需要使用PULLUP
  myservo.attach(SERVO_PIN);
  myservo.write(0);

  attachInterrupt(digitalPinToInterrupt(ENC_A), count_1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A), count_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_3, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_4, FALLING);
  
}

void loop(){
  long servo_angle = map(cnt,0,COUNTS_PER_OUTPUT_REV,0,180);

  if(servo_angle != prev_angle){
    myservo.write(servo_angle);
    prev_angle = servo_angle;
    
  }
  delay(50);

}