#include "config.h"
#include "ui.h"
#include "mosfet.h"

static const int GATE_PIN = 0;
static const int DRAIN_PIN = 1;
static const int SOURCE_PIN = 2;

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

static void dischargeGate() {
  releaseComponentPins();
  setWeakLow(GATE_PIN);
  delay(20);
  releaseComponentPins();
}

static bool bodyDiodeLooksLikeNMOS(float &vds1, float &vds2) {
  releaseComponentPins();

  setStrongHigh(DRAIN_PIN);
  setStrongLow(SOURCE_PIN);
  delay(10);

  float vd1 = readTP(DRAIN_PIN);
  float vs1 = readTP(SOURCE_PIN);
  vds1 = vd1 - vs1;

  releaseComponentPins();
  delay(10);

  setStrongHigh(SOURCE_PIN);
  setStrongLow(DRAIN_PIN);
  delay(10);

  float vs2 = readTP(SOURCE_PIN);
  float vd2 = readTP(DRAIN_PIN);
  vds2 = vs2 - vd2;

  releaseComponentPins();

  return (vds2 > 0.15 && vds2 < 1.2);
}

static bool gateTurnsOnNMOS(float &beforeVD, float &afterVD) {
  dischargeGate();

  releaseComponentPins();
  setStrongLow(SOURCE_PIN);
  setStrongHigh(DRAIN_PIN);
  delay(10);

  beforeVD = readTP(DRAIN_PIN);

  setWeakHigh(GATE_PIN);
  delay(50);

  afterVD = readTP(DRAIN_PIN);

  releaseComponentPins();

  return (afterVD < beforeVD - 0.15);
}

void runMOSFETMode() {
  releaseComponentPins();

  while (true) {
    float vds1 = 0;
    float vds2 = 0;
    float beforeVD = 0;
    float afterVD = 0;

    bool diodeOK = bodyDiodeLooksLikeNMOS(vds1, vds2);
    bool turnOnOK = gateTurnsOnNMOS(beforeVD, afterVD);

    if (diodeOK && turnOnOK) {
      showMessage("N-MOS Found", "TP1=G TP2=D", "TP3=S");
    } else if (diodeOK) {
      showMessage("Diode Found", "Maybe N-MOS", "Gate weak");
    } else if (turnOnOK) {
      showMessage("MOS action", "Check pin order", "TP1=G D2 S3");
    } else {
      showMessage("No N-MOS", "Use order:", "TP1=G TP2=D TP3=S");
    }

    if (buttonPressed(BTN_OK)) {
      break;
    }

    delay(1200);
  }

  releaseComponentPins();
}