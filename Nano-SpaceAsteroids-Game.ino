#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Tlačidlá ---
#define BUTTON_ENTER 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4

// --- Herné nastavenia ---
#define MAX_ROCKETS 5
#define MAX_ASTEROIDS 3

// --- Loď ---
int shipX = SCREEN_WIDTH / 2;
const int shipY = SCREEN_HEIGHT - 8;

// --- Rakety ---
bool rocketActive[MAX_ROCKETS];
int rocketX[MAX_ROCKETS];
int rocketY[MAX_ROCKETS];
unsigned long lastShotTime = 0;
const unsigned long shotCooldown = 200; // v ms

// --- Asteroidy ---
int asteroidX[MAX_ASTEROIDS];
int asteroidY[MAX_ASTEROIDS];
bool asteroidActive[MAX_ASTEROIDS];
int asteroidTick = 0;
const int asteroidSpeedDelay = 3;

int score = 0;
bool gameRunning = true;

// --- Inicializácia ---
void setup() {
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  resetAsteroids();

  display.setCursor(25, 25);
  display.print("SPACE SHOOTER");
  display.display();
  delay(2000);
}

void loop() {
  if (!gameRunning) {
    showGameOver();
    return;
  }

  handleInput();
  updateGame();
  drawGame();
  delay(30);
}

// --- Ovládanie ---
void handleInput() {
  if (digitalRead(BUTTON_LEFT) == LOW && shipX > 0) {
    shipX -= 2;
  }

  if (digitalRead(BUTTON_RIGHT) == LOW && shipX < SCREEN_WIDTH - 8) {
    shipX += 2;
  }

  // Viac rakiet s cooldownom
  if (digitalRead(BUTTON_ENTER) == LOW && millis() - lastShotTime > shotCooldown) {
    for (int i = 0; i < MAX_ROCKETS; i++) {
      if (!rocketActive[i]) {
        rocketActive[i] = true;
        rocketX[i] = shipX + 3;
        rocketY[i] = shipY - 2;
        lastShotTime = millis();
        break; // vystrelíme len jednu naraz
      }
    }
  }
}

// --- Herná logika ---
void updateGame() {
  // Aktualizácia rakiet
  for (int i = 0; i < MAX_ROCKETS; i++) {
    if (rocketActive[i]) {
      rocketY[i] -= 4;
      if (rocketY[i] < 0) {
        rocketActive[i] = false;
      }
    }
  }

  // Spomalenie asteroidov pomocou tickera
  asteroidTick++;
  if (asteroidTick >= asteroidSpeedDelay) {
    asteroidTick = 0;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
      if (asteroidActive[i]) {
        asteroidY[i] += 1;

        if (asteroidY[i] > SCREEN_HEIGHT) {
          resetAsteroid(i);
        }

        // Kolízia s loďou
        if (asteroidY[i] >= shipY - 4 && abs(asteroidX[i] - shipX) < 8) {
          gameRunning = false;
        }

        // Kolízia s raketami
        for (int j = 0; j < MAX_ROCKETS; j++) {
          if (rocketActive[j] &&
              abs(rocketX[j] - asteroidX[i]) < 6 &&
              abs(rocketY[j] - asteroidY[i]) < 6) {
            rocketActive[j] = false;
            resetAsteroid(i);
            score++;
          }
        }
      }
    }
  }
}

// --- Reset jedného asteroidu ---
void resetAsteroid(int i) {
  asteroidX[i] = random(0, SCREEN_WIDTH - 6);
  asteroidY[i] = 0;
  asteroidActive[i] = true;
}

// --- Reset všetkých asteroidov ---
void resetAsteroids() {
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    resetAsteroid(i);
  }

  // Reset rakiet
  for (int i = 0; i < MAX_ROCKETS; i++) {
    rocketActive[i] = false;
  }
}

// --- Zobrazovanie ---
void drawGame() {
  display.clearDisplay();

  // Loď
  display.fillRect(shipX, shipY, 8, 4, SSD1306_WHITE);

  // Rakety
  for (int i = 0; i < MAX_ROCKETS; i++) {
    if (rocketActive[i]) {
      display.drawLine(rocketX[i], rocketY[i], rocketX[i], rocketY[i] - 3, SSD1306_WHITE);
    }
  }

  // Asteroidy
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroidActive[i]) {
      display.fillCircle(asteroidX[i], asteroidY[i], 3, SSD1306_WHITE);
    }
  }

  // Skóre
  display.setCursor(0, 0);
  display.print("Skore: ");
  display.print(score);

  display.display();
}

// --- Game Over ---
void showGameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print("Koniec hry");
  display.setTextSize(1);
  display.setCursor(20, 45);
  display.print("Skore: ");
  display.print(score);
  display.display();
  delay(3000);

  // Reset hry
  score = 0;
  shipX = SCREEN_WIDTH / 2;
  gameRunning = true;
  resetAsteroids();
}
