#pragma once

bool buttonPressed(int pin);
void showMessage(const char* line1, const char* line2, const char* line3);
void drawMenu(const char* title, const char* items[], int count, int index);
void showMessageWait(const char* line1, const char* line2, const char* line3);