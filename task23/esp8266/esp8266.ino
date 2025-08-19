#include <U8g2lib.h>
#include <SoftwareSerial.h>
#define SCL_PIN 5
#define SDA_PIN 4
#define RX_PIN 13
#define TX_PIN 15
#define ENCODER_A 12
#define ENCODER_B 14

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN, U8X8_PIN_NONE);
SoftwareSerial arduinoSerial(RX_PIN, TX_PIN);
void setup() {
  
}

void loop() {
  
}
