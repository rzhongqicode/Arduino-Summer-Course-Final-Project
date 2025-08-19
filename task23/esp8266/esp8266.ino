#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include "bitmap.h"
#define SCL_PIN 5
#define SDA_PIN 4
#define RX_PIN 13
#define TX_PIN 15
#define ENC_A 12
#define ENC_B 14
//判断当前所处的状态的变量
bool inMenu = false;
bool inFunction = false;
bool threshold = false;
int distance_threshold = 15;
//与菜单相关量
const char* menu_str[] = {"Rangefinder", "Voltage Meter", "Oscilloscope", "Music Player", "Flashlight"};
const char* menu_mark[] = {"R", "V", "O", "M", "F"};
int menu_idx = 0;
//与编码器相关变量
volatile int cnt = 0;
const int encoder_ppr = 13, gear_ratio = 30;
const int counts_per_rev = encoder_ppr * gear_ratio * 4;
//变量值
float distance = 0.0;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN, U8X8_PIN_NONE);
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN);
//编码器计数中断程序
void count_1(){
  if(digitalRead(ENC_B) == LOW)
  {
    cnt++;
  }
  else{
    cnt--;
  }
}

void count_2(){
  if(digitalRead(ENC_B) == LOW)
  {
    cnt--;
  }
  else{
    cnt++;
  }
}

void count_3(){
  if(digitalRead(ENC_A) == HIGH)
  {
    cnt++;
  }
  else{
    cnt--;
  }
}

void count_4(){
  if(digitalRead(ENC_A) == LOW)
  {
    cnt++;
  }
  else{
    cnt--;
  }
}

void calculate_roller(){
  while(cnt < 0){
    cnt += counts_per_rev;
  }
  while(cnt >= counts_per_rev){
    cnt -= counts_per_rev;
  }
  if(inMenu){
    menu_idx = int(cnt / (counts_per_rev / 5));
  }
}


void setup() {
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  //配置内置LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(ENC_A), count_1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A), count_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_3, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_4, FALLING);

  arduinoSerial.begin(9600);
  //开机动画和欢迎界面
  u8g2.begin();
  u8g2.enableUTF8Print();
  for (int i = -32; i < (128 - 32) / 2; i += 4) {
    u8g2.clearBuffer();
    u8g2.drawXBMP(i, (64 - 32) / 2, 32, 32, epd_bitmap_icon_cube_00);
    u8g2.sendBuffer();
    delay(20);
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(20, 40, "Welcome!");
  u8g2.sendBuffer();
  delay(1500);
  digitalWrite(LED_BUILTIN, HIGH);
  u8g2.setDrawColor(1); 
  u8g2.setBitmapMode(1); 
  u8g2.setFontMode(1);
}

void loop() {
  
}
