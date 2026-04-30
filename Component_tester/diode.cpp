#include "config.h"
#include "ui.h"
#include "diode.h"

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
  long total = 0;

  for (int k = 0; k < 8; k++) {
    total += analogRead(sensePins[i]);
    delayMicroseconds(100);
  }

  float adc = total / 8.0;
  return adc * VCC / ADC_MAX;
}

static bool testDiode(int from, int to, float &vf) {
  releaseComponentPins();

  setHigh(from);
  setLow(to);
  delay(8);

  float vFrom = readTP(from);
  float vTo   = readTP(to);

  releaseComponentPins();

  vf = vFrom - vTo;

  Serial.print(tpNames[from]);
  Serial.print(" -> ");
  Serial.print(tpNames[to]);
  Serial.print("  vFrom=");
  Serial.print(vFrom, 3);
  Serial.print("  vTo=");
  Serial.print(vTo, 3);
  Serial.print("  vf=");
  Serial.println(vf, 3);

  // Diode must have diode-like voltage AND real driven voltages
  return (vf > 0.15 && vf < 1.20 && vFrom > 1.20 && vTo > 0.10);
}

void runDiodeMode() {
  releaseComponentPins();

  while (true) {
    bool found = false;
    int diodeFrom = -1;
    int diodeTo = -1;
    float diodeVf = 0;

    for (int i = 0; i < 3; i++) {
      for (int j = i + 1; j < 3; j++) {
        float vf1 = 0;
        float vf2 = 0;

        bool d1 = testDiode(i, j, vf1);
        bool d2 = testDiode(j, i, vf2);

        if (d1 && !d2) {
          diodeFrom = i;
          diodeTo = j;
          diodeVf = vf1;
          found = true;
          break;
        }

        if (d2 && !d1) {
          diodeFrom = j;
          diodeTo = i;
          diodeVf = vf2;
          found = true;
          break;
        }
      }

      if (found) break;
    }

    if (found) {
      char line2[24];
      char line3[20];

      sprintf(line2, "%s -> %s", tpNames[diodeFrom], tpNames[diodeTo]);
      sprintf(line3, "Vf=%.2fV", diodeVf);

      showMessage("Diode Found", line2, line3);
    } else {
      showMessage("Diode Test", "No Diode Found", "OK = Back");
    }

    if (buttonPressed(BTN_OK)) {
      releaseComponentPins();
      return;
    }

    delay(800);
  }
}