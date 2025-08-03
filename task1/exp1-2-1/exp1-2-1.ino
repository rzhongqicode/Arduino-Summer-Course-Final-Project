#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11
unsigned long prev_time = 0;
int interval_time = 50;
int direction = 1;
int step = 5;
int analog_value = 0;
int state = 0;

void setup() {
  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  analogWrite(RED_PIN,0);
  analogWrite(GREEN_PIN,0);
  analogWrite(BLUE_PIN,0);
  prev_time = millis();
}

void loop() {
  int pin = 0;
  switch(state){
    case 0: pin = RED_PIN; break;
    case 1: pin = GREEN_PIN; break;
    case 2: pin = BLUE_PIN; break;
  }

  if(millis() - prev_time >= interval_time){
    analog_value += direction * step;
    prev_time = millis();
    if(analog_value >= 255){
      direction = -1;
      analog_value = 255;
    }
    if(analog_value <= 0){
      direction = 1;
      analog_value = 0;
      state = (state + 1)%3;
    }
    analogWrite(pin, analog_value);
  }
}
