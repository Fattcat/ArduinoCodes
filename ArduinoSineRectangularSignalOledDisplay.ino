#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Displej nastavenia
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Príkazy
String inputString = "";
bool generateSine = false;
bool generateSquare = false;

// Ovládanie signálov
unsigned long lastToggle = 0;
unsigned long squareToggleInterval = 500;
bool squareState = false;

const int outputInterval = 20;
unsigned long lastOutputTime = 0;

float angle = 0;
const int sineInterval = 20;
unsigned long lastSineUpdate = 0;

int x = 0;             // pozícia kreslenia v osi X
int prevY = SCREEN_HEIGHT / 2;  // posledná Y pozícia pre plynulé vykreslenie

void setup() {
  Serial.begin(9600);
  inputString.reserve(20);
  Serial.println("Napíš 'obdlznik', 'sinus' alebo 'stop'.");

  // Inicializácia displeja
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
  // Čítanie príkazu
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

  // Prepínanie obdĺžnika
  if (generateSquare && currentMillis - lastToggle >= squareToggleInterval) {
    lastToggle = currentMillis;
    squareState = !squareState;
  }

  // Generovanie a vykreslenie
  if ((generateSquare || generateSine) && currentMillis - lastOutputTime >= outputInterval) {
    lastOutputTime = currentMillis;

    int value = 0;

    if (generateSquare) {
      value = squareState ? 1023 : 0;
    } else if (generateSine && currentMillis - lastSineUpdate >= sineInterval) {
      lastSineUpdate = currentMillis;
      value = 512 + 511 * sin(angle);
      angle += 0.1;
      if (angle >= 2 * PI) angle = 0;
    }

    // Mapa 0–1023 → 0–63 (invertovaná Y os)
    int y = map(value, 0, 1023, SCREEN_HEIGHT - 1, 0);

    // Vykresli spojnicu medzi posledným a aktuálnym bodom
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

  if (cmd == "obdlznik") {
    generateSine = false;
    generateSquare = true;
    x = 0;
    prevY = SCREEN_HEIGHT / 2;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("OBDLZNIK");
    display.display();
    Serial.println(">> Generujem OBDLZNIK");
  } else if (cmd == "sinus") {
    generateSquare = false;
    generateSine = true;
    x = 0;
    prevY = SCREEN_HEIGHT / 2;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("SINUS");
    display.display();
    Serial.println(">> Generujem SINUS");
  } else if (cmd == "stop") {
    generateSine = false;
    generateSquare = false;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("STOP");
    display.display();
    Serial.println(">> Vykreslovanie ZASTAVENE");
  } else {
    Serial.println(">> Neznamy prikaz.");
  }
}
