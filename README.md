# component_testing_kit
# Component Testing Kit

A versatile, Arduino-based tool designed to identify and measure a wide range of electronic components. Featuring an intuitive OLED interface, this kit can test and provide values for resistors, capacitors, diodes, LEDs, BJTs (Bipolar Junction Transistors), and MOSFETs.[reference:0]

## ✨ Features
- **Multi-Component Support**: Seamlessly test 6 different component types
- **User-Friendly Interface**: 128x64 OLED display with clear navigation[reference:1]
- **High-Resolution Measurement**: 12-bit analog-to-digital conversion for precise readings[reference:2]
- **Automatic Component Identification**: Intelligent pin detection eliminates the need for manual configuration
- **Dual Capacitor Mode**: Dedicated small and high-capacitance mode for accurate measurements across different ranges[reference:3]

## 🛠️ Hardware Requirements
To build this Component Testing Kit, you will need the following:

### Core Components
| Component Type | Designation / Purpose |
|:---|:---|
| **Microcontroller** | STM32 board with Arduino IDE support |
| **Display** | 128x64 SH1106 OLED (I2C interface)[reference:4] |
| **User Controls** | Two tactile push buttons |
| **Test Interface** | Three test points (TP1, TP2, TP3)[reference:5] |

### Pin Configuration
The complete pin mapping is defined in `config.h`. Key connections are as follows:

| Category | Pins |
|:---|:---|
| **Buttons** | NEXT (PB13), OK (PB14) |
| **Resistor Testing** | Range control (PB0), Sense ADC (PA0) |
| **Capacitor Testing** | Sense ADC (PA1), Discharge (PB15), <br>Small cap charge (PA8), Large cap charge (PA5) |
| **Component Tester** | Strong drive: PA2, PA3, PA4 <br>Weak drive: PB9, PB8, PB5 <br>Sense ADC: PA6, PA7, PA11 |

The code also uses PB6 (SCL) and PB7 (SDA) for I2C communication.[reference:6]

## 🚀 Getting Started
To begin using the Component Testing Kit, follow these steps:

### 1. Clone the Repository
```bash
git clone https://github.com/AdityaHiremath88/component_testing_kit.git
cd component_testing_kit/Component_tester
