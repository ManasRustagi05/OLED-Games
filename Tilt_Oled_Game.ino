#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADXL345_U.h>

#define W 128
#define H 64

Adafruit_SSD1306 display(W, H, &Wire, -1);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Player
int px = 10, py = 10;
int r = 3;

// Stars
int stars[3][2] = { {110,10}, {60,30}, {110,50} };
bool collected[3] = { false, false, false };

// Walls (x1,y1,x2,y2)
int walls[4][4] = {
  {20, 0, 20, 45},
  {40, 20, 40, 64},
  {70, 0, 70, 40},
  {90, 25, 90, 64}
};

bool gameWon = false;

void setup() {
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  accel.begin();
  accel.setRange(ADXL345_RANGE_4_G);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 28);
  display.print("TILT GAME");
  display.display();
  delay(1500);
}

void loop() {

  // WIN SCREEN
  if (gameWon) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 22);
    display.print("YOU WIN");
    display.display();
    while (1);   // STOP GAME
  }

  sensors_event_t e;
  accel.getEvent(&e);

  float dx = e.acceleration.x * 0.4;
  float dy = e.acceleration.y * 0.4;

  px += dx;
  py -= dy;

  px = constrain(px, r, W - r);
  py = constrain(py, r, H - r);

  // Wall collision
  for (int i = 0; i < 4; i++) {
    if (px > walls[i][0] - r && px < walls[i][0] + r &&
        py > walls[i][1] && py < walls[i][3]) {
      px -= dx;
      py += dy;
    }
  }

  // Collect stars
  for (int i = 0; i < 3; i++) {
    if (!collected[i] &&
        abs(px - stars[i][0]) < 5 &&
        abs(py - stars[i][1]) < 5) {
      collected[i] = true;
    }
  }

  // Check win
  if (collected[0] && collected[1] && collected[2]) {
    gameWon = true;
    return;
  }

  // Draw game
  display.clearDisplay();

  for (int i = 0; i < 4; i++)
    display.drawLine(walls[i][0], walls[i][1],
                     walls[i][2], walls[i][3], WHITE);

  for (int i = 0; i < 3; i++)
    if (!collected[i])
      display.fillCircle(stars[i][0], stars[i][1], 2, WHITE);

  display.fillCircle(px, py, r, WHITE);
  display.display();

  delay(20);
}