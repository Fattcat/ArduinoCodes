#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------------------------------------------
// ----- Connection ----- \\
// Arduino Uno - Oled Display 0.96 Inch
// 5V - VCC
// GND - GND
// SDA - A4
// SCL - A5

// Buttons
// OK - D2
// UP - D4

// - HAVE FUN :D
// ------------------------------------------

// Definícia OLED displeja (128x64) s SWITCHCAPVCC
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Piny pre tlačidlá
#define BTN_OK 2
#define BTN_JUMP 4

// Premenné pre Flappy Bird
float birdY = SCREEN_HEIGHT / 2.0; // Pozícia vtáka
float birdVelocity = 0;           // Rýchlosť vtáka
float gravity = 0.4;              // Gravitačná sila
float jumpForce = -2.5;           // Sila skoku
int maxFallSpeed = 3;             // Maximálna rýchlosť pádu
#define BIRD_RADIUS 4             // Polomer vtáčika

struct Obstacle {
  int x;
  int gapY;
  int gapHeight;
};
Obstacle flappyObstacle = {SCREEN_WIDTH, 30, 20};

bool inGame = false; // Indikátor, či sme v hre (začína na false)
int score = 0;       // Skóre hráča

void setup() {
  Serial.begin(9600);

  // Inicializácia OLED displeja
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Inicializácia tlačidiel
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_JUMP, INPUT_PULLUP);

  // Vyčistenie displeja
  display.clearDisplay();
  display.display();

  // Zobrazenie úvodnej obrazovky
  showIntroScreen();
}

void loop() {
  if (inGame) {
    playFlappyBird(); // Spustenie hry Flappy Bird
  } else {
    waitForGameStart(); // Čakanie na začiatok hry
  }
}

// Funkcia pre úvodnú obrazovku
void showIntroScreen() {
  display.clearDisplay();
  display.setTextSize(2); // Väčší text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1, 10); // Nastavenie pozície textu
  display.println(F("Flappy"));
  display.setCursor(77, 10);
  display.println(F("Bird"));

  display.setTextSize(1); // Menší text
  display.setCursor(10, 40); // Nastavenie pozície textu
  display.println(F("Press OK to start"));
  display.display();
}

// Funkcia pre čakanie na stlačenie tlačidla OK
void waitForGameStart() {
  while (digitalRead(BTN_OK) == HIGH) {
    // Čakanie na stlačenie tlačidla
    delay(10);
  }

  // Po stlačení tlačidla sa hra začne
  resetGame();
  inGame = true;
}

// Funkcia pre Flappy Bird
void playFlappyBird() {
  display.clearDisplay();

  // Čítanie tlačidla pre skok
  if (digitalRead(BTN_JUMP) == LOW) {
    birdVelocity = jumpForce; // Posun vtáka nahor
  }

  // Aplikácia gravitácie
  birdVelocity += gravity;

  // Obmedzenie maximálnej rýchlosti pádu
  if (birdVelocity > maxFallSpeed) {
    birdVelocity = maxFallSpeed;
  }

  // Aktualizácia pozície vtáka
  birdY += birdVelocity;

  // Kontrola, či vtáčik nevyliezol mimo obrazovky
  if (birdY < BIRD_RADIUS) {
    birdY = BIRD_RADIUS;
    birdVelocity = 0;
  } else if (birdY > SCREEN_HEIGHT - BIRD_RADIUS) {
    birdY = SCREEN_HEIGHT - BIRD_RADIUS;
    inGame = false; // Koniec hry
    gameOverScreen();
    return;
  }

  // Pohyb prekážky doľava
  flappyObstacle.x -= 3; // Rýchlosť pohybu prekážky
  if (flappyObstacle.x < -10) {
    flappyObstacle.x = SCREEN_WIDTH;
    flappyObstacle.gapY = random(10, SCREEN_HEIGHT - flappyObstacle.gapHeight - 10);
    score++; // Zvýšenie skóre pri prekonaní prekážky
  }

  // Kontrola kolízie vtáčika s prekážkami
  if (flappyObstacle.x < 10 + BIRD_RADIUS && flappyObstacle.x + 10 > 10 - BIRD_RADIUS) {
    if (birdY - BIRD_RADIUS < flappyObstacle.gapY || 
        birdY + BIRD_RADIUS > flappyObstacle.gapY + flappyObstacle.gapHeight) {
      inGame = false; // Nastavenie konca hry
      gameOverScreen();
      return;
    }
  }

  // Kreslenie hry
  display.fillCircle(10, birdY, BIRD_RADIUS, SSD1306_WHITE); // Kreslenie vtáka
  display.fillRect(flappyObstacle.x, 0, 10, flappyObstacle.gapY, SSD1306_WHITE); // Horná prekážka
  display.fillRect(flappyObstacle.x, flappyObstacle.gapY + flappyObstacle.gapHeight, 10, SCREEN_HEIGHT - (flappyObstacle.gapY + flappyObstacle.gapHeight), SSD1306_WHITE); // Dolná prekážka

  // Zobrazenie skóre počas hry
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Score: "));
  display.print(score);

  display.display();
}

// Funkcia pre obrazovku Game Over
void gameOverScreen() {
  display.clearDisplay();

  // Nakreslenie čierneho rámu
  display.fillRect(5, 2, 115, 60, SSD1306_BLACK); // Čierny rám
  display.drawRect(5, 2, 120, 60, SSD1306_WHITE); // Biely okraj rámu

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 10);
  display.println(F("Game Over"));

  display.setCursor(35, 20);
  display.print(F("Score: "));
  display.print(score);

  display.setCursor(25, 30);
  display.println(F("OK to restart"));
  display.display();

  while (digitalRead(BTN_OK) == HIGH) {
    delay(10);
  }

  resetGame();
  inGame = true;
}

// Funkcia pre resetovanie hry
void resetGame() {
  birdY = SCREEN_HEIGHT / 2.0;
  birdVelocity = 0;

  flappyObstacle.x = SCREEN_WIDTH;
  flappyObstacle.gapY = random(10, SCREEN_HEIGHT - flappyObstacle.gapHeight - 10);

  score = 0;
}
