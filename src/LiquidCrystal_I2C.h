#pragma once

#include <Arduino.h>
#include <inttypes.h>
#include <Print.h>
#include <Wire.h>

namespace LCD {
	namespace Function {
		constexpr byte Bit8     = 0x10; // 8-bit interface
		constexpr byte Bit4     = 0x00; // 4-bit interface
		constexpr byte Lines1   = 0x00; // 1-line display
		constexpr byte Lines2   = 0x08; // 2-line display
		constexpr byte Font5x8  = 0x00; // 5×8 dot font
		constexpr byte Font5x10 = 0x04; // 5×10 dot font
	}

	namespace Control {
		constexpr byte On        = 0x04; // display on
		constexpr byte Off       = 0x00; // display off
		constexpr byte CursorOn  = 0x02; // cursor on
		constexpr byte CursorOff = 0x00; // cursor off
		constexpr byte BlinkOn   = 0x01; // blink on
		constexpr byte BlinkOff  = 0x00; // blink off
	}

	namespace Mode {
		constexpr byte Left       = 0x02; // I/D = 1 (increment)
		constexpr byte Right      = 0x00; // I/D = 0 (decrement)
		constexpr byte ShiftIncr  = 0x01; // S = 1
		constexpr byte ShiftDecr  = 0x00; // S = 0
	}

	namespace Shift {
		constexpr byte DisplayMove = 0x08; // SC = 1
		constexpr byte CursorMove  = 0x00; // SC = 0
		constexpr byte MoveRight   = 0x04; // RL = 1
		constexpr byte MoveLeft    = 0x00; // RL = 0
	}

	namespace Backlight {
		constexpr byte On  = 0x08;
		constexpr byte Off = 0x00;
	}

	namespace Pin {
		constexpr byte En = 0x04; // Enable
		constexpr byte Rw = 0x02; // Read/Write
		constexpr byte Rs = 0x01; // Register Select
	}

	namespace Command {
		constexpr byte ClearDisplay   = 0x01;
		constexpr byte ReturnHome     = 0x02;
		constexpr byte EntryModeSet   = 0x04;
		constexpr byte DisplayControl = 0x08;
		constexpr byte CursorShift    = 0x10;
		constexpr byte FunctionSet    = 0x20;
		constexpr byte SetCGRAMAddr   = 0x40;
		constexpr byte SetDDRAMAddr   = 0x80;
	}

} // namespace LCD


/**
 * This is the driver for the Liquid Crystal LCD displays that use the I2C bus.
 *
 * After creating an instance of this class, first call begin() before anything else.
 * The backlight is on by default, since that is the most likely operating mode in
 * most cases.
 */
class LiquidCrystal_I2C : public Print {
public:
	/**
	 * Constructor
	 *
	 * @param lcd_addr	I2C slave address of the LCD display. Most likely printed on the
	 *					LCD circuit board, or look in the supplied LCD documentation.
	 * @param lcd_cols	Number of columns your LCD display has.
	 * @param lcd_rows	Number of rows your LCD display has.
	 * @param charsize	The size in dots that the display has, use LCD::Function::Font5x10 or LCD::Function::Font5x8.
	 */
	LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize = LCD::Function::Font5x8, TwoWire& wire = Wire);

	/**
	 * Set the LCD display in the correct begin state, must be called before anything else is done.
	 */
	void begin();

	 /**
	  * Remove all the characters currently shown. Next print/write operation will start
	  * from the first position on LCD display.
	  */
	void clear();

	/**
	 * Next print/write operation will will start from the first position on the LCD display.
	 */
	void home();

	 /**
	  * Do not show any characters on the LCD display. Backlight state will remain unchanged.
	  * Also all characters written on the display will return, when the display in enabled again.
	  */
	void noDisplay();

	/**
	 * Show the characters on the LCD display, this is the normal behaviour. This method should
	 * only be used after noDisplay() has been used.
	 */
	void display();

	/**
	 * Do not blink the cursor indicator.
	 */
	void noBlink();

	/**
	 * Start blinking the cursor indicator.
	 */
	void blink();

	/**
	 * Do not show a cursor indicator.
	 */
	void noCursor();

	/**
 	 * Show a cursor indicator, cursor can blink on not blink. Use the
	 * methods blink() and noBlink() for changing cursor blink.
	 */
	void cursor();

	void scrollDisplayLeft();
	void scrollDisplayRight();
	void leftToRight();
	void rightToLeft();
	void noBacklight();
	void backlight();
	bool getBacklight();
	void autoscroll();
	void noAutoscroll();
	void createChar(uint8_t, uint8_t[]);
	void setCursor(uint8_t, uint8_t);
	virtual size_t write(uint8_t);
	void command(uint8_t);

	inline void blink_on() { blink(); }
	inline void blink_off() { noBlink(); }
	inline void cursor_on() { cursor(); }
	inline void cursor_off() { noCursor(); }

// Compatibility API function aliases
	void setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
	void load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
	void printstr(const char[]);

private:
	void send(uint8_t, uint8_t);
	void write4bits(uint8_t);
	void expanderWrite(uint8_t);
	void pulseEnable(uint8_t);
	uint8_t _addr;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _charsize;
	uint8_t _backlightval;
	TwoWire& _Wire;
};
