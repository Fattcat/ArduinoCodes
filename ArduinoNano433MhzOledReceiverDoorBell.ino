#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RCSwitch.h>

#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  

#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RCSwitch mySwitch = RCSwitch();
long lastSignal = 0; 
int lastBitLength = 0;
int lastPulseLength = 0;

void setup() {
    Serial.begin(9600);

    // Inicializácia OLED displeja
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("OLED inicializácia zlyhala"));
        for (;;);
    }
    display.clearDisplay();
    display.setTextSize(2);     
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Start...");
    display.display();
    delay(2000); 

    mySwitch.enableReceive(0); // RF prijímač na digitálnom pine 2
    display.clearDisplay();
    display.display();
}

void loop() {
    if (mySwitch.available()) {
        long receivedSignal = mySwitch.getReceivedValue();
        int receivedBitLength = mySwitch.getReceivedBitlength();
        int receivedPulseLength = mySwitch.getReceivedDelay();

        // Ak je signál nový, aktualizuj displej
        if (receivedSignal != lastSignal || receivedBitLength != lastBitLength || receivedPulseLength != lastPulseLength) {
            lastSignal = receivedSignal;
            lastBitLength = receivedBitLength;
            lastPulseLength = receivedPulseLength;

            // Vymaže displej a zobrazí nový signál
            display.clearDisplay();

            // Zobrazenie signálu veľkým textom
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.println("Signal:");

            display.setTextSize(2);
            display.setCursor(0, 10);
            display.println(lastSignal);

            // Zobrazenie bitovej dlzky signalu
            display.setTextSize(1);
            display.setCursor(0, 40);
            display.print("Bitov: ");
            display.println(lastBitLength);

            // Zobrazenie pulznej sirky signalu
            display.setCursor(0, 50);
            display.print("Pulzna sirka: ");
            display.print(lastPulseLength);
            display.println(" us");

            display.display();
        }

        mySwitch.resetAvailable();
    }
}
