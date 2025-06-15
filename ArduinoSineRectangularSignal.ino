String inputString = "";
bool generateSine = false;
bool generateSquare = false;


// Open Serial Plotter inside Arduino IDE on top right side (signal logo)


unsigned long lastToggle = 0;
unsigned long squareToggleInterval = 500; // prepínanie medzi HIGH a LOW
bool squareState = false;

const int outputInterval = 20;  // ako často sa opakuje výstup do Plottra
unsigned long lastOutputTime = 0;

float angle = 0;
const int sineInterval = 20;
unsigned long lastSineUpdate = 0;

void setup() {
  Serial.begin(9600);
  inputString.reserve(20);
  Serial.println("Napíš 'obdlznik' alebo 'sinus'.");
}

void loop() {
  // Čítanie serial vstupu
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

  // Prepínanie stavu obdĺžnika (každých X ms)
  if (generateSquare && currentMillis - lastToggle >= squareToggleInterval) {
    lastToggle = currentMillis;
    squareState = !squareState;
  }

  // Výstup obdĺžnikového signálu každých outputInterval ms
  if (generateSquare && currentMillis - lastOutputTime >= outputInterval) {
    lastOutputTime = currentMillis;
    int value = squareState ? 1023 : 0;
    Serial.println(value);
  }

  // Generovanie sínusu
  if (generateSine && currentMillis - lastSineUpdate >= sineInterval) {
    lastSineUpdate = currentMillis;
    float sineValue = 512 + 511 * sin(angle);
    Serial.println((int)sineValue);
    angle += 0.1;
    if (angle >= 2 * PI) angle = 0;
  }
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd == "obdlznik") {
    generateSine = false;
    generateSquare = true;
    Serial.println(">> Generujem OBDLZNIKOVY SIGNAL");
  } else if (cmd == "sinus") {
    generateSquare = false;
    generateSine = true;
    Serial.println(">> Generujem SINUSOVY SIGNAL");
  } else {
    generateSine = false;
    generateSquare = false;
    Serial.println(">> Neznamy prikaz. Skus 'obdlznik' alebo 'sinus'.");
  }
}
