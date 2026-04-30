#include "config.h"
#include "ui.h"
#include "resistor.h"

static int rangePins[4] = {PB0, PB1, PB10, PB11};
static float knownResistors[4] = {1000.0, 22000.0, 100000.0, 1000000.0};
static const char* rangeNames[4] = {"1K", "22K", "100K", "1M"};

static int sensePin = PA0;
static int adcMax = 4095;

static void turnOffAllRanges() {
  for (int i = 0; i < 4; i++) {
    pinMode(rangePins[i], INPUT);
  }
}

static int readADCFromRange(int index) {
  turnOffAllRanges();

  pinMode(rangePins[index], OUTPUT);
  digitalWrite(rangePins[index], HIGH);

  delay(20);

  for (int i = 0; i < 10; i++) {
    analogRead(sensePin);
    delay(2);
  }

  long total = 0;
  for (int i = 0; i < 20; i++) {
    total += analogRead(sensePin);
    delay(2);
  }

  int average = total / 20;

  pinMode(rangePins[index], INPUT);

  return average;
}

static void showResistorMessage(const char* line1, const char* line2) {
  showMessage(line1, line2, "OK = Back");
}

static void showResistance(float resistance, int rangeIndex, int adcValue) {
  char valueText[20];
  char numberText[12];
  char rangeText[20];
  char adcText[20];

  if (resistance < 1000.0) {
    dtostrf(resistance, 6, 1, numberText);
    sprintf(valueText, "%s Ohm", numberText);
  } 
  else if (resistance < 1000000.0) {
    dtostrf(resistance / 1000.0, 6, 2, numberText);
    sprintf(valueText, "%s KOhm", numberText);
  } 
  else {
    dtostrf(resistance / 1000000.0, 6, 3, numberText);
    sprintf(valueText, "%s MOhm", numberText);
  }

  sprintf(rangeText, "Range: %s", rangeNames[rangeIndex]);
  sprintf(adcText, "ADC: %d", adcValue);

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tr);
  oled.drawStr(0, 14, "Resistor");
  oled.drawStr(0, 32, valueText);
  oled.drawStr(0, 48, rangeText);
  oled.drawStr(0, 62, adcText);
  oled.sendBuffer();
}

void runResistorMode() {
  pinMode(sensePin, INPUT_ANALOG);
  turnOffAllRanges();

  while (true) {
    float bestResistance = -1;
    int bestRange = -1;
    int bestAdc = 0;
    int bestDistance = 5000;

    for (int i = 0; i < 4; i++) {
      int adcValue = readADCFromRange(i);

      // Ignore very low and very high values
      if (adcValue <= 1 || adcValue >= 4094) {
        continue;
      }

      float unknownResistance = knownResistors[i] * ((float)adcValue / (adcMax - adcValue));
      int distance = abs(adcValue - 2048);

      if (distance < bestDistance) {
        bestDistance = distance;
        bestRange = i;
        bestAdc = adcValue;
        bestResistance = unknownResistance;
      }
    }

    if (bestRange == -1) {
      showResistorMessage("Resistor", "Insert Resistor");
    } else {
      showResistance(bestResistance, bestRange, bestAdc);
    }

    if (buttonPressed(BTN_OK)) {
      break;
    }

    delay(300);
  }

  turnOffAllRanges();
}