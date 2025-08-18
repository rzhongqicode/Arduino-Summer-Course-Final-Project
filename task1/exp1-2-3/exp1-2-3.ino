#include <OneButton.h>
#include <GyverOLED.h>
#define ADD_PIN 10
#define MINUS_PIN 11


OneButton add_button = OneButton(ADD_PIN, true);
OneButton minus_button = OneButton(MINUS_PIN, true);

GyverOLED<SSH1106_128x64> oled;
int number = 100;
void add_clicked(){
  number++;
  int number_len = String(number).length() * 6;
  int startX = (128 - number_len) / 2;
  oled.clear();
  oled.setCursorXY(startX, 30);
  oled.println((String)number); 
  oled.update();

}
void minus_clicked(){
  number--;
  int number_len = String(number).length() * 6;
  int startX = (128 - number_len) / 2;
  oled.clear();
  oled.setCursorXY(startX, 30);
  oled.println((String)number);
  oled.update();  
}

void setup() {
  Serial.begin(9600);
  oled.init(0x3C); // 确保地址正确                        
  oled.clear(); 
  oled.update();

  int number_len = String(number).length() * 6;
  int startX = (128 - number_len) / 2;
  oled.setCursorXY(startX, 30);
  oled.println((String)number);
  oled.update();  

  add_button.attachClick(add_clicked);
  minus_button.attachClick(minus_clicked);
}

void loop() {
  add_button.tick();
  minus_button.tick();
}
