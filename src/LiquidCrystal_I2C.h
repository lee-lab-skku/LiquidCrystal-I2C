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


/*!
 * @class LiquidCrystal_I2C
 * @brief Library class for I2C interface HD44780 LCD displays shielded with DFRobot I2C interface.
 *
 * This class provides functions to control character LCDs via the I2C interface, 
 * including printing text, cursor control, backlight management, custom character creation, and more.
 */
class LiquidCrystal_I2C: public Print {
	public:
		/**
		 * @brief Constructor for LiquidCrystal_I2C.
		 *
		 * @param lcd_addr   I2C slave address of the LCD display (usually printed on the PCB or specified in documentation).
		 * @param lcd_cols   Number of columns (characters per line) of the LCD display.
		 * @param lcd_rows   Number of rows (lines) of the LCD display.
		 * @param charsize   Character dot size; use LCD::Function::Font5x10 or LCD::Function::Font5x8.
		 * @param wire       (Optional) Reference to the I2C bus to use (default: Wire).
		 */
		LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize = LCD::Function::Font5x8, TwoWire& wire = Wire);

		/**
		 * @brief Destructor (does nothing).
		 */
		~LiquidCrystal_I2C() = default;

		/**
		 * @brief Compatibility function for legacy Arduino source codes.
		 * @warning This function is deprecated and should not be used in new code.
		 * @see begin()
		 * @deprecated This function calls begin() on the global Wire object, which should be avoided.
		 * 				Instead, do it explicitly in your setup() function, then call begin() on this instance.
		 */
		void init();

		/**
		 * @brief Initializes the LCD display and prepares it for use.
		 * 
		 * This function must be called before using other LCD functions such as print(), write(), or clear().
		 */
		void begin();

		/**
		 * @brief Clears all characters currently shown on the LCD.
		 * 
		 * The display will be empty, and the cursor will return to the (0,0) position.
		 */
		void clear();

		/**
		 * @brief Moves the cursor to the home position (top-left corner).
		 * 
		 * The display content is unchanged, but the next print/write will start from (0,0).
		 */
		void home();

		/**
		 * @brief Turns the display off (hides all characters).
		 * 
		 * The backlight state is unaffected. Characters are preserved and will reappear when display() is called.
		 */
		void noDisplay();

		/**
		 * @brief Turns the display on (shows all characters).
		 * 
		 * Use this after noDisplay() to show the characters again.
		 */
		void display();

		/**
		 * @brief Disables blinking of the cursor indicator.
		 */
		void noBlink();

		/**
		 * @brief Enables blinking of the cursor indicator.
		 */
		void blink();

		/**
		 * @brief Hides the cursor indicator.
		 */
		void noCursor();

		/**
		 * @brief Shows the cursor indicator.
		 *
		 * Cursor blink can be controlled with blink() and noBlink().
		 */
		void cursor();

		/**
		 * @brief Scrolls the entire display content one position to the left.
		 */
		void scrollDisplayLeft();

		/**
		 * @brief Scrolls the entire display content one position to the right.
		 */
		void scrollDisplayRight();

		/**
		 * @brief Sets the text direction to left-to-right.
		 */
		void leftToRight();

		/**
		 * @brief Sets the text direction to right-to-left.
		 */
		void rightToLeft();

		/**
		 * @brief Turns the backlight off.
		 */
		void noBacklight();

		/**
		 * @brief Turns the backlight on.
		 */
		void backlight();

		/**
		 * @brief Returns the current state of the backlight.
		 * @retval true  Backlight is ON.
		 * @retval false Backlight is OFF.
		 */
		bool getBacklight();

		/**
		 * @brief Enables automatic scrolling of the display when new text is printed.
		 * 
		 * When autoscroll is enabled, text that reaches the edge will cause the display to shift.
		 */
		void autoscroll();

		/**
		 * @brief Disables automatic scrolling of the display.
		 */
		void noAutoscroll();

		/**
		 * @brief Creates a custom character (glyph) in one of the 8 supported locations.
		 *
		 * @param location  Memory location (0~7) to store the custom character.
		 * @param charmap   Pointer to an array of 8 bytes defining the 5x8 dot pattern.
		 */
		void createChar(uint8_t, uint8_t[]);

		/**
		 * @brief Sets the cursor to the specified position.
		 * @param col  Column position (0-based).
		 * @param row  Row position (0-based).
		 */
		void setCursor(uint8_t, uint8_t);

		/**
		 * @brief Writes a single character (byte) to the display.
		 *
		 * @param value  ASCII code of the character to write.
		 * @return The number of bytes written (typically 1).
		 *
		 * @note Overrides the write() method from Arduino's Print class.
		 */
		virtual size_t write(uint8_t);

		/**
		 * @brief Sends a raw command to the LCD controller.
		 * @param value  Command byte to send.
		 *
		 * @note This method is mid-level and usually not needed in user code.
		 */
		void command(uint8_t);

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
