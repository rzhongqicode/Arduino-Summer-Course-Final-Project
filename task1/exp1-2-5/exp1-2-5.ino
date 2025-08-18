#include <Servo.h>
#include <U8g2lib.h>

const int ENC_A = 2;  
const int ENC_B = 3;                                                                                                                                  // Encoder Channel B (must be an interrupt pin)
const int SERVO_PIN = 9;  // Servo signal pin (can be any PWM pin)

const int ENCODER_PPR = 13;      // Pulses Per Revolution for the motor's encoder
const int GEAR_RATIO = 30;       // 1:30 gear ratio
const long COUNTS_PER_OUTPUT_REV = ENCODER_PPR * 4 * GEAR_RATIO; // 13 * 4 * 30 = 1560

long cnt = 0;
long prev_angle = 0;

Servo myservo;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 进度条的位置和尺寸
const int barX = 10;
const int barY = 30;
const int barW = 100;
const int barH = 10;

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

// 绘制进度条的函数
void drawProgressBar(int progress,long roller_angle) {
  u8g2.clearBuffer();  

  // 绘制进度条外框
  u8g2.drawFrame(barX, barY, barW, barH);

  // 计算实心进度条的宽度
  int filledWidth = map(progress, 0, 100, 0, barW);
  filledWidth = constrain(filledWidth, 0, barW);

  // 绘制实心进度条
  u8g2.drawBox(barX, barY, filledWidth, barH);

  // 显示角度
  String progressText = String(roller_angle) + "deg";
  u8g2.setFont(u8g2_font_6x10_tr); // 选择一个字体
  int textWidth = u8g2.getStrWidth(progressText.c_str());
  u8g2.drawStr(barX + (barW - textWidth) / 2, barY + barH + 10, progressText.c_str());

  u8g2.sendBuffer(); // 将缓冲区内容发送到OLED显示屏
}



void setup() {
  
  u8g2.begin();

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT); //可能需要使用PULLUP
  myservo.attach(SERVO_PIN);
  myservo.write(0);

  attachInterrupt(digitalPinToInterrupt(ENC_A), count_1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A), count_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_3, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_4, FALLING);
  drawProgressBar(0,0);

}


void loop() {
  long servo_angle = map(cnt,0,COUNTS_PER_OUTPUT_REV,0,180);
  if(servo_angle != prev_angle){
    myservo.write(servo_angle);
    prev_angle = servo_angle;
    
    long roller_angle = map(cnt,0,COUNTS_PER_OUTPUT_REV,0,360);
    int progress = map(roller_angle,0,360,0,100);
    drawProgressBar(progress, roller_angle);
  }
  delay(50);
    
  }


