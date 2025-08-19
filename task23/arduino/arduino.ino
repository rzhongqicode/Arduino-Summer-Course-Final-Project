#include <OneButton.h>

const int button_1_pin = 8;
const int button_2_pin = 2;
const int red_pin = 9, green_pin = 10, blue_pin = 11;
//标定当前的状态
char cur_mode = 'W';
//W : 欢迎界面
//U : 菜单 UI
//R : 测距
//V : 电压
//O : 示波器
//M : 音乐
//F : 闪光灯

//标定Led状态的变量
int LedState = 0;
//0:熄灭
//1：微蓝
//2：微白
//3：白

OneButton button_1(button_1_pin, true);
OneButton button_2(button_2_pin, true);

//LED功能
void updateLed(){
    switch (LedState) {
    case 0: analogWrite(red_pin, 0); analogWrite(green_pin, 0); analogWrite(blue_pin, 0); break;
    case 1: analogWrite(red_pin, 0); analogWrite(green_pin, 0); analogWrite(blue_pin, 128); break;
    case 2: analogWrite(red_pin, 150); analogWrite(green_pin, 150); analogWrite(blue_pin, 70); break;
    case 3: analogWrite(red_pin, 255); analogWrite(green_pin, 255); analogWrite(blue_pin, 255); break;
  }
}
//按键1单击
void button1_singleClick(){
  if(cur_mode == 'F'){
    LedState++;
    LedState %= 4;
    updateLed();
  }
}

void setup() {
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  //绑定按键行为
  button_1.attachClick(button1_singleClick);

}

void loop() {
  button_1.tick();
  button_2.tick();

}
