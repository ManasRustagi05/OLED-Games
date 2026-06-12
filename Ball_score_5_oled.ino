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

// Game state
bool gameOver = false;
bool gameWin = false;

void resetGame() {
  ballX = 64;
  ballY = 32;
  score = 0;
  targetX = random(10, SCREEN_WIDTH - 10);
  targetY = random(10, SCREEN_HEIGHT - 10);
  gameOver = false;
  gameWin = false;
}

void setup() {
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  if (!accel.begin()) {
    while (1);
  }

  accel.setRange(ADXL345_RANGE_2_G);
  randomSeed(analogRead(A0));
}

void loop() {

  // LOSE screen → restart after 2 seconds
  if (gameOver) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 25);
    display.print("YOU LOSE");
    display.display();

    delay(2000);
    resetGame();
    return;
  }

  // WIN screen → stop game
  if (gameWin) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 25);
    display.print("YOU WIN");
    display.display();
    while (1);
  }

  sensors_event_t event;
  accel.getEvent(&event);

  // Move ball using tilt
  ballX += event.acceleration.x * 0.5;
  ballY -= event.acceleration.y * 0.5;

  // Boundary hit → LOSE
  if (ballX <= ballR || ballX >= SCREEN_WIDTH - ballR ||
      ballY <= ballR || ballY >= SCREEN_HEIGHT - ballR) {
    gameOver = true;
  }

  // Target hit → score
  if (abs(ballX - targetX) < 6 && abs(ballY - targetY) < 6) {
    score++;
    targetX = random(10, SCREEN_WIDTH - 10);
    targetY = random(10, SCREEN_HEIGHT - 10);

    if (score >= 5) {
      gameWin = true;
    }
  }

  // Draw game
  display.clearDisplay();
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.fillCircle(ballX, ballY, ballR, SSD1306_WHITE);
  display.drawCircle(targetX, targetY, 4, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(2, 2);
  display.print("Score:");
  display.print(score);

  display.display();
  delay(30);
}