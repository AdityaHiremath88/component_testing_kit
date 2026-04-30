#include "config.h"
#include "ui.h"
#include "resistor.h"
#include "cap_small.h"
#include "cap_large.h"
#include "diode.h"
#include "led.h"
#include "bjt.h"
#include "mosfet.h"
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
const char* tpNames[3] = {"TP1", "TP2", "TP3"};

const char* mainMenu[] = {
  "Resistor",
  "Capacitor",
  "Diode",
  "LED",
  "BJT",
  "MOSFET"
};

const char* capMenu[] = {
  "Small Cap",
  "Large Cap",
  "Back"
};

int currentScreen = 0;   // 0 = main menu, 1 = capacitor submenu
int mainIndex = 0;
int capIndex = 0;

void setup() {
  Wire.setSCL(PB6);
  Wire.setSDA(PB7);
  Wire.begin();
  Serial.begin(9600);
  oled.begin();
  analogReadResolution(12);

  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);

  showMessage("Multi Tester", "Starting...", "");
  delay(800);
}

void loop() {
  if (currentScreen == 0) {
    drawMenu("Main Menu", mainMenu, 6, mainIndex);

    if (buttonPressed(BTN_NEXT)) {
      mainIndex++;
      if (mainIndex >= 6) mainIndex = 0;
    }

    if (buttonPressed(BTN_OK)) {
      if (mainIndex == 0) runResistorMode();
      else if (mainIndex == 1) currentScreen = 1;
      else if (mainIndex == 2) runDiodeMode();
      else if (mainIndex == 3) runLEDMode();
      else if (mainIndex == 4) runBJTMode();
      else if (mainIndex == 5) runMOSFETMode();
    }
  }

  else if (currentScreen == 1) {
    drawMenu("Capacitor", capMenu, 3, capIndex);

    if (buttonPressed(BTN_NEXT)) {
      capIndex++;
      if (capIndex >= 3) capIndex = 0;
    }

    if (buttonPressed(BTN_OK)) {
      if (capIndex == 0) runCapSmallMode();
      else if (capIndex == 1) runCapLargeMode();
      else if (capIndex == 2) currentScreen = 0;
    }
  }

  delay(20);
}