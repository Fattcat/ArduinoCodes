#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// ----- Definície OLED a pinov -----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definície pinov tlačidiel
#define BUTTON_ENTER 2
#define BUTTON_UP 3
#define BUTTON_DOWN 4

// Premenné pre menu
int cursorPosition = 0; // Poloha kurzora v menu
const int menuItems = 2; // Počet položiek menu

// ----- Premenné pre DinoGame -----
int dinoY = SCREEN_HEIGHT - 10; // Vertikálna pozícia hráča
float dinoVelocity = 0;         // Vertikálna rýchlosť hráča
const int dinoJumpForce = 5;    // Sila skoku
const float dinoGravity = 0.25; // Gravitačná sila

struct DinoObstacle {
  int x;
  int y;
};
DinoObstacle dinoObstacle = {SCREEN_WIDTH, SCREEN_HEIGHT - 10};
bool dinoGameActive = false;
int dinoScore = 0;

// ----- Premenné pre FlappyBird -----
float birdY = SCREEN_HEIGHT / 2.0; // Vertikálna pozícia vtáka
float birdVelocity = 0;            // Rýchlosť vtáka
float birdGravity = 0.4;           // Gravitačná sila
float birdJumpForce = -2.5;        // Sila skoku
int birdScore = 0;
bool flappyGameActive = false;

struct FlappyObstacle {
  int x;
  int gapY;
  int gapHeight;
};
FlappyObstacle flappyObstacle = {SCREEN_WIDTH, 30, 20};

// ----- Funkcie -----
void setup() {
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  showMenu();
}

void showMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Zobrazenie menu
  display.setCursor(0, 0);
  display.println(F("Select a game:"));

  display.setCursor(10, 20);
  display.print(cursorPosition == 0 ? "> DinoGame" : "  DinoGame");
  display.setCursor(10, 30);
  display.print(cursorPosition == 1 ? "> FlappyBird" : "  FlappyBird");

  display.display();

  // Ovládanie kurzora
  if (digitalRead(BUTTON_UP) == LOW) {
    cursorPosition = (cursorPosition - 1 + menuItems) % menuItems;
    delay(200);
  } else if (digitalRead(BUTTON_DOWN) == LOW) {
    cursorPosition = (cursorPosition + 1) % menuItems;
    delay(200);
  } else if (digitalRead(BUTTON_ENTER) == LOW) {
    delay(200);
    if (cursorPosition == 0) {
      startDinoGame();
    } else if (cursorPosition == 1) {
      startFlappyBird();
    }
  }
}

// ----- DinoGame -----
void startDinoGame() {
  dinoGameActive = true;
  dinoScore = 0;
  dinoY = SCREEN_HEIGHT - 10;
  dinoVelocity = 0;
  dinoObstacle.x = SCREEN_WIDTH;

  while (dinoGameActive) {
    display.clearDisplay();

    // Tlačidlo pre skok
    if (digitalRead(BUTTON_ENTER) == LOW && dinoY == SCREEN_HEIGHT - 10) {
      dinoVelocity = -dinoJumpForce;
    }

    // Aplikácia gravitácie a aktualizácia pozície
    dinoVelocity += dinoGravity;
    dinoY += dinoVelocity;

    // Obmedzenie na zem
    if (dinoY > SCREEN_HEIGHT - 10) {
      dinoY = SCREEN_HEIGHT - 10;
      dinoVelocity = 0;
    }

    // Pohyb prekážky
    dinoObstacle.x -= 3;
    if (dinoObstacle.x < -10) {
      dinoObstacle.x = SCREEN_WIDTH;
      dinoScore++;
    }

    // Kontrola kolízie
    if (dinoObstacle.x < 18 && dinoObstacle.x > 6 && dinoY > SCREEN_HEIGHT - 20) {
      dinoGameActive = false;
      showGameOverScreen(dinoScore);
      break;
    }

    // Kreslenie hry
    display.fillRect(5, dinoY, 10, 10, SSD1306_WHITE); // Dino
    display.fillRect(dinoObstacle.x, SCREEN_HEIGHT - 10, 10, 10, SSD1306_WHITE); // Prekážka

    // Skóre
    display.setCursor(0, 0);
    display.print(F("Score: "));
    display.print(dinoScore);

    display.display();
    delay(30);
  }
}

// ----- FlappyBird -----
void startFlappyBird() {
  flappyGameActive = true;
  birdScore = 0;
  birdY = SCREEN_HEIGHT / 2.0;
  birdVelocity = 0;
  flappyObstacle.x = SCREEN_WIDTH;

  while (flappyGameActive) {
    display.clearDisplay();

    // Skok
    if (digitalRead(BUTTON_ENTER) == LOW) {
      birdVelocity = birdJumpForce;
    }

    // Aplikácia gravitácie
    birdVelocity += birdGravity;
    birdY += birdVelocity;

    // Obmedzenie obrazovky
    if (birdY < 0) birdY = 0;
    if (birdY > SCREEN_HEIGHT) {
      flappyGameActive = false;
      showGameOverScreen(birdScore);
      break;
    }

    // Pohyb prekážky
    flappyObstacle.x -= 3;
    if (flappyObstacle.x < -10) {
      flappyObstacle.x = SCREEN_WIDTH;
      flappyObstacle.gapY = random(10, SCREEN_HEIGHT - flappyObstacle.gapHeight - 10);
      birdScore++;
    }

    // Kontrola kolízie
    if (flappyObstacle.x < 14 && flappyObstacle.x > 6) {
      if (birdY < flappyObstacle.gapY || birdY > flappyObstacle.gapY + flappyObstacle.gapHeight) {
        flappyGameActive = false;
        showGameOverScreen(birdScore);
        break;
      }
    }

    // Kreslenie hry
    display.fillCircle(10, birdY, 3, SSD1306_WHITE); // Bird
    display.fillRect(flappyObstacle.x, 0, 10, flappyObstacle.gapY, SSD1306_WHITE); // Horná prekážka
    display.fillRect(flappyObstacle.x, flappyObstacle.gapY + flappyObstacle.gapHeight, 10, SCREEN_HEIGHT - flappyObstacle.gapY - flappyObstacle.gapHeight, SSD1306_WHITE); // Dolná prekážka

    // Skóre
    display.setCursor(0, 0);
    display.print(F("Score: "));
    display.print(birdScore);

    display.display();
    delay(30);
  }
}

// ----- Obrazovka Game Over -----
void showGameOverScreen(int score) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(30, 20);
  display.println(F("Game Over!"));
  display.setCursor(20, 40);
  display.print(F("Score: "));
  display.print(score);
  display.display();
  delay(2000);
}
