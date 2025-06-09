#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int redPin = 25;
const int greenPin = 26;
const int bluePin = 27;

const int potPin = 34;

const int nextButtonPin = 4;
const int prevButtonPin = 5;

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;

struct Color {
  uint8_t r, g, b;
};

Color colors[] = {
  {255, 0, 0},     // Red
  {0, 255, 0},     // Green
  {0, 0, 255},     // Blue
  {255, 255, 0},   // Yellow
  {0, 255, 255},   // Cyan
  {255, 0, 255},   // Magenta
  {255, 255, 255}  // White
};

const char* colorNames[] = {
  "Red",
  "Green",
  "Blue",
  "Yellow",
  "Cyan",
  "Magenta",
  "White"
};

const int numColors = sizeof(colors) / sizeof(colors[0]);

int colorIndex = 0;
int brightness = 0;
int lastBrightness = -1;

unsigned long lastNextButtonPress = 0;
unsigned long lastPrevButtonPress = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(115200);

  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);

  pinMode(nextButtonPin, INPUT_PULLUP);
  pinMode(prevButtonPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
  display.clearDisplay();
  display.display();

  brightness = map(analogRead(potPin), 0, 4095, 0, 255);
  updateLED();
  updateDisplay();
}

void loop() {
  int potValue = analogRead(potPin);
  brightness = map(potValue, 0, 4095, 0, 255);

  // Only update LED if brightness changed significantly to avoid flicker
  if (abs(brightness - lastBrightness) > 2) {
    updateLED();
    lastBrightness = brightness;
    updateDisplay();
  }

  if (digitalRead(nextButtonPin) == LOW) {
    if (millis() - lastNextButtonPress > debounceDelay) {
      colorIndex++;
      if (colorIndex >= numColors) colorIndex = 0;
      updateLED();
      updateDisplay();
      lastNextButtonPress = millis();
    }
  }

  if (digitalRead(prevButtonPin) == LOW) {
    if (millis() - lastPrevButtonPress > debounceDelay) {
      colorIndex--;
      if (colorIndex < 0) colorIndex = numColors - 1;
      updateLED();
      updateDisplay();
      lastPrevButtonPress = millis();
    }
  }

  delay(50);
}

void updateLED() {
  uint8_t r = (colors[colorIndex].r * brightness) / 255;
  uint8_t g = (colors[colorIndex].g * brightness) / 255;
  uint8_t b = (colors[colorIndex].b * brightness) / 255;

  ledcWrite(redChannel, r);
  ledcWrite(greenChannel, g);
  ledcWrite(blueChannel, b);
}

void updateDisplay() {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(colorNames[colorIndex]);
  display.print(" ");
  display.print(colorIndex + 1);
  display.print("/");
  display.println(numColors);

  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("R: ");
  display.print(colors[colorIndex].r);
  display.print(" G: ");
  display.print(colors[colorIndex].g);
  display.print(" B: ");
  display.println(colors[colorIndex].b);

  display.setCursor(0, 45);
  display.print("Brightness: ");
  display.println(brightness);

  display.display();
}
