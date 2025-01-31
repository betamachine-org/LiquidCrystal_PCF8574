/// \file LiquidCrystal_PCF8574.cpp
/// \brief LiquidCrystal library with PCF8574 I2C adapter.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) 2019 by Matthias Hertel.
///
/// ChangeLog see: LiquidCrystal_PCF8574.h

#include "LiquidCrystal_PCF8574.h"

#include <Wire.h>

/// Definitions on how the PCF8574 is connected to the LCD

/// These are Bit-Masks for the special signals and background light
#define PCF_RS 0x01
#define PCF_RW 0x02
#define PCF_EN 0x04
#define PCF_BACKLIGHT 0x08
// the 0xF0 bits are used for 4-bit data to the display.

// a nibble is a half Byte

LiquidCrystal_PCF8574::LiquidCrystal_PCF8574(int i2cAddr)
{
  _i2cAddr = i2cAddr;
  _backlight = 0;

  _entrymode = 0x02;         // Increment no shift
  _displaycontrol = 0x04;    // display on & no cursor
} // LiquidCrystal_PCF8574


void LiquidCrystal_PCF8574::begin(int cols, int lines)
{
  _cols = cols;
  _lines = lines;

  // initializing the display
  Wire.begin();

  // after reset the mode is this
  _displaycontrol = 0x04;       // Increment no shift
  _entrymode = 0x02;            // display on & no cursor
  // Init is now 100% in clear or init function
  //  _sendNibble(0x00, true);      //to setup  RS up and E down after power up
  //  // delayMicroseconds(200);    // no need I2C delay is enough
  //  _sendNibble(0x03);
  //
  //  _sendNibble(0x03);
  //  //delayMicroseconds(200);  // no need I2C delay is enough
  //  _sendNibble(0x03);
  //  //delayMicroseconds(200);  //  no need I2C delay is enough
  //  _sendNibble(0x02);   // finally, set to 4-bit interface
  //  _send(0x24 | functionFlags);
  clear();   // all reinit of LCD register are done here
  //_send(0x01);
  //delayMicroseconds(1000);
  display();
  leftToRight();
} // begin()

// do aclear and a full reinit
void LiquidCrystal_PCF8574::clear()
{
  // Instruction: Clear display = 0x01
  _send(0x01);
  // here we try to force LCD back to 4 bit mode in case of power loss
  // now LCD is blind (clear is long ) unless if it was in 8 bit mode
  // so try to reset it in 8 bit mode (a second clear is send in case of reset)
  _sendNibble(0x02); // mode 4 bit  will be ignored if LCD is clearing
  delayMicroseconds(2000); // this command takes up to 1.5ms! 
  int functionFlags = 0;

  if (_lines > 1) {
    functionFlags = 0x08;
  }
  // reset LCD registers
  _send(0x24 | functionFlags); // 2C if 2 lines
  _send(0x04 | _entrymode);
  _send(0x08 | _displaycontrol);
  //  _send(0x80);   // home
  _send(0x01);   // second clear to really clear if lcd was not init
  delayMicroseconds(2000); // this command takes 1.5ms!  - 4ms of I2C write
  _row = 0;
  _col = 0;
} // clear()


void LiquidCrystal_PCF8574::init()
{
  clear();
} // init()


void LiquidCrystal_PCF8574::home()
{
  setCursor(0, 0);
  // Instruction: Return home = 0x02
  //  _send(0x02);

  //  delayMicroseconds(1500); // this command takes 1.5ms! ONLY With Shift ON
  // _send(0x80);               // so use setcursor 0 0 no delay (if you dont use shift)
} // home()

/// Set the cursor to a new position but update lcd only if needed
void LiquidCrystal_PCF8574::setCursor(byte col, byte  row)
{
  if (_col != col || _row != row) setCursorLCD(col, row);
} // setCursor()

/// Set the cursor to a new position on lcd
void LiquidCrystal_PCF8574::setCursorLCD(byte col, byte row)
{
  col %= _cols;
  row %= _lines; 
  byte row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  // Instruction: Set DDRAM address = 0x80
  _send(0x80 | ((row_offsets[row] ) + col));
  _col = col;
  _row = row;
} // setCursor()


// Turn the display on/off (quickly)
void LiquidCrystal_PCF8574::noDisplay()
{
  // Instruction: Display on/off control = 0x08
  _displaycontrol &= ~0x04; // display
  _send(0x08 | _displaycontrol);
} // noDisplay()


void LiquidCrystal_PCF8574::display()
{
  // Instruction: Display on/off control = 0x08
  _displaycontrol |= 0x04; // display
  _send(0x08 | _displaycontrol);
} // display()


// Turns the underline cursor on/off
void LiquidCrystal_PCF8574::cursor()
{
  // Instruction: Display on/off control = 0x08
  _displaycontrol |= 0x01; // cursor
  _send(0x08 | _displaycontrol);
} // cursor()


void LiquidCrystal_PCF8574::noCursor()
{
  // Instruction: Display on/off control = 0x08
  _displaycontrol &= ~0x01; // cursor
  _send(0x08 | _displaycontrol);
} // noCursor()


// Turn on and off the blinking cursor
void LiquidCrystal_PCF8574::blink()
{
  // Instruction: Display on/off control = 0x08
  _displaycontrol |= 0x01; // blink
  _send(0x08 | _displaycontrol);
} // blink()


