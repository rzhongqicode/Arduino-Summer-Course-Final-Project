#include <OneButton.h>
#include <Servo.h>
const int button_1_pin = 8;
const int button_2_pin = 2;
const int red_pin = 9, green_pin = 10, blue_pin = 11;
const int buzz_pin = 3;
const int trig_pin = 5;
const int echo_pin = 4;
const int servo_pin = 6;

//标定当前的状态
char cur_mode = 'W';
//W : 欢迎界面
//U : 菜单 UI
//R : 测距
//V : 电压
//O : 示波器
//M : 音乐
//F : 闪光灯
bool threshold = false;
bool music_is_on = false;

//标定Led状态的变量
int LedState = 0;
//0:熄灭
//1：微蓝
//2：微白
//3：白

//音乐播放有关量
int cur_beat = 0;//标定当前第几个音节
unsigned long prev_beatTime = 0;//标定上个音节的开始时间
const int beats[] = {784, 784, 880, 587, 523, 523, 440, 587, 784, 880, 1046, 880, 784, 523, 523, 440, 587};
const int durations[] = {600, 300, 300, 1200, 600, 300, 300, 1200, 600, 300, 300, 300, 300, 600, 300, 300, 1200};
int total_beats = sizeof(beats) / sizeof(int);
unsigned long prev_progress_time = 0;

//蓝灯呼吸有关量
unsigned long prev_breath_time = 0;
int interval_bearth = 50;
int step_breath = 10;
int direction_breath = 1;
int breath_value = 0;

//与测距,警报有关量
int distance_threshold = 15;
bool buzz_on = false;
unsigned long prev_buzz_time = 0;
int buzz_on_time = 100;
int buzz_off_time = 100;

//测电压有关量
const int channels[] = {A0, A1, A2, A3, A4, A5};

//示波器有关量
int cur_channel_idx = 0;


OneButton button_1(button_1_pin, true);
OneButton button_2(button_2_pin, true);
Servo myservo;

//LED功能
void updateLed(){
    switch (LedState) {
    case 0: analogWrite(red_pin, 0); analogWrite(green_pin, 0); analogWrite(blue_pin, 0); break;
    case 1: analogWrite(red_pin, 0); analogWrite(green_pin, 0); analogWrite(blue_pin, 128); break;
    case 2: analogWrite(red_pin, 150); analogWrite(green_pin, 150); analogWrite(blue_pin, 70); break;
    case 3: analogWrite(red_pin, 255); analogWrite(green_pin, 255); analogWrite(blue_pin, 255); break;
  }
}
//蓝灯呼吸功能
void blueLed_breathe(){
  if(millis() - prev_breath_time >= interval_bearth){
    breath_value += direction_breath * step_breath;
    prev_breath_time = millis();
    if(breath_value >= 255){
      direction_breath = -1;
      breath_value = 255;
    }
    if(breath_value <= 0){
      direction_breath = 1;
      breath_value = 0;
    }
    analogWrite(blue_pin, breath_value);
  }
}


//RangeFinder测距功能
void RangeFinder(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  float duration_time = pulseIn(echo_pin, HIGH, 30000); 
  float distance = duration_time * 0.0344 / 2;

  //控制LED灯
  if(threshold){//在阈值模式
    blueLed_breathe();
  }
  else{//在测距模式
    digitalWrite(blue_pin, LOW);

    //在测距模式下，向esp发送距离值
    Serial.print("Distance=");
    Serial.println(distance, 2);

    //报警功能
    if(distance < distance_threshold){
      digitalWrite(green_pin, LOW);
      buzz_on_time = map(distance*100, distance_threshold * 100, 0, 500, 30);

      buzz_off_time = map(distance*100, distance_threshold * 100, 0, 150, 30);
      if(buzz_on){
        if(millis() - prev_buzz_time > buzz_on_time){
          noTone(buzz_pin);
          buzz_on = !buzz_on;
          prev_buzz_time = millis();
          digitalWrite(red_pin, LOW);
        }
      }
      else{
        if(millis() - prev_buzz_time > buzz_off_time){
          tone(buzz_pin, 800);
          buzz_on = !buzz_on;
          prev_buzz_time = millis();
          digitalWrite(red_pin, HIGH);
        }
      }
    }
    else{
      noTone(buzz_pin);
      digitalWrite(red_pin, LOW);
      digitalWrite(green_pin, HIGH);
      buzz_on = false;
    }
  }
}
//voltage meter测电压功能
void VoltageMeter(){
  int cur_channel = channels[0];
  int analog_value = analogRead(cur_channel);
  float voltage = analog_value * 5.0 / 1023.0;
  int angle = map(voltage, 0, 5, 10, 170);
  myservo.write(angle);
  Serial.print("Voltage=");
  Serial.println(voltage,2);
}

//示波器测电压功能
void Oscilloscope(){
  int cur_channel = channels[cur_channel_idx];
  int analog_value = analogRead(cur_channel);
  float voltage = analog_value * 5.0 / 1023.0;
  Serial.print("Analog=");
  Serial.println(voltage);
}

//音乐播放功能
void MusicPlayer(){
  if(music_is_on){
    if(millis() - prev_changeBeat_time > durations[cur_beat]){
      cur_beat += 1;
      prev_changeBeat_time = millis();
      if(cur_beat < total_beats){
        tone(buzz_pin, beats[cur_beat]);
        if(millis() - prev_progress_time >= 300){
          Serial.print("Progress=");
          Serial.println(cur_beat);
          prev_progress_time = millis();
        }
      }
      else{//关闭音乐
        music_is_on = false;
        noTone(buzz_pin);
        cur_beat = 0;
        Serial.print("Progress=");
        Serial.println(cur_beat);
      }
    }
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
void button1_doubleClick(){
  if(cur_mode == 'U'){//在主菜单
    Serial.println("c"); //发送c (confirm)确定当前功能, 此处需要确定当前cur_mode？？？
  }
  if(cur_mode == 'F'){//在闪光灯
    digitalWrite(red_pin, LOW);
    digitalWrite(blue_pin, LOW);
    digitalWrite(green_pin, LOW);
  }
}
void button1_longpress(){
  Serial.println("u"); //发送u, ui菜单，返回主菜单
  cur_mode = 'U';
}
void button2_longpress()
{
  if(cur_mode == 'R'){//在测距模式
    Serial.println("r");
    threshold = !threshold;
  }
}
void button2_singleClick(){
   if(cur_mode == 'O'){//在示波器
    Serial.println("c"); //发送c， change改变通道
  }
  if(cur_mode == 'M'){//在音乐
    if(music_is_on){
      noTone(buzz_pin);
    }
    else{
      cur_beat = 0;
      prev_beatTime = millis();
    }
    music_is_on = !music_is_on;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(buzz_pin, OUTPUT);
  //绑定按键行为
  //按键1单击
  button_1.attachClick(button1_singleClick);
  //按键1双击
  button_1.attachDoubleClick(button1_doubleClick);
  //按键1长按
  button_1.attachLongPressStop(button1_longpress);
  //按键2长按
  button_2.attachLongPressStop(button2_longpress);
  //按键2单击
  button_2.attachClick(button2_singleClick);
  //绑定舵机
  myservo.attach(servo_pin);
  myservo.write(10);//初始化
}


void loop() {
  button_1.tick();
  button_2.tick();
}
