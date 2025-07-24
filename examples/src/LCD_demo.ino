#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Wire.begin();
    lcd.begin();
    lcd.backlight();
    lcd.print("Hello, LeeLab!");
}

void loop() {
}