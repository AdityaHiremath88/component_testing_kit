#include "config.h"
#include "ui.h"
#include "led.h"

static void releaseComponentPins() {
  for (int i = 0; i < 3; i++) {
    pinMode(strongPins[i], INPUT);
    pinMode(weakPins[i], INPUT);
  }
}

static void setHigh(int i) {
  pinMode(strongPins[i], OUTPUT);
  digitalWrite(strongPins[i], HIGH);
}

static void setLow(int i) {
  pinMode(strongPins[i], OUTPUT);
  digitalWrite(strongPins[i], LOW);
}

static float readTP(int i) {
  return analogRead(sensePins[i]) * VCC / ADC_MAX;
}

static bool testLED(int from, int to, float &vf) {
  releaseComponentPins();

  setHigh(from);
  setLow(to);
  delay(5);

  float vFrom = readTP(from);
  float vTo   = readTP(to);

  releaseComponentPins();

  vf = vFrom - vTo;

  return (vf > 1.2 && vf < 3.2);
}

static void glowLED(int from, int to, int ms) {
  releaseComponentPins();

  setHigh(from);
  setLow(to);

  unsigned long startTime = millis();

  while (millis() - startTime < ms) {
    // keep LED continuously ON
    delay(10);
  }

  releaseComponentPins();
}

void runLEDMode() {
  releaseComponentPins();

  while (true) {
    bool found = false;
    int ledFrom = -1;
    int ledTo = -1;
    float ledVf = 0;

    for (int i = 0; i < 3; i++) {
      for (int j = i + 1; j < 3; j++) {
        float vf1 = 0, vf2 = 0;

        bool l1 = testLED(i, j, vf1);
        bool l2 = testLED(j, i, vf2);

        if (l1 && !l2) {
          ledFrom = i;
          ledTo = j;
          ledVf = vf1;
          found = true;
          break;
        }

        if (l2 && !l1) {
          ledFrom = j;
          ledTo = i;
          ledVf = vf2;
          found = true;
          break;
        }
      }

      if (found) break;
    }

    if (found) {
      char line2[24];
      char line3[20];

      sprintf(line2, "%s -> %s", tpNames[ledFrom], tpNames[ledTo]);
      sprintf(line3, "Vf=%.2fV", ledVf);

      showMessage("LED Found", line2, line3);

      // longer glow for better visibility
      glowLED(ledFrom, ledTo, 1500);
    } 
    else {
      showMessage("LED Test", "No LED Found", "OK = Back");
      delay(300);
    }

    if (buttonPressed(BTN_OK)) {
      break;
    }
  }

  releaseComponentPins();
}