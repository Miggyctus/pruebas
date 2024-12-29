#include"TFT_eSPI.h"

int X = 160;
int Y = 120;
int R = 10;
// Screen and TFT object
TFT_eSPI tft;
#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
void setup() {
  // Initialize serial communication and pins
  Serial.begin(115200);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(D8, OUTPUT);

  // Initialize the display
  tft.begin();
  tft.setRotation(3);   // Rotate display for landscape
  tft.fillScreen(TFT_BLACK);  // Clear the screen with black
}

void loop() {
  // Use the 5-way switch to draw different sections of the circle
  if (digitalRead(WIO_5S_UP) == LOW && Y > 10) {
    tft.fillScreen(TFT_BLACK);
    Y = Y - 5;
  }
  if (digitalRead(WIO_5S_DOWN) == LOW && Y < 230) {
    tft.fillScreen(TFT_BLACK);
    Y = Y + 5;
  }
  if (digitalRead(WIO_5S_LEFT) == LOW && X > 10) {
    tft.fillScreen(TFT_BLACK);
    X = X - 5;
  }
  if (digitalRead(WIO_5S_RIGHT) == LOW && X < 305) {
    tft.fillScreen(TFT_BLACK);
    X = X + 5;
  }
  if(X == 305 || X == 10 || Y == 230 || Y == 10)
  {
    digitalWrite(D8, HIGH);
    analogWrite(WIO_BUZZER, 128);
    analogWrite(WIO_BUZZER, 96);
    analogWrite(WIO_BUZZER, 64);
    analogWrite(WIO_BUZZER, 32);
    delay(100);
    analogWrite(WIO_BUZZER, 64);
    analogWrite(WIO_BUZZER, 96);
  }
  else
  {
    analogWrite(WIO_BUZZER, 0);
    digitalWrite(D8, LOW);
  }
  tft.drawCircle(X, Y, R, TFT_WHITE);
  delay(200);  // Delay to debounce the button press
  
}