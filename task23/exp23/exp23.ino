#include <GyverOLED.h>
#include <OneButton.h>
#define ENC_A  2
#define ENC_B  3
#define BUTTON1_PIN 1
#define BUTTON2_PIN 2
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11
#define ROLLER_MAX 1560

GyverOLED<SSH1106_128x64> oled;
OneButton button1 = OneButton(BUTTON1_PIN, true);
OneButton button2 = OneButton(BUTTON2_PIN, true);
bool isstart = false;
bool isconfirmed = false;
uint8_t dot_state = 0; //对应当前指示哪个菜单选项
uint8_t light_state = 0; //0:灭，1:蓝色微亮，2:白色微亮，3:白色最亮
bool processInterrupt = false;
int cnt = 0;

//显示原始主菜单
void init_mainmenu(){
  dot_state = 0;
  oled.clear();
  oled.setCursorXY(32, 10);
  oled.print("Menu");
  uint8_t dot_y = 20 + 10 * dot_state;
  oled.dot(32, dot_y);
  oled.setCursorXY(80, 20);
  oled.print("Rangefinder");
  oled.setCursorXY(80, 30);
  oled.print("Voltage Meter");
  oled.setCursorXY(80, 40);
  oled.print("Oscilloscope");
  oled.setCursorXY(80, 50);
  oled.print("Music Player");
  oled.setCursorXY(80, 60);
  oled.print("Flashlight");
}

void count_1(){
  if(!processInterrupt)return;
  else{if(digitalRead(ENC_B) == LOW)
  {
    cnt++;
    constrain(cnt, 0, ROLLER_MAX);
  }
  else{
    cnt--;
    constrain(cnt, 0, ROLLER_MAX);
  }}
}

void count_2(){
  if(!processInterrupt)return;
  else{
  if(digitalRead(ENC_B) == LOW)
  {
    cnt--;
    constrain(cnt, 0, ROLLER_MAX);
  }
  else{
    cnt++;
    constrain(cnt, 0, ROLLER_MAX);
  }
  }
}

void count_3(){
  if(!processInterrupt)return;
  else{
  if(digitalRead(ENC_A) == HIGH)
  {
    cnt++;
    constrain(cnt, 0, ROLLER_MAX);
  }
  else{
    cnt--;
    constrain(cnt, 0, ROLLER_MAX);
  }
  }
}

void count_4(){
  if(!processInterrupt)return;
  else{
  if(digitalRead(ENC_A) == LOW)
  {
    cnt++;
    constrain(cnt, 0, ROLLER_MAX);
  }
  else{
    cnt--;
    constrain(cnt, 0, ROLLER_MAX);
  }
  }
}

void button1_longpress(){
  //如果还没有开始进入到主菜单中，从欢迎界面进入主菜单
  if(!isstart){
  processInterrupt = true;
  isstart = true;
  init_mainmenu();
  }
  else{
    //从其他功能界面返回主菜单
    if(isconfirmed){
      isconfirmed = false;
      //区分从不同的功能界面返回
      if(dot_state == 0){

      }
      if(dot_state == 1){

      }
      if(dot_state == 2){

      }
      if(dot_state == 3){

      }
      if(dot_state == 4){//关闭闪光灯
        light_state = 0;
        analogWrite(RED_PIN,0);
        analogWrite(GREEN_PIN,0);
        analogWrite(BLUE_PIN,0);
      }
      //显示主菜单界面
      init_mainmenu();
      processInterrupt = true;

    }
    //从主界面返回主界面
    else{
      init_mainmenu();
      processInterrupt = true;
    }
  }
}

void button1_doubleclick(){
 
  if(dot_state == 0){

  }
  if(dot_state == 1){

  }
  if(dot_state == 2){

  }
  if(dot_state == 3){

  }
  if(dot_state == 4){
    //如果还没有确认，进入闪光灯界面
    if(isconfirmed == false){
    isconfirmed = true;
    button1.attachClick(button1_singleclick_flashlight);
    oled.clear();
    oled.setCursorXY(30,10);
    oled.print("Flashlight");
    oled.update();
    }
    //如果已经确认了，关灯
    else{
      light_state = 0;
      analogWrite(RED_PIN,0);
      analogWrite(GREEN_PIN,0);
      analogWrite(BLUE_PIN,0);
    }
  }
  //双击之后，必不会处理滚轮
  processInterrupt = false;
}
void button1_singleclick_flashlight(){
  light_state = (light_state + 1)%4;
  switch(light_state){
    case 0: analogWrite(RED_PIN,0);
            analogWrite(GREEN_PIN,0);
            analogWrite(BLUE_PIN,0);
            break;
    case 1: analogWrite(RED_PIN,0);
            analogWrite(GREEN_PIN,0);
            analogWrite(BLUE_PIN,100);
            break;
    case 2: analogWrite(RED_PIN,100);
            analogWrite(GREEN_PIN,100);
            analogWrite(BLUE_PIN,100);
            break;
    case 3: analogWrite(RED_PIN,255);
            analogWrite(GREEN_PIN,255);
            analogWrite(BLUE_PIN,255);
            break;
  }
}
void setup() {

  //欢迎界面
  //exp2.1
  oled.init();
  oled.clear();
  oled.setScale(1); // 设置文本大小为1
  oled.setCursor(15, 2); // (x, y)
  oled.print("Hello!");
  oled.update();

  button1.attachLongPressStart(button1_longpress);
  button1.attachDoubleClick(button1_doubleclick);

  attachInterrupt(digitalPinToInterrupt(ENC_A), count_1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_A), count_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_3, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), count_4, FALLING);
  
}

void loop() {
  button1.tick();

  if(isconfirmed){
    
  }
}
