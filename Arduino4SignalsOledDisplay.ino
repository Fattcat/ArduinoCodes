#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

String inputString = "";
enum SignalType { NONE, SINE, SQUARE, SAW, TRIANGLE };
SignalType currentSignal = NONE;

unsigned long lastToggle = 0;
unsigned long squareToggleInterval = 500;
bool squareState = false;

const int outputInterval = 20;
unsigned long lastOutputTime = 0;

float angle = 0;
const int sineInterval = 20;
unsigned long lastSineUpdate = 0;

int sawValue = 0;
int triangleValue = 0;
int triangleStep = 20;

int x = 0;
int prevY = SCREEN_HEIGHT / 2;

void setup() {
  Serial.begin(9600);
  delay(500);
  inputString.reserve(20);

  Serial.println("~Napíš 'sinus', 'obdlznik', 'saw', 'triangle' alebo 'stop'.");

  Wire.begin();  // Dôležité pre OLED

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Nepodarilo sa nájsť OLED displej");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pripraveny...");
  display.display();
}

void loop() {
  // Čítanie príkazu zo sériového vstupu
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      processCommand(inputString);
      inputString = "";
    } else {
      inputString += inChar;
    }
  }

  unsigned long currentMillis = millis();

  // Obdĺžnik - prepínanie
  if (currentSignal == SQUARE && currentMillis - lastToggle >= squareToggleInterval) {
    lastToggle = currentMillis;
    squareState = !squareState;
  }

  // Generovanie signálu
  if (currentSignal != NONE && currentMillis - lastOutputTime >= outputInterval) {
    lastOutputTime = currentMillis;
    int value = 0;

    switch (currentSignal) {
      case SINE:
        if (currentMillis - lastSineUpdate >= sineInterval) {
          lastSineUpdate = currentMillis;
          value = 512 + 511 * sin(angle);
          angle += 0.1;
          if (angle >= 2 * PI) angle = 0;
        }
        break;

      case SQUARE:
        value = squareState ? 1023 : 0;
        break;

      case SAW:
        value = sawValue;
        sawValue += 20;
        if (sawValue > 1023) sawValue = 0;
        break;

      case TRIANGLE:
        value = triangleValue;
        triangleValue += triangleStep;
        if (triangleValue >= 1023 || triangleValue <= 0) triangleStep = -triangleStep;
        break;

      default:
        value = 0;
        break;
    }

    int y = map(value, 0, 1023, SCREEN_HEIGHT - 1, 0);
    display.drawLine(x, prevY, x, y, SSD1306_WHITE);
    prevY = y;

    x++;
    if (x >= SCREEN_WIDTH) {
      x = 0;
      display.clearDisplay();
    }

    display.display();
  }
}

void processCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  Serial.print(">> Prikaz: ");
  Serial.println(cmd);

  angle = 0;
  sawValue = 0;
  triangleValue = 0;
  triangleStep = 20;
  squareState = false;
  x = 0;
  prevY = SCREEN_HEIGHT / 2;

  display.clearDisplay();
  display.setCursor(0, 0);

  if (cmd == "sinus") {
    currentSignal = SINE;
    display.println("SINUS");
  } else if (cmd == "obdlznik") {
    currentSignal = SQUARE;
    display.println("OBDLZNIK");
  } else if (cmd == "saw") {
    currentSignal = SAW;
    display.println("SAW");
  } else if (cmd == "triangle") {
    currentSignal = TRIANGLE;
    display.println("TRIANGLE");
  } else if (cmd == "stop") {
    currentSignal = NONE;
    display.println("STOP");
  } else {
    display.println("NEZNAMY PRIKAZ");
    Serial.println(">> Neznamy prikaz");
  }

  display.display();
}
