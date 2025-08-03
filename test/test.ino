#include <GyverOLED.h>
GyverOLED<SSH1106_128x64> oled;
int dot_state = 0;
void setup() {
  // put your setup code here, to run once:
  oled.init();
  oled.clear();
  uint8_t dot_y = 3 + 12 * dot_state;
  oled.circle(20,dot_y,2,1);
  oled.setCursorXY(30, 0);
  oled.print("Rangefinder");
  oled.setCursorXY(30, 12);
  oled.print("Voltage Meter");
  oled.setCursorXY(30, 24);
  oled.print("Oscilloscope");
  oled.setCursorXY(30, 36);
  oled.print("Music Player");
  oled.setCursorXY(30, 48);
  oled.print("Flashlight");
  oled.update();
}

void loop() {
  // put your main code here, to run repeatedly:

}
