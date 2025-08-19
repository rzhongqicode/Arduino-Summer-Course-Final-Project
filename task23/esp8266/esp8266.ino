#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include "bitmap.h"
#define SCL_PIN 5
#define SDA_PIN 4
#define RX_PIN 13
#define TX_PIN 15
#define ENCODER_A 12
#define ENCODER_B 14
//判断当前所处的状态的变量
bool inMenu = false;
bool inFunction = false;
bool thresholdMode = false;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN, U8X8_PIN_NONE);
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN);

void setup() {
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  //配置内置LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
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
