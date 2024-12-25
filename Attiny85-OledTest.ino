#include <TinyWireM.h>
#include <Tiny4kOLED.h>
// _____________________________________________________ //
// ! FOR UPLOAD MUST DO THIS !
// In Arduino IDE Program FIRST Arduino UNO as "ArduinoISP" 
// Then PASTE THIS CODE into Arduino IDE Sketch
// Install "Tiny4kOLED" & "TinyWireM" Libraries
// PLUG Attiny85 to your Arduino UNO and in Left top menu sellect this :
// - Board : Attiny25/45/85
// - Port : Arduino UNO board port
// - Clock : INTERNAL 8 MHz
// - Processor : Attiny85
// - Programmer : Arduino as ISP
// Then click Sketch -> and click Upload Using Programmer
// It should works without error
// _____________________________________________________ //
void setup() {

  // Send the initialization sequence to the oled. This leaves the display turned off
  // Added this cuz it is FOR FULL SIZE RESOLUTION OF DISPLAY (Not just 128x32 BUT 128x64)
  oled.begin(128,64,sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  // Clear the memory before turning on the display
  oled.clear();
  // Turn on the display
  oled.on();
  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  //oled.switchRenderFrame();
}

void loop() {

  // Clear the non-displayed half of the memory to all black
  // (The previous clear only cleared the other half of RAM)
  oled.clear();

  // The characters in the 8x16 font are 8 pixels wide and 16 pixels tall
  // 2 lines of 16 characters exactly fills 128x32
  oled.setFont(FONT8X16);

  // Position the cusror
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(12, 0);

  // Write the text to oled RAM (which is not currently being displayed)
  // Wrap strings in F() to save RAM!
  oled.print(F("Hello World!"));

  // Swap which half of RAM is being written to, and which half is being displayed
  //oled.switchFrame();

  delay(3000);
}
