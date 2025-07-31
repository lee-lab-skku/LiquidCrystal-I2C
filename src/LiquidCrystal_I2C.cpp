#include "LiquidCrystal_I2C.h"
#include <inttypes.h>
#include <Arduino.h>

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize, TwoWire& wire):
	_addr(lcd_addr),
	_displayfunction(LCD::Function::Bit4 | LCD::Function::Lines1 | LCD::Function::Font5x8),
	_displaycontrol(LCD::Control::On | LCD::Control::CursorOff | LCD::Control::BlinkOff),
	_displaymode(LCD::Mode::Left | LCD::Mode::ShiftDecr),
	_cols(lcd_cols),
	_rows(lcd_rows),
	_charsize(charsize),
	_backlightval(LCD::Backlight::On),
	_Wire(wire)
{}

void LiquidCrystal_I2C::init() {
	_Wire.begin();
	begin();
}

void LiquidCrystal_I2C::begin() {
	if (_rows > 1) {
		_displayfunction |= LCD::Function::Lines2; // 2-line display
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1)) {
		_displayfunction |= LCD::Function::Font5x10;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50);
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	write4bits(0x03 << 4);
	delayMicroseconds(4500);
	write4bits(0x03 << 4);
	delayMicroseconds(4500);
	write4bits(0x03 << 4);
	delayMicroseconds(150);
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD::Command::FunctionSet | _displayfunction);

	display();

	// clear it off
	clear();

	// set the entry mode
	command(LCD::Command::EntryModeSet | _displaymode);

	home();
}

/********** high level commands, for the user! */
void LiquidCrystal_I2C::clear(){
	command(LCD::Command::ClearDisplay);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::home(){
	command(LCD::Command::ReturnHome);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > _rows) {
		row = _rows-1;    // we count rows starting w/0
	}
	command(LCD::Command::SetDDRAMAddr | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C::noDisplay() {
	_displaycontrol &= ~LCD::Control::On;
	command(LCD::Command::DisplayControl | _displaycontrol);
}
void LiquidCrystal_I2C::display() {
	_displaycontrol |= LCD::Control::On;
	command(LCD::Command::DisplayControl | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C::noCursor() {
	_displaycontrol &= ~LCD::Control::CursorOn;
	command(LCD::Command::DisplayControl | _displaycontrol);
}
void LiquidCrystal_I2C::cursor() {
	_displaycontrol |= LCD::Control::CursorOn;
	command(LCD::Command::DisplayControl | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C::noBlink() {
	_displaycontrol &= ~LCD::Control::BlinkOn;
	command(LCD::Command::DisplayControl | _displaycontrol);
}
void LiquidCrystal_I2C::blink() {
	_displaycontrol |= LCD::Control::BlinkOn;
	command(LCD::Command::DisplayControl | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C::scrollDisplayLeft(void) {
	command(LCD::Shift::CursorMove | LCD::Shift::DisplayMove | LCD::Shift::MoveLeft);
}
void LiquidCrystal_I2C::scrollDisplayRight(void) {
	command(LCD::Shift::CursorMove | LCD::Shift::DisplayMove | LCD::Shift::MoveRight);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C::leftToRight(void) {
	_displaymode |= LCD::Mode::Left;
	command(LCD::Command::EntryModeSet | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C::rightToLeft(void) {
	_displaymode &= ~LCD::Mode::Left;
	command(LCD::Command::EntryModeSet | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C::autoscroll(void) {
	_displaymode |= LCD::Mode::ShiftIncr;
	command(LCD::Command::EntryModeSet | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C::noAutoscroll(void) {
	_displaymode &= ~LCD::Mode::ShiftIncr;
	command(LCD::Command::EntryModeSet | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD::Command::SetCGRAMAddr | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C::noBacklight(void) {
	_backlightval=LCD::Backlight::Off;
	expanderWrite(0);
}

void LiquidCrystal_I2C::backlight(void) {
	_backlightval=LCD::Backlight::On;
	expanderWrite(0);
}
bool LiquidCrystal_I2C::getBacklight() {
  return _backlightval == LCD::Backlight::On;
}


/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2C::command(uint8_t value) {
	send(value, 0);
}

inline size_t LiquidCrystal_I2C::write(uint8_t value) {
	send(value, LCD::Pin::Rs); // Rs = 1, data
	return 1;
}


/************ low level data pushing commands **********/

// write either command or data
void LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void LiquidCrystal_I2C::write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void LiquidCrystal_I2C::expanderWrite(uint8_t _data){
	_Wire.beginTransmission(_addr);
	_Wire.write((int)(_data) | _backlightval);
	_Wire.endTransmission();
}

void LiquidCrystal_I2C::pulseEnable(uint8_t _data){
	expanderWrite(_data | LCD::Pin::En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns

	expanderWrite(_data & ~LCD::Pin::En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
}
