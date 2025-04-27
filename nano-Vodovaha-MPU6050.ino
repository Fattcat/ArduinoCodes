#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU6050.h> // download "MPU6050 By Electronic Cats version 1.4.3" in Arduino IDE
// Adafruit_KPU6050 Does NOT work (I think it, so please test it)

// OLED nastavenia
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MPU6050
MPU6050 mpu;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Inicializácia OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // zastaviť
  }
  display.clearDisplay();
  
  // Inicializácia MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println(F("MPU6050 connection failed"));
    while (1);
  }
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Prevod z raw dát na "g" jednotky (akcelerácia)
  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;

  // Výpočet náklonu
  float angleX = atan2(ayg, sqrt(axg * axg + azg * azg)) * 180.0 / PI;
  float angleY = atan2(-axg, sqrt(ayg * ayg + azg * azg)) * 180.0 / PI;

  // Debug na Serial Monitor
  Serial.print("X: ");
  Serial.print(angleX);
  Serial.print(" | Y: ");
  Serial.println(angleY);

  // Výpis na OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("X Tilt: ");
  display.print(angleX, 1);
  display.print((char)247); // znak °

  display.setCursor(0, 10);
  display.print("Y Tilt: ");
  display.print(angleY, 1);
  display.print((char)247); // znak °

  // Grafický indikátor
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;
  int offsetX = constrain((int)(angleY * 2), -30, 30);
  int offsetY = constrain((int)(angleX * 2), -20, 20);

  display.drawCircle(centerX, centerY, 5, SSD1306_WHITE); // statická "bublinka"
  display.fillCircle(centerX + offsetX, centerY + offsetY, 3, SSD1306_WHITE); // pohybujúca sa bodka

  display.display();
  delay(100);
}
