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

// Pre Space Shooter použijeme tieto piny tlačidiel
#define BUTTON_LEFT BUTTON_UP    // Nahradíme pre Space Shooter
#define BUTTON_RIGHT BUTTON_DOWN

// ----- Premenné pre menu -----
int cursorPosition = 0;        // Poloha kurzora v menu
const int menuItems = 4;       // Počet položiek menu (pridaný Space Shooter)

// ----- Premenné pre DinoGame -----
int dinoY = SCREEN_HEIGHT - 10;
float dinoVelocity = 0;
const int dinoJumpForce = 10;
const float dinoGravity = 1;

struct DinoObstacle {
  int x;
  int y;
  bool active;
};

DinoObstacle dinoObstacle;

bool dinoGameActive = false;
int dinoScore = 0;

// ----- Premenné pre Flappy Bird -----
int birdY = SCREEN_HEIGHT / 2;
float birdVelocity = 0;
const int birdGravity = 1;
const int birdJump = 6;
bool flappyBirdActive = false;

struct FlappyObstacle {
  int x;
  int gapY;
  const int gapHeight = 16;
};

FlappyObstacle flappyObstacle;

// ----- Premenné pre Pong -----
const unsigned long PADDLE_RATE = 64;
const unsigned long BALL_RATE = 16;
const uint8_t PADDLE_HEIGHT = 12;
const uint8_t SCORE_LIMIT = 9;

bool game_over, win;

uint8_t player_score, mcu_score;
uint8_t ball_x = 53, ball_y = 26;
uint8_t ball_dir_x = 1, ball_dir_y = 1;

unsigned long ball_update;
unsigned long paddle_update;

const uint8_t MCU_X = 12;
uint8_t mcu_y = 16;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;

// ----- Premenné pre Space Shooter -----
#define MAX_ROCKETS 5
#define MAX_ASTEROIDS 3

int shipX = SCREEN_WIDTH / 2;
const int shipY = SCREEN_HEIGHT - 8;

bool rocketActive[MAX_ROCKETS];
int rocketX[MAX_ROCKETS];
int rocketY[MAX_ROCKETS];
unsigned long lastShotTime = 0;
const unsigned long shotCooldown = 200;

int asteroidX[MAX_ASTEROIDS];
int asteroidY[MAX_ASTEROIDS];
bool asteroidActive[MAX_ASTEROIDS];
int asteroidTick = 0;
const int asteroidSpeedDelay = 3;

int spaceScore = 0;
bool spaceGameRunning = true;

// ----- Setup -----
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

// ----- Loop -----
void loop() {
  showMenu();
}

// ----- Menu -----
void showMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(F("Select a game:"));

  display.setCursor(10, 20);
  display.print(cursorPosition == 0 ? "> DinoGame" : "  DinoGame");
  display.setCursor(10, 30);
  display.print(cursorPosition == 1 ? "> FlappyBird" : "  FlappyBird");
  display.setCursor(10, 40);
  display.print(cursorPosition == 2 ? "> PongGame" : "  PongGame");
  display.setCursor(10, 50);
  display.print(cursorPosition == 3 ? "> SpaceShooter" : "  SpaceShooter");

  display.display();

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
    } else if (cursorPosition == 2) {
      startPongGame();
    } else if (cursorPosition == 3) {
      startSpaceShooter();
    }
  }
}

// ----- Game Over Screen -----
void showGameOverScreen(int score) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Game Over");
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Score: ");
  display.print(score);
  display.display();
  delay(2000);
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

    if (digitalRead(BUTTON_ENTER) == LOW && dinoY == SCREEN_HEIGHT - 10) {
      dinoVelocity = -dinoJumpForce;
    }

    dinoVelocity += dinoGravity;
    dinoY += dinoVelocity;

    if (dinoY > SCREEN_HEIGHT - 10) {
      dinoY = SCREEN_HEIGHT - 10;
      dinoVelocity = 0;
    }

    dinoObstacle.x -= 4;
    if (dinoObstacle.x < -10) {
      dinoObstacle.x = SCREEN_WIDTH;
      dinoScore++;
    }

    if (dinoObstacle.x < 18 && dinoObstacle.x > 8 && dinoY > SCREEN_HEIGHT - 20) {
      dinoGameActive = false;
      showGameOverScreen(dinoScore);
      break;
    }

    display.fillRect(5, dinoY, 10, 10, SSD1306_WHITE);
    display.fillRect(dinoObstacle.x, SCREEN_HEIGHT - 10, 10, 10, SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print(F("Score: "));
    display.print(dinoScore);

    display.display();
    delay(30);
  }
}

// ----- Flappy Bird -----
void startFlappyBird() {
  flappyBirdActive = true;
  int birdScore = 0;

  birdY = SCREEN_HEIGHT / 2;
  birdVelocity = 0;
  flappyObstacle.x = SCREEN_WIDTH;
  flappyObstacle.gapY = random(10, SCREEN_HEIGHT - flappyObstacle.gapHeight - 10);

  while (flappyBirdActive) {
    display.clearDisplay();

    if (digitalRead(BUTTON_ENTER) == LOW) {
      birdVelocity = -birdJump;
    }

    birdVelocity += birdGravity;
    birdY += birdVelocity;

    if (birdY < 0) birdY = 0;
    if (birdY > SCREEN_HEIGHT) {
      flappyBirdActive = false;
      showGameOverScreen(birdScore);
      break;
    }

    flappyObstacle.x -= 3;
    if (flappyObstacle.x < -10) {
      flappyObstacle.x = SCREEN_WIDTH;
      flappyObstacle.gapY = random(10, SCREEN_HEIGHT - flappyObstacle.gapHeight - 10);
      birdScore++;
    }

    if (flappyObstacle.x < 14 && flappyObstacle.x > 6) {
      if (birdY < flappyObstacle.gapY || birdY > flappyObstacle.gapY + flappyObstacle.gapHeight) {
        flappyBirdActive = false;
        showGameOverScreen(birdScore);
        break;
      }
    }

    display.fillCircle(10, birdY, 3, SSD1306_WHITE);
    display.fillRect(flappyObstacle.x, 0, 10, flappyObstacle.gapY, SSD1306_WHITE);
    display.fillRect(flappyObstacle.x, flappyObstacle.gapY + flappyObstacle.gapHeight,
                     10, SCREEN_HEIGHT - flappyObstacle.gapY - flappyObstacle.gapHeight, SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print(F("Score: "));
    display.print(birdScore);

    display.display();
    delay(30);
  }
}

// ----- Pong -----
void startPongGame() {
  game_over = false;
  win = false;
  player_score = 0;
  mcu_score = 0;
  ball_x = SCREEN_WIDTH / 2;
  ball_y = SCREEN_HEIGHT / 2;
  ball_dir_x = 1;
  ball_dir_y = 1;
  player_y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
  mcu_y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;

  unsigned long ball_update = millis();
  unsigned long paddle_update = millis();

  while (!game_over) {
    unsigned long time = millis();

    // Update ball position
    if (time - ball_update > BALL_RATE) {
      ball_x += ball_dir_x;
      ball_y += ball_dir_y;

      // Odrazenie od horného a dolného okraja
      if (ball_y <= 0 || ball_y >= SCREEN_HEIGHT - 2) {
        ball_dir_y = -ball_dir_y;
      }

      // Odrazenie od pádla hráča
      if (ball_x >= PLAYER_X - 2 && ball_x <= PLAYER_X &&
          ball_y + 2 >= player_y && ball_y <= player_y + PADDLE_HEIGHT) {
        ball_dir_x = -ball_dir_x;
        ball_x = PLAYER_X - 2; // trochu posun, aby sa lopta nepretlačila cez pádlo
      }

      // Odrazenie od pádla CPU
      if (ball_x <= MCU_X + 2 && ball_x >= MCU_X &&
          ball_y + 2 >= mcu_y && ball_y <= mcu_y + PADDLE_HEIGHT) {
        ball_dir_x = -ball_dir_x;
        ball_x = MCU_X + 2;
      }

      // Skóre, keď lopta prejde za pádlo
      if (ball_x < 0) {
        player_score++;
        ball_x = SCREEN_WIDTH / 2;
        ball_y = SCREEN_HEIGHT / 2;
        ball_dir_x = 1;
        ball_dir_y = (random(0, 2) == 0) ? 1 : -1;
      } else if (ball_x > SCREEN_WIDTH) {
        mcu_score++;
        ball_x = SCREEN_WIDTH / 2;
        ball_y = SCREEN_HEIGHT / 2;
        ball_dir_x = -1;
        ball_dir_y = (random(0, 2) == 0) ? 1 : -1;
      }

      ball_update = time;
    }

    // Pohyb hráčovho pádla
    if (time - paddle_update > PADDLE_RATE) {
      if (digitalRead(BUTTON_UP) == LOW && player_y > 0) {
        player_y -= 2;
      }
      if (digitalRead(BUTTON_DOWN) == LOW && player_y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        player_y += 2;
      }

      // Jednoduché AI pre CPU pádlo - nasleduje loptičku
      if (ball_y > mcu_y + PADDLE_HEIGHT / 2 && mcu_y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        mcu_y += 1; // rýchlosť CPU pádla
      } else if (ball_y < mcu_y + PADDLE_HEIGHT / 2 && mcu_y > 0) {
        mcu_y -= 1;
      }

      paddle_update = time;
    }

    // Kreslenie obrazovky
    display.clearDisplay();

    // Lopta
    display.fillRect(ball_x, ball_y, 2, 2, SSD1306_WHITE);

    // Pádla
    display.fillRect(PLAYER_X, player_y, 2, PADDLE_HEIGHT, SSD1306_WHITE);
    display.fillRect(MCU_X, mcu_y, 2, PADDLE_HEIGHT, SSD1306_WHITE);

    // Skóre
    display.setCursor(30, 0);
    display.setTextSize(1);
    display.print(player_score);

    display.setCursor(80, 0);
    display.print(mcu_score);

    display.display();

    // Ukončenie hry
    if (player_score >= SCORE_LIMIT || mcu_score >= SCORE_LIMIT) {
      game_over = true;
      win = (player_score >= SCORE_LIMIT);
    }

    delay(10);
  }

  // Zobrazenie výsledku
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 20);
  if (win) {
    display.print("You Win!");
  } else {
    display.print("You Lose!");
  }
  display.display();
  delay(3000);
}

// ----- Space Shooter Helper Functions -----
void resetAsteroid(int i) {
  asteroidX[i] = random(0, SCREEN_WIDTH - 5);
  asteroidY[i] = 0;
  asteroidActive[i] = true;
}

void resetAsteroids() {
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    resetAsteroid(i);
  }
}

void handleSpaceInput() {
  // Move ship left/right
  if (digitalRead(BUTTON_LEFT) == LOW && shipX > 0) {
    shipX -= 3;
  }
  if (digitalRead(BUTTON_RIGHT) == LOW && shipX < SCREEN_WIDTH - 10) {
    shipX += 3;
  }
  // Fire rocket
  if (digitalRead(BUTTON_ENTER) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastShotTime > shotCooldown) {
      for (int i = 0; i < MAX_ROCKETS; i++) {
        if (!rocketActive[i]) {
          rocketActive[i] = true;
          rocketX[i] = shipX + 4;
          rocketY[i] = shipY - 5;
          lastShotTime = currentTime;
          break;
        }
      }
    }
  }
}

void updateSpaceGame() {
  // Update rockets
  for (int i = 0; i < MAX_ROCKETS; i++) {
    if (rocketActive[i]) {
      rocketY[i] -= 5;
      if (rocketY[i] < 0) {
        rocketActive[i] = false;
      }
    }
  }

  // Update asteroids
  asteroidTick++;
  if (asteroidTick > asteroidSpeedDelay) {
    asteroidTick = 0;
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
      if (asteroidActive[i]) {
        asteroidY[i] += 2;
        if (asteroidY[i] > SCREEN_HEIGHT) {
          resetAsteroid(i);
          spaceScore++;
        }
      }
    }
  }

  // Collision detection (rocket hits asteroid)
  for (int i = 0; i < MAX_ROCKETS; i++) {
    if (rocketActive[i]) {
      for (int j = 0; j < MAX_ASTEROIDS; j++) {
        if (asteroidActive[j]) {
          if (rocketX[i] >= asteroidX[j] && rocketX[i] <= asteroidX[j] + 5 &&
              rocketY[i] >= asteroidY[j] && rocketY[i] <= asteroidY[j] + 5) {
            rocketActive[i] = false;
            resetAsteroid(j);
            spaceScore += 5;
          }
        }
      }
    }
  }

  // Collision detection (ship hits asteroid)
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroidActive[i]) {
      if (shipX < asteroidX[i] + 5 && shipX + 10 > asteroidX[i] &&
          shipY < asteroidY[i] + 5 && shipY + 5 > asteroidY[i]) {
        spaceGameRunning = false;
      }
    }
  }
}

void drawSpaceGame() {
  display.clearDisplay();

  // Draw ship
  display.fillRect(shipX, shipY, 10, 5, SSD1306_WHITE);

  // Draw rockets
  for (int i = 0; i < MAX_ROCKETS; i++) {
    if (rocketActive[i]) {
      display.drawFastVLine(rocketX[i], rocketY[i], 3, SSD1306_WHITE);
    }
  }

  // Draw asteroids
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroidActive[i]) {
      display.fillCircle(asteroidX[i] + 2, asteroidY[i] + 2, 3, SSD1306_WHITE);
    }
  }

  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Score: "));
  display.print(spaceScore);

  display.display();
}

void showSpaceGameOver() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 20);
  display.print(F("Game Over"));
  display.setTextSize(1);
  display.setCursor(20, 45);
  display.print(F("Score: "));
  display.print(spaceScore);
  display.display();
  delay(2000);
}

// ----- Space Shooter main function -----
void startSpaceShooter() {
  shipX = SCREEN_WIDTH / 2;
  spaceScore = 0;
  spaceGameRunning = true;
  resetAsteroids();

  while (spaceGameRunning) {
    handleSpaceInput();
    updateSpaceGame();
    drawSpaceGame();
    delay(30);
  }

  showSpaceGameOver();
}
