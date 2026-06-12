#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADXL345_U.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(123);

// Ball
int ballX = 64;
int ballY = 32;
int ballR = 3;

// Target
int targetX = 30;
int targetY = 30;

// Score
int score = 0;

void setup() {
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  if (!accel.begin()) {
    while (1);   // Stop if ADXL not detected
  }

  accel.setRange(ADXL345_RANGE_2_G);
  randomSeed(analogRead(A0));
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Move ball using tilt
  ballX += event.acceleration.x * 0.5;
  ballY -= event.acceleration.y * 0.5;

  // Screen boundaries
  ballX = constrain(ballX, ballR, SCREEN_WIDTH - ballR);
  ballY = constrain(ballY, ballR, SCREEN_HEIGHT - ballR);

  // Target hit detection
  if (abs(ballX - targetX) < 6 && abs(ballY - targetY) < 6) {
    score++;
    targetX = random(10, SCREEN_WIDTH - 10);
    targetY = random(10, SCREEN_HEIGHT - 10);
  }

  // Draw everything
  display.clearDisplay();

  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.fillCircle(ballX, ballY, ballR, SSD1306_WHITE);
  display.drawCircle(targetX, targetY, 4, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print("Score:");
  display.print(score);

  display.display();
  delay(30);
}