#include "config.h"
#include "ui.h"
#include "cap_small.h"

static const float R = 1000000.0;
static const int ADC_TARGET = 2589;

static float offsetPF = 20.0;

static void dischargeCap() {
  pinMode(CAP_SMALL_CHARGE_PIN, INPUT);

  pinMode(CAP_DISCHARGE_PIN, OUTPUT);
  digitalWrite(CAP_DISCHARGE_PIN, LOW);
  delay(50);

  pinMode(CAP_DISCHARGE_PIN, INPUT);
}

static unsigned long measureOneTime() {
  pinMode(CAP_SMALL_CHARGE_PIN, OUTPUT);
  digitalWrite(CAP_SMALL_CHARGE_PIN, HIGH);

  unsigned long startTime = micros();

  while (analogRead(CAP_SENSE_PIN) < ADC_TARGET) {
    if (micros() - startTime > 500000UL) {
      pinMode(CAP_SMALL_CHARGE_PIN, INPUT);
      return 500000UL;
    }
  }

  unsigned long elapsed = micros() - startTime;
  pinMode(CAP_SMALL_CHARGE_PIN, INPUT);

  return elapsed;
}

static void sortArray(unsigned long *arr, int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      if (arr[j] < arr[i]) {
        unsigned long temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
}

static float measureCapPF() {
  unsigned long times[12];
  int count = 0;

  for (int i = 0; i < 12; i++) {
    dischargeCap();
    delay(2);

    unsigned long t = measureOneTime();

    if (t > 2 && t < 500000UL) {
      times[count++] = t;
    }
  }

  if (count < 5) return NAN;

  sortArray(times, count);

  int start = count / 4;
  int end = count - start;

  float sum = 0;
  for (int i = start; i < end; i++) {
    sum += times[i];
  }

  float avgTimeUs = sum / (end - start);

  float k = -log(1.0 - ((float)ADC_TARGET / 4095.0));
  float capPF = (avgTimeUs * 1e-6) / (R * k) * 1e12;

  capPF -= offsetPF;
  if (capPF < 0) capPF = 0;

  return capPF;
}

static void showCap(float capPF) {
  char num[16];
  char txt[24];

  if (capPF < 1000.0) {
    dtostrf(capPF, 6, 1, num);
    sprintf(txt, "%s pF", num);
  } else if (capPF < 1000000.0) {
    dtostrf(capPF / 1000.0, 6, 2, num);
    sprintf(txt, "%s nF", num);
  } else {
    dtostrf(capPF / 1000000.0, 6, 2, num);
    sprintf(txt, "%s uF", num);
  }

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tr);
  oled.drawStr(0, 20, "Cap Small");
  oled.drawStr(0, 40, txt);
  oled.drawStr(0, 60, "OK = Back");
  oled.sendBuffer();
}

void runCapSmallMode() {
  pinMode(CAP_SENSE_PIN, INPUT_ANALOG);

  while (true) {
    float capPF = measureCapPF();

    if (isnan(capPF) || capPF < 5.0) {
      showMessage("Cap Small", "Insert Cap", "OK = Back");
    } else {
      showCap(capPF);
    }

    if (buttonPressed(BTN_OK)) {
      break;
    }

    delay(300);
  }
}