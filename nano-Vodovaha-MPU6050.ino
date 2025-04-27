#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <MPU6050.h>

// Definície OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Inicializácia MPU6050
MPU6050 mpu;

// Premenné pre kalibráciu
float offsetAngleX = 0;
float offsetAngleY = 0;

// Stred displeja
int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;

// Funkcia na kalibráciu MPU6050
void calibrateMPU() {
  const int samples = 500;
  float sumX = 0;
  float sumY = 0;

  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    float axg = ax / 16384.0;
    float ayg = ay / 16384.0;
    float azg = az / 16384.0;

    float angleX = atan2(ayg, sqrt(axg * axg + azg * azg)) * 180.0 / PI;
    float angleY = atan2(-axg, sqrt(ayg * ayg + azg * azg)) * 180.0 / PI;

    sumX += angleX;
    sumY += angleY;
    delay(5);
  }

  offsetAngleX = sumX / samples;
  offsetAngleY = sumY / samples;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Chyba: MPU6050 sa nepripojil.");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Chyba: OLED displej sa nenačítal."));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);              // HORE displeja
  display.println("KALIBRUJEM...");

  display.setTextSize(2);                // Väčšie písmo pre hlášku
  display.setCursor(10, 20);         // V strede displeja
  display.println("POLOZ MA");
  display.setCursor(20, centerY + 5);    // Pod to
  display.println("ROVNO !");
  display.display();

  calibrateMPU();
  delay(500);
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;

  float angleX = atan2(ayg, sqrt(axg * axg + azg * azg)) * 180.0 / PI;
  float angleY = atan2(-axg, sqrt(ayg * ayg + azg * azg)) * 180.0 / PI;

  float correctedAngleX = angleX - offsetAngleX;
  float correctedAngleY = angleY - offsetAngleY;

  // 🔄 OTOČENIE O 180°:
  int offsetX = constrain((int)(-correctedAngleX * 2), -45, 45);
  int offsetY = constrain((int)(correctedAngleY * 2), -20, 20);

  display.clearDisplay();

  // Rámček "rúry"
  display.drawRect(centerX - 50, centerY - 5, 100, 10, SSD1306_WHITE); // horizontálny
  display.drawRect(centerX - 5, centerY - 30, 10, 60, SSD1306_WHITE);  // vertikálny

  // Gulička (bublinka)
  display.fillCircle(centerX + offsetX, centerY + offsetY, 4, SSD1306_WHITE);

  // Debug: zobrazenie uhlov
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("X: ");
  display.print(correctedAngleX, 1);
  display.print((char)247);
  display.println(" ");

  display.setCursor(0, 10);
  display.print("Y: ");
  display.print(correctedAngleY, 1);
  display.print((char)247);
  display.println(" ");

  display.display();

  delay(20);
}
