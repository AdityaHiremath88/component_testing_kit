#include "config.h"
#include "ui.h"

bool buttonPressed(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(25);
    if (digitalRead(pin) == LOW) {
      while (digitalRead(pin) == LOW) delay(5);
      delay(25);
      return true;
    }
  }
  return false;
}

void showMessage(const char* line1, const char* line2, const char* line3) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tr);
  oled.drawStr(0, 14, line1);
  oled.drawStr(0, 32, line2);
  oled.drawStr(0, 50, line3);
  oled.sendBuffer();
}

void drawMenu(const char* title, const char* items[], int count, int index) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tr);
  oled.drawStr(0, 10, title);

  int top = 0;
  if (index > 3) top = index - 3;

  for (int i = 0; i < 4; i++) {
    int itemIndex = top + i;
    if (itemIndex >= count) break;

    int y = 24 + i * 10;
    if (itemIndex == index) oled.drawStr(0, y, ">");
    oled.drawStr(10, y, items[itemIndex]);
  }

  oled.sendBuffer();
}
void showMessageWait(const char* line1, const char* line2, const char* line3) {
  showMessage(line1, line2, line3);

  while (true) {
    if (buttonPressed(BTN_OK)) {
      break;
    }
    delay(20);
  }
}