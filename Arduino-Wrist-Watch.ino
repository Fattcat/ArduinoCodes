#include <TinyWireM.h>
#include <Tiny4kOLED.h>

#define UP_BUTTON 3
#define DOWN_BUTTON 4
#define ENTER_BUTTON 1

// fixed display
// CONNECTION \\
// ----------------------------------------------------- \\
//        (RESET) PB5  - +-----+ -  VCC (OLED VCC)
//        Up Btn  PB3  - +     + -  PB2 (SCL to OLED)
//       Down Btn PB4  - +     + -  PB1 (Enter Btn)
//  Buttons GND   GND  - +-----+ -  PB0 (SDA to OLED)
//  - And OLED GND !
// ----------------------------------------------------- \\

int hours = 13; // Prednastavený čas (hodiny)
int minutes = 21; // Prednastavený čas (minúty)
int seconds = 0; // Prednastavený čas (sekundy)

bool settingMode = false; // Režim nastavovania
int settingOption = 0;    // 0 = hodiny, 1 = minúty
unsigned long buttonPressStart = 0; // Čas začiatku stlačenia ENTER
bool buttonPressed = false; // Stav tlačidla ENTER

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
  // Automatické aktualizovanie času, ak nie sme v režime nastavovania
  if (!settingMode) {
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

  // Čítanie stavu tlačidla ENTER
  if (digitalRead(ENTER_BUTTON) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      buttonPressStart = millis(); // Uložíme čas začiatku stlačenia
    }
  } else {
    if (buttonPressed) {
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= 3000) {  // Dlhé stlačenie (> 3 sekundy)
        // Uložíme nastavený čas a prepne režim nastavenia
        settingMode = false;
      } else if (pressDuration < 1000) {  // Krátke stlačenie (< 1 sekunda)
        // Prepnutie medzi hodinami a minútami
        if (settingMode) {
          settingOption = (settingOption + 1) % 2;
        } else {
          settingMode = true;  // Prejdeme do režimu nastavovania
          settingOption = 0;   // Začneme nastavovať hodiny
        }
      }
      buttonPressed = false;
    }
  }

  // Ovládanie tlačidlami UP a DOWN v režime nastavovania
  if (settingMode) {
    if (digitalRead(UP_BUTTON) == LOW) {
      delay(200); // Debouncing
      if (settingOption == 0) {
        hours = (hours + 1) % 24; // Zvýšenie hodín
      } else if (settingOption == 1) {
        minutes = (minutes + 1) % 60; // Zvýšenie minút
      }
    }

    if (digitalRead(DOWN_BUTTON) == LOW) {
      delay(200); // Debouncing
      if (settingOption == 0) {
        hours = (hours - 1 + 24) % 24; // Zníženie hodín
      } else if (settingOption == 1) {
        minutes = (minutes - 1 + 60) % 60; // Zníženie minút
      }
    }
  }

  // Zobrazenie času na OLED displeji
  oled.clear();

  // Zobrazenie kurzora nad hodiny alebo minúty v režime nastavovania
  if (settingMode) {
    if (settingOption == 0) {
      oled.setCursor(6, 0); // Kurzor nad hodiny
      oled.print("+");
    } else if (settingOption == 1) {
      oled.setCursor(28, 0); // Kurzor nad minúty
      oled.print("+");
    }
  }

  // Zobrazenie času (HH:MM:SS) na druhom riadku
  oled.setCursor(0, 2);
  oled.print(hours < 10 ? "0" : "");
  oled.print(hours);
  oled.print(":");
  oled.print(minutes < 10 ? "0" : "");
  oled.print(minutes);
  oled.print(":");
  oled.print(seconds < 10 ? "0" : "");
  oled.print(seconds);

  // Ak sme v režime nastavovania, pridáme text na spodok displeja
  if (settingMode) {
    oled.setCursor(0, 5);
    if (settingOption == 0) {
      oled.print("Set Hours");
    } else if (settingOption == 1) {
      oled.print("Set Minutes");
    }
  }
}
