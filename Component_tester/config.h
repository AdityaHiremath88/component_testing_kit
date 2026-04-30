#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <math.h>
#include <string.h>

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C oled;
extern const char* tpNames[3];

// Buttons
const int BTN_NEXT = PB13;
const int BTN_OK   = PB14;

// Resistor
const int RES_RANGE_PIN = PB0;
const int RES_SENSE_PIN = PA0;
const float RES_KNOWN_R = 1000.0;

// Capacitor
const int CAP_SENSE_PIN = PA1;
const int CAP_SMALL_CHARGE_PIN = PA8;
const int CAP_LARGE_CHARGE_PIN = PA5;
const int CAP_DISCHARGE_PIN = PB15;

// Component tester
const int strongPins[3] = {PA2, PA3, PA4};
const int weakPins[3]   = {PB9, PB8, PB5};
const int sensePins[3]  = {PA6, PA7, PA11};

// ADC / voltage
const float VCC = 3.3;
const int ADC_MAX = 4095;