#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include <stdlib.h>  // For random()

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void runGearAnimation() {
  const int radius = 8;
  const int gearY = SCREEN_HEIGHT - radius - 2;
  const int numTeeth = 8;
  const char* name = "McCunn";
  int textSize = 2;

  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  int16_t x1, y1;
  uint16_t textW, textH;
  display.getTextBounds(name, 0, 0, &x1, &y1, &textW, &textH);
  int stopX = (SCREEN_WIDTH - textW) / 2;

  float angle = 0.0;
  const int offset = 32;

  for (int gearX = SCREEN_WIDTH + radius; gearX >= -radius - 4; gearX--) {
    display.clearDisplay();

    // Draw trail line (like measuring tape)
    display.drawLine(gearX + radius, gearY + radius + 1, SCREEN_WIDTH, gearY + radius + 1, SSD1306_WHITE);

    // Draw gear body
    display.fillCircle(gearX, gearY, radius, SSD1306_WHITE);

    // Draw gear teeth
    for (int i = 0; i < numTeeth; i++) {
      float toothAngle = angle + i * (2 * PI / numTeeth);
      int x1 = gearX + radius * cos(toothAngle);
      int y1 = gearY - radius * sin(toothAngle);
      int x2 = gearX + (radius + 2) * cos(toothAngle);
      int y2 = gearY - (radius + 2) * sin(toothAngle);
      display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }

    // Small center circle for gear axis
    display.fillCircle(gearX, gearY, 1, SSD1306_BLACK);

    angle += 0.2; // rotation increment

    // Move and stop name in the center
    int textX = gearX + offset;
    if (textX > stopX) {
      display.setCursor(textX, 6);
    } else {
      display.setCursor(stopX, 6);
    }
    display.print(name);

    display.display();
    delay(10);
  }

  delay(500);
  fadeOutRandomPixels(display);
  display.clearDisplay();
  display.display();
}

void fadeOutRandomPixels(Adafruit_SSD1306 &display) {
  uint8_t* buffer = display.getBuffer();
  int width = display.width();
  int height = display.height();
  int bufferSize = (width * height) / 8;  // Each byte is 8 vertical pixels

  while (true) {
    bool anyPixelsLeft = false;

    // Randomly try turning off some pixels
    for (int i = 0; i < 512; i++) { // try 50 pixels per frame
      int byteIndex = random(bufferSize);
      uint8_t b = buffer[byteIndex];

      if (b != 0) {  // If this byte has any pixels ON
        anyPixelsLeft = true;

        // Pick a random bit set in this byte to clear
        int bitToClear = random(8);
        if (b & (1 << bitToClear)) {
          buffer[byteIndex] &= ~(1 << bitToClear); // turn off that pixel
        }
      }
    }

    display.display();
    delay(50);

    if (!anyPixelsLeft) break;  // Stop when no pixels remain ON
  }
}

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  runGearAnimation();
}

void loop() {
  // Main loop
}