void LiquidCrystal_PCF8574::noBlink()
{
  // Instruction: Display on/off control = 0x08
  _displaycontrol &= ~0x01; // blink
  _send(0x08 | _displaycontrol);
} // noBlink()


// These commands scroll the display without changing the RAM
void LiquidCrystal_PCF8574::scrollDisplayLeft(void)
{
  // Instruction: Cursor or display shift = 0x10
  // shift: 0x08, left: 0x00
  _send(0x10 | 0x08 | 0x00);
} // scrollDisplayLeft()


void LiquidCrystal_PCF8574::scrollDisplayRight(void)
{
  // Instruction: Cursor or display shift = 0x10
  // shift: 0x08, right: 0x04
  _send(0x10 | 0x08 | 0x04);
} // scrollDisplayRight()


// == controlling the entrymode

// This is for text that flows Left to Right
void LiquidCrystal_PCF8574::leftToRight(void)
{
  // Instruction: Entry mode set, set increment/decrement =0x02
  _entrymode |= 0x02;
  _send(0x04 | _entrymode);
} // leftToRight()


// This is for text that flows Right to Left
void LiquidCrystal_PCF8574::rightToLeft(void)
{
  // Instruction: Entry mode set, clear increment/decrement =0x02
  _entrymode &= ~0x02;
  _send(0x04 | _entrymode);
} // rightToLeft()


// This will 'right justify' text from the cursor
void LiquidCrystal_PCF8574::autoscroll(void)
{
  // Instruction: Entry mode set, set shift S=0x01
  _entrymode |= 0x01;
  _send(0x04 | _entrymode);
} // autoscroll()


// This will 'left justify' text from the cursor
void LiquidCrystal_PCF8574::noAutoscroll(void)
{
  // Instruction: Entry mode set, clear shift S=0x01
  _entrymode &= ~0x01;
  _send(0x04 | _entrymode);
} // noAutoscroll()


/// Setting the brightness of the background display light.
/// The backlight can be switched on and off.
/// The current brightness is stored in the private _backlight variable to have it available for further data transfers.
void LiquidCrystal_PCF8574::setBacklight(int brightness)
{
  _backlight = brightness;
  // send no data but set the background-pin right;
  _send(0x00, false);
} // setBacklight()


// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_PCF8574::createChar(int location, int charmap[])
{
  location &= 0x7; // we only have 8 locations 0-7
  // Set CGRAM address
  _send(0x40 | (location << 3));
  for (int i = 0; i < 8; i++) {
    writeLCD(charmap[i]);
  }
} // createChar()

/* The write function is needed for derivation from the Print class. */
size_t LiquidCrystal_PCF8574::write(uint8_t ch) {
  switch (ch) {
    case '\x01': clear(); break;
    case '\x02': home(); break;
    case '\x03': {  // fill up to end of line with ' ' and set back cursor in place
      for (byte N = _col; N<_cols; N++) writeLCD(' ');
      setCursorLCD(_col,_row);
      break;
    }
    case '\r': setCursorLCD(0,_row); break;
    case '\n': setCursorLCD(_col,_row+1); break;
    default:
      writeLCD(ch);
      // keep cursor inside the display
      if (++_col >= _cols) {
        setCursorLCD(0, _row +1);
      }
  }
  return 1;
}

// write any char
inline void LiquidCrystal_PCF8574::writeLCD(uint8_t ch)
{
  _send(ch, true);
} // write()


// write either command or data
void LiquidCrystal_PCF8574::_send(byte value, bool isData)
{
  Wire.beginTransmission(_i2cAddr);
  // write high 4 bits
  _writePCF((value >> 4), isData);
  // write low 4 bits
  _writePCF((value & 0x0F), isData);
  Wire.endTransmission();
} // _send()


// write a nibble / halfByte with handshake
void LiquidCrystal_PCF8574::_sendNibble(byte halfByte, bool isData)
{
  Wire.beginTransmission(_i2cAddr);
  _writePCF(halfByte, isData);
  Wire.endTransmission();
} // _sendNibble


// private function to change the PCF8674 pins to the given value
// Note:
// you may change this function what the display is attached to the PCF8574 in a different wiring.
//void LiquidCrystal_PCF8574::_write2Wire(byte halfByte, bool isData, bool enable)
void LiquidCrystal_PCF8574::_writePCF(byte halfByte, bool isData)
{
  // map the given values to the hardware of the I2C schema
  static bool i2cDataState = HIGH;  //  nasty way to keep track of RS state :)
  byte i2cData = halfByte << 4;

  // PCF_RW is never used.

  if (_backlight > 0)
    i2cData |= PCF_BACKLIGHT;

  if (isData)
    i2cData |= PCF_RS;

  // if RS change it is important to setup RS before setting EN high
  // to respect the timing setup.  few nano sec are needed but no choice here : 1 more I2Cwrite (800micro sec)
  // in fact its work without if power is full 5V  but make your display more robust if power is low
  // see chaper 7.1 Write timing of LCD documentation  NET234 02/2018
  if  (i2cDataState != isData) {
    i2cDataState = isData;
    Wire.write(i2cData);
  }

  Wire.write( i2cData | PCF_EN);   //  EN is up
  //  delayMicroseconds(37); // commands need > 37us to settle
  //  yes but I2C need 800us to write  :)
  Wire.write(i2cData);             // EN is down
} // write2Wire

// The End.
