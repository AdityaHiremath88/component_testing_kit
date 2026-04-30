#include "config.h"
#include "ui.h"
#include "bjt.h"

static void releaseComponentPins() {
  for (int i = 0; i < 3; i++) {
    pinMode(strongPins[i], INPUT);
    pinMode(weakPins[i], INPUT);
  }
}

static void setStrongHigh(int i) {
  pinMode(strongPins[i], OUTPUT);
  digitalWrite(strongPins[i], HIGH);
}

static void setStrongLow(int i) {
  pinMode(strongPins[i], OUTPUT);
  digitalWrite(strongPins[i], LOW);
}

static void setWeakHigh(int i) {
  pinMode(weakPins[i], OUTPUT);
  digitalWrite(weakPins[i], HIGH);
}

static void setWeakLow(int i) {
  pinMode(weakPins[i], OUTPUT);
  digitalWrite(weakPins[i], LOW);
}

static float readTP(int i) {
  int adc = analogRead(sensePins[i]);
  return (adc * VCC) / ADC_MAX;
}

static bool diodeForward(int from, int to, float &vf) {
  releaseComponentPins();

  setStrongHigh(from);
  setStrongLow(to);
  delay(5);

  float vFrom = readTP(from);
  float vTo = readTP(to);

  releaseComponentPins();

  vf = vFrom - vTo;

  return (vf > 0.15 && vf < 1.2);
}

static int estimateHFE(bool isNPN, int base, int collector, int emitter) {
  releaseComponentPins();

  if (isNPN) {
    setWeakHigh(base);
    setStrongHigh(collector);
    setStrongLow(emitter);
  } else {
    setWeakLow(base);
    setStrongLow(collector);
    setStrongHigh(emitter);
  }

  delay(10);

  float vBase = readTP(base);
  float vCollector = readTP(collector);

  releaseComponentPins();

  float ib = 0.0;
  float ic = 0.0;

  if (isNPN) {
    ib = (VCC - vBase) / 100000.0;
    ic = (VCC - vCollector) / 1000.0;
  } else {
    ib = vBase / 100000.0;
    ic = vCollector / 1000.0;
  }

  if (ib < 0.0000001) return 0;

  int hfe = (int)(ic / ib);

  if (hfe < 0) hfe = 0;
  if (hfe > 999) hfe = 999;

  return hfe;
}

void runBJTMode() {
  releaseComponentPins();

  while (true) {
    int base = -1;
    int p1 = -1;
    int p2 = -1;
    bool isNPN = false;

    for (int b = 0; b < 3; b++) {
      int x = (b == 0) ? 1 : 0;
      int y = 3 - b - x;

      float vf1, vf2, vf3, vf4;

      bool b_to_x = diodeForward(b, x, vf1);
      bool b_to_y = diodeForward(b, y, vf2);
      bool x_to_b = diodeForward(x, b, vf3);
      bool y_to_b = diodeForward(y, b, vf4);

      if (b_to_x && b_to_y && !x_to_b && !y_to_b) {
        base = b;
        p1 = x;
        p2 = y;
        isNPN = true;
        break;
      }

      if (x_to_b && y_to_b && !b_to_x && !b_to_y) {
        base = b;
        p1 = x;
        p2 = y;
        isNPN = false;
        break;
      }
    }

    if (base == -1) {
      showMessage("BJT Test", "No BJT Found", "OK = Back");
    } else {
      int h1 = estimateHFE(isNPN, base, p1, p2);
      int h2 = estimateHFE(isNPN, base, p2, p1);

      int collector, emitter, hfe;

      if (h1 >= h2) {
        collector = p1;
        emitter = p2;
        hfe = h1;
      } else {
        collector = p2;
        emitter = p1;
        hfe = h2;
      }

      char line1[20];
      char line2[24];
      char line3[24];

      if (isNPN) strcpy(line1, "NPN");
      else strcpy(line1, "PNP");

      sprintf(line2, "B=%s C=%s", tpNames[base], tpNames[collector]);
      sprintf(line3, "E=%s hFE=%d", tpNames[emitter], hfe);

      showMessage(line1, line2, line3);
    }

    if (buttonPressed(BTN_OK)) {
      break;
    }

    delay(1200);
  }

  releaseComponentPins();
}