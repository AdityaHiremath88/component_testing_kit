#include "config.h"
#include "ui.h"
#include "cap_large.h"

static const float R = 10000.0;
static const int ADC_TARGET = 2589;

static int averageADC() {
  long total = 0;
  for (int i = 0; i < 16; i++) {
    total += analogRead(CAP_SENSE_PIN);
    delayMicroseconds(50);
  }
  return total / 16;
}

static void dischargeCap() {
  pinMode(CAP_LARGE_CHARGE_PIN, INPUT);

  pinMode(CAP_DISCHARGE_PIN, OUTPUT);
  digitalWrite(CAP_DISCHARGE_PIN, LOW);
  delay(3000);

  pinMode(CAP_DISCHARGE_PIN, INPUT);
}

static float measureCap() {
  dischargeCap();

  unsigned long startTime = micros();

  pinMode(CAP_LARGE_CHARGE_PIN, OUTPUT);
  digitalWrite(CAP_LARGE_CHARGE_PIN, HIGH);

  delayMicroseconds(100);

  while (averageADC() < ADC_TARGET) {
    if (micros() - startTime > 20000000UL) {
      pinMode(CAP_LARGE_CHARGE_PIN, INPUT);
      return NAN;
    }
    delayMicroseconds(200);
  }

  unsigned long elapsed = micros() - startTime;
  pinMode(CAP_LARGE_CHARGE_PIN, INPUT);

  float k = -log(1.0 - ((float)ADC_TARGET / 4095.0));
  float capF = (elapsed * 1e-6) / (R * k);

  return capF;
}

static void showCapLargeValue(float capF) {
  char num[16];
  char txt[24];

  if (capF < 1e-6) {
    dtostrf(capF * 1e9, 6, 2, num);
    sprintf(txt, "%s nF", num);
  } else if (capF < 1e-3) {
    dtostrf(capF * 1e6, 6, 2, num);
    sprintf(txt, "%s uF", num);
  } else {
    dtostrf(capF * 1e3, 6, 2, num);
    sprintf(txt, "%s mF", num);
  }

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tr);
  oled.drawStr(0, 20, "Cap Large");
  oled.drawStr(0, 40, txt);
  oled.drawStr(0, 60, "OK = Back");
  oled.sendBuffer();
}

void runCapLargeMode() {
  pinMode(CAP_SENSE_PIN, INPUT_ANALOG);
  pinMode(CAP_LARGE_CHARGE_PIN, INPUT);
  pinMode(CAP_DISCHARGE_PIN, INPUT);

  while (true) {
    float capF = measureCap();

    if (isnan(capF) || capF < 1e-8) {
      showMessage("Cap Large", "Insert Capacitor", "OK = Back");
    } else {
      showCapLargeValue(capF);
    }

    if (buttonPressed(BTN_OK)) {
      break;
    }

    delay(500);
  }

  pinMode(CAP_LARGE_CHARGE_PIN, INPUT);
  pinMode(CAP_DISCHARGE_PIN, INPUT);
}