#include <TinyWireM.h>
#include <Tiny4kOLED.h>

#define UP_BUTTON 3
#define DOWN_BUTTON 4
#define ENTER_BUTTON 1

int hours = 13; // Prednastavený čas (hodiny)
int minutes = 21; // Prednastavený čas (minúty)
int seconds = 0; // Prednastavený čas (sekundy)

bool settingMode = false; // Prepnúť medzi režimom nastavovania a normálnym režimom
int settingOption = 0; // 0 = hodiny, 1 = minúty

unsigned long previousMillis = 0;
const long interval = 1000; // 1 sekunda

void setup() {
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(ENTER_BUTTON, INPUT_PULLUP);

  oled.begin();
  oled.setFont(FONT8X16);
  oled.clear();
  oled.on();
}

void loop() {
  if (!settingMode) {
    // Automatické aktualizovanie času
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      seconds++;
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
          minutes = 0;
          hours++;
          if (hours >= 24) {
            hours = 0;
          }
        }
      }
    }
  }

  // Kontrola tlačidiel
  if (digitalRead(ENTER_BUTTON) == LOW) {
    delay(200); // Debouncing
    settingMode = !settingMode;
    settingOption = 0; // Reset nastavenia na hodiny
  }

  if (settingMode) {
    if (digitalRead(UP_BUTTON) == LOW) {
      delay(200); // Debouncing
      if (settingOption == 0) {
        hours = (hours + 1) % 24;
      } else if (settingOption == 1) {
        minutes = (minutes + 1) % 60;
      }
    }

    if (digitalRead(DOWN_BUTTON) == LOW) {
      delay(200); // Debouncing
      if (settingOption == 0) {
        hours = (hours - 1 + 24) % 24;
      } else if (settingOption == 1) {
        minutes = (minutes - 1 + 60) % 60;
      }
    }

    if (digitalRead(ENTER_BUTTON) == LOW) {
      delay(200); // Debouncing
      settingOption = (settingOption + 1) % 2; // Prepnutie medzi hodinami a minútami
    }
  }

  // Zobrazenie na OLED displeji
  oled.clear();
  oled.setCursor(0, 2);
  oled.print(hours < 10 ? "0" : "");
  oled.print(hours);
  oled.print(":");
  oled.print(minutes < 10 ? "0" : "");
  oled.print(minutes);
  oled.print(":");
  oled.print(seconds < 10 ? "0" : "");
  oled.print(seconds);

  if (settingMode) {
    oled.setCursor(0, 5);
    if (settingOption == 0) {
      oled.print("Set Hours");
    } else if (settingOption == 1) {
      oled.print("Set Minutes");
    }
  }
  oled.display();
}