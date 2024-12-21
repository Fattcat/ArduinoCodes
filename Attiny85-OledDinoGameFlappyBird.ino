#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definícia pinov pre tlačidlá
#define BUTTON_UP    5    // Pin pre tlačidlo "Up"
#define BUTTON_DOWN  6    // Pin pre tlačidlo "Down"
#define BUTTON_ENTER 7    // Pin pre tlačidlo "Enter"

// Definícia tlačidiel pre hru Flappy Bird
#define BTN_OK    5
#define BTN_JUMP  6

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

// Menu pre výber hry
int cursorPosition = 0; // Kurzorská pozícia (0 = FlappyBird, 1 = DinoGame)
String menuItems[] = {"FlappyBird", "DinoGame"};
int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);

int gameState = 0;  // 0: Main Menu, 1: Dino Game, 2: Flappy Bird

// Dino game premenné
int dinoY = SCREEN_HEIGHT - 10; // Pozícia Dina (skákajúca postavička)
int dinoVelocity = 0;           // Rýchlosť Dina
int dinoJumpForce = -4;         // Sila skoku Dina
int dinoGravity = 1;            // Gravitačná sila Dina
bool dinoJumping = false;       // Indikátor, či je Dino vo vzduchu

struct DinoObstacle {
  int x;
  int height;
};
DinoObstacle dinoObstacle = {SCREEN_WIDTH, SCREEN_HEIGHT - 10}; // Prekážka pre Dino hru

void setup() {
  Serial.begin(9600);

  // Inicializácia OLED displeja
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Nastavenie pinov tlačidiel
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_JUMP, INPUT_PULLUP);

  drawMenu();
}

void loop() {
  // Získať stav tlačidiel
  if (digitalRead(BUTTON_UP) == LOW) {
    cursorPosition--;
    if (cursorPosition < 0) cursorPosition = totalItems - 1;  // Cyklovanie na poslednú položku
    drawMenu();
    delay(200); // Debouncing
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    cursorPosition++;
    if (cursorPosition >= totalItems) cursorPosition = 0;  // Cyklovanie na prvú položku
    drawMenu();
    delay(200); // Debouncing
  }

  if (digitalRead(BUTTON_ENTER) == LOW) {
    // Spustenie hry podľa vybranej položky
    if (cursorPosition == 0) {
      gameState = 2;  // Flappy Bird
      startFlappyBird();
    } else if (cursorPosition == 1) {
      gameState = 1;  // Dino Game
      startDinoGame();
    }
    delay(200); // Debouncing
  }

  if (gameState == 2 && inGame) {
    playFlappyBird(); // Spustenie hry Flappy Bird
  } else if (gameState == 1 && inGame) {
    playDinoGame(); // Spustenie hry Dino
  }
}

void drawMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);

  // Vykreslenie menu
  for (int i = 0; i < totalItems; i++) {
    if (i == cursorPosition) {
      display.print("-> "); // Kurzorka pred položkou
    } else {
      display.print("   ");
    }
    display.println(menuItems[i]);
  }

  display.display();
}

void startFlappyBird() {
  // Inicializácia hry Flappy Bird
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Flappy Bird");
  display.display();
  delay(2000);  // Simulujeme spustenie hry (zastavenie pred návratom do menu)
  
  inGame = true; // Začiatok hry
  resetGame();   // Resetovanie hry
}

void startDinoGame() {
  // Inicializácia hry Dino
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Dino Game");
  display.display();
  delay(2000);  // Simulujeme spustenie hry (zastavenie pred návratom do menu)
  
  inGame = true; // Začiatok hry
  resetDinoGame();   // Resetovanie hry
}

void playDinoGame() {
  display.clearDisplay();

  // Čítanie tlačidla pre skok
  if (digitalRead(BTN_JUMP) == LOW && !dinoJumping) {
    dinoVelocity = dinoJumpForce; // Posun Dina nahor
    dinoJumping = true;
  }

  // Aplikácia gravitácie
  dinoVelocity += dinoGravity;

  // Aktualizácia pozície Dina
  dinoY += dinoVelocity;

  // Kontrola, či Dino nedopadol na zem
  if (dinoY > SCREEN_HEIGHT - 10) {
    dinoY = SCREEN_HEIGHT - 10;
    dinoVelocity = 0;
    dinoJumping = false;
  }

  // Pohyb prekážky
  dinoObstacle.x -= 2;  // Rýchlosť pohybu prekážky
  if (dinoObstacle.x < -10) {
    dinoObstacle.x = SCREEN_WIDTH;
    dinoObstacle.height = random(10, SCREEN_HEIGHT - 10);
    score++; // Zvýšenie skóre
  }

  // Kreslenie prekážky
  display.fillRect(dinoObstacle.x, dinoObstacle.height, 10, SCREEN_HEIGHT - dinoObstacle.height, SSD1306_WHITE);

  // Kontrola kolízie medzi Dinom a prekážkou
  if (dinoObstacle.x < 20 && dinoObstacle.x + 10 > 10) {
    if (dinoY > dinoObstacle.height) {
      inGame = false; // Konec hry
      gameOverScreen();
      return;
    }
  }

  // Kreslenie Dina
  display.fillRect(10, dinoY, 10, 10, SSD1306_WHITE); // Dino je zobrazený ako malý štvorček

  // Zobrazenie skóre
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Score: "));
  display.print(score);

  display.display();
}

void gameOverScreen() {
  display.clearDisplay();
  display.setCursor(30, 20);
  display.println("Game Over");
  display.setCursor(30, 30);
  display.print("Score: ");
  display.print(score);
  display.setCursor(30, 40);
  display.println("Press Enter to Restart");
  display.display();

  while (digitalRead(BUTTON_ENTER) == HIGH) {
    delay(10); // Čakanie na stlačenie tlačidla Enter pre reštart
  }

  resetDinoGame(); // Resetovanie hry
  inGame = true;
}

void resetDinoGame() {
  dinoY = SCREEN_HEIGHT
  10; dinoVelocity = 0;
  dinoObstacle.x = SCREEN_WIDTH;     dinoObstacle.height = random(10,SCREEN_HEIGHT - 10); score = 0;   dinoJumping = false;
}