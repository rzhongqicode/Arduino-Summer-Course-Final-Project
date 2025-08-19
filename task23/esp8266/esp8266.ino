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
int cur_channel_idx = 0;
float voltage = 0.0;
int progress = 0;
int analog_value[128];
unsigned long prev_pro_time = 0;
bool led_is_on = false;

byte xpos = 0; 
byte ypos = 0; 
int frame = 0; 
byte item_scale_frame = 0;
byte icon_size = 0; 
char current_message[20] = "Dashboard"; 
char selected_item_name[20] = "---"; 

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
void Uart_communicate(){
  if(arduinoSerial.available()){
    String info = arduinoSerial.readStringUntil('\n');
    info.trim();
    if(info == "ui"){
      inMenu = true;
      inFunction = false;
      threshold = false;
    }
    if(info == "confirm"){
      if(inMenu){
        inMenu = false;
        inFunction = true;
        threshold = false;
        arduinoSerial.print("Mode=");
        arduinoSerial.println(menu_mark[menu_idx]);
      }
    }
    if(info == "range_threshold"){
      if(inFunction && menu_idx == 0){
        threshold = !threshold;
      }
    }
    if(info == "change"){
      if(inFunction && menu_idx == 2){
        cur_channel_idx = (cur_channel_idx + 1) % 6;
        arduinoSerial.print("Channel=");
        arduinoSerial.println(cur_channel_idx);        
      }
    }
    if(info.startsWith("Voltage=")){
      voltage = info.substring(8).toFloat();
    }
    if(info.startsWith("Distance=")){
      distance = info.substring(9).toFloat();
    }
    if(info.startsWith("Analog=")){
      int read_value = info.substring(7).toInt();
      for(int i = 0; i < 127; i++){
        analog_value[i] = analog_value[i+1];
      }
      analog_value[127] = map(read_value, 0, 1023, 0, 44);
    }
    if(info.startsWith("Progress=")){
      progress = info.substring(9).toInt();
    }
  }
}
//rangefinder测距界面
void Rangefinder(){
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.setCursor(5, 40);
  u8g2.print(distance, 2);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.print(" cm");

  if(threshold){
    calculate_roller();
    threshold = map(cnt, 0, counts_per_rev, 10, 50);
    arduinoSerial.print("Threshold=");
    arduinoSerial.println(threshold);
  }
  u8g2.setCursor(10, 60);
  u8g2.print("Threshold: ");
  u8g2.print(threshold);
}
//VoltageMeter测电压界面,画一个dashboard
void VoltageMeter(){
  int X = 64;
  int Y = 64;
  int R = 45;
  u8g2.drawCircle(X, Y, R, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  float angle_circle = map(voltage, 0.0, 5.0, 180, 0) * 3.14159 / 180.0;
  int X_end = X - (R - 5) * cos(angle_circle);
  int Y_end = Y - (R - 5) * sin(angle_circle);
  u8g2.drawLine(X, Y, X_end, Y_end);
  u8g2.setFont(u8g2_font_logisoso16_tr);
  u8g2.setCursor(30, 40);
  u8g2.print(voltage, 2);
  u8g2.print(" V");  
}
//Oscilloscope示波器界面
void Oscilloscope(){
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  u8g2.drawLine(20, 10, 20, 54); 
  u8g2.drawLine(20, 54, 118, 54); 
  u8g2.drawLine(18, 12, 20, 10); 
  u8g2.drawLine(22, 12, 20, 10); 
  u8g2.drawLine(116, 52, 118, 54); 
  u8g2.drawLine(116, 56, 118, 54); 
  for (int i = 1; i < 128; i++) {
    int x1 = 20 + i - 1;
    int y1 = 54 - analog_value[i - 1]; 
    int x2 = 20 + i;
    int y2 = 54 - analog_value[i]; 
    x1 = constrain(x1, 20, 118); 
    x2 = constrain(x2, 20, 118); 
    y1 = constrain(y1, 10, 54); 
    y2 = constrain(y2, 10, 54); 
    u8g2.drawLine(x1, y1, x2, y2);
  }
  u8g2.setCursor(20, 62); 
  u8g2.print("Ch: A");
  u8g2.print(cur_channel_idx);
}
//音乐播放器界面
void MusicPlayer(){
  //画进度条
  if(progress > 0 && progress < 17){
    if(millis() - prev_pro_time > 200){
      led_is_on = !led_is_on;
      digitalWrite(LED_BUILTIN, led_is_on);
      prev_pro_time = millis();
    }
    else{
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  int barwidth = map(progress, 0, 17, 0, 100);
  u8g2.drawFrame(14, 35, 102, 12);
  u8g2.drawBox(15, 36, barwidth, 10);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(15, 30);
  u8g2.print("Playing Music...");
  u8g2.setCursor(50, 60);

  if (progress > 0 && progress < 17) {
    u8g2.print("Playing");
  } else {
     u8g2.print("Paused");
  }
}
//闪光灯界面
void Flashlight(){
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(10, 40);
  u8g2.print("Click BTN1 on UNO");
  u8g2.setCursor(20, 55);
  u8g2.print("to change brightness.");
}

//根据menu_idx绘制功能界面
void FunctionUI(){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 10, menu_str[menu_idx]);
  u8g2.drawHLine(0, 12, 128);
  switch (menu_idx) {
  case 0: Rangefinder(); break;
  case 1: VoltageMeter(); break;
  case 2: Oscilloscope(); break;
  case 3: MusicPlayer(); break;
  case 4: Flashlight(); break;
  }
  u8g2.sendBuffer();
}
//绘制主界面
void MainMenu(){
  frame = int(cnt * 150 / counts_per_rev) % 150;
  if (frame >= 0 && frame < 30) {strcpy(selected_item_name, menu_str[0]);menu_idx=0;}
  else if (frame >= 30 && frame < 60) {strcpy(selected_item_name, menu_str[1]);menu_idx=1;}
  else if (frame >= 60 && frame < 90) {strcpy(selected_item_name, menu_str[2]);menu_idx=2;}
  else if (frame >= 90 && frame < 120) {strcpy(selected_item_name, menu_str[3]);menu_idx=3;}
  else {strcpy(selected_item_name, menu_str[4]);menu_idx=4;}
  u8g2.firstPage();
  do {
  for (int i = 0; i < 5; i++) { 
    int item_frame = (frame + (30*i)) % 150; 
    if ((item_frame >= 0) && (item_frame <= 18)) { 
      item_scale_frame = (item_frame / 2);
      icon_size = 32;
    } else if ((item_frame > 132) && (item_frame <= 150)) {
      item_scale_frame = (150 - item_frame) / 2;
      icon_size = 32;
    } else {
      item_scale_frame = 10;
      icon_size = 16; 
    }
    xpos = menu_positions[item_frame][0];
    ypos = menu_positions[item_frame][1];
  u8g2.drawXBMP( xpos-icon_size/2, ypos-icon_size/2, icon_size, icon_size, bitmap_icons_array[item_scale_frame + (i * 11)]);
  }
  u8g2.setFont(u8g2_font_helvR08_tr); 
  strcpy(current_message, selected_item_name); 
  int cut_string_length = round(abs(cos(frame * 3.141592654 / 30.0 )) * 11); 
  current_message[cut_string_length] = 0x00; 
  int w = u8g2.getStrWidth(current_message); 
  u8g2.drawStr(64 - w/2, 64, current_message); 

  u8g2.drawXBMP(128-16-4, 0, 16, 4, upir_logo);  

  } while ( u8g2.nextPage() ); 
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
  Uart_communicate();
    if (inFunction) {
    FunctionUI();
  } else if (inMenu) {
    calculate_roller();
    MainMenu();
  } else {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "Long press BTN1");
    u8g2.drawStr(25, 50, "to enter menu");
    u8g2.sendBuffer();
  }
}
