#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#define LCD_I2CADR  0x4E / 2  //adresse LCD

LiquidCrystal_PCF8574 lcd(LCD_I2CADR); // set the LCD address to 0x27 for a 16 chars and 2 line display

int show = -1;

void setup()
{
  int error;

  Serial.begin(115200);
  // wait on Serial to be available on Leonardo
  while (!Serial);

  Serial.println("LCD...");

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
  //Init I2C
  //Wire.begin(D2,D1);
  Wire.begin();
  Serial.print('.');
  Wire.beginTransmission(LCD_I2CADR);
  Serial.print('.');
  error = Wire.endTransmission();
  Serial.print('.');
  Serial.print("I2C Status1: ");
  Serial.println(error);
  Serial.print('.');
 Wire.beginTransmission(LCD_I2CADR);
 Serial.print('.');
  error = Wire.endTransmission();
  Serial.print('.');
  Serial.print("I2C Status2: ");
  Serial.println(error);
  Serial.print('.');
   Wire.beginTransmission(LCD_I2CADR);
   Serial.print('.');
  error = Wire.endTransmission();
  Serial.print('.');
  Serial.print("I2C Status3: ");
  Serial.println(error);
  if (error == 0) {
    Serial.println(": LCD found.");
    show = 0;
    //    lcd.begin(20, 4); // initialize the lcd
    lcd.begin(16, 2); // initialize the lcd

  } else {
    Serial.println(": LCD not found.");
  } // if

} // setup()


void loop()
{
  Serial.print("Show=");
  Serial.println(show);
  
  if (show == 0) {
    Serial.print("Hello LCD");
    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
    lcd.print("Hello LCD");
    delay(400);

    lcd.setBacklight(0);
    delay(400);
    lcd.setBacklight(255);
    delay(400);
    lcd.setBacklight(0);
    delay(400);
    lcd.setBacklight(255);
    Serial.println(".");

  } else if (show == 1) {
    lcd.clear();
    lcd.print("Cursor On");
    lcd.cursor();

  } else if (show == 2) {
    lcd.clear();
    lcd.print("Cursor Blink");
    lcd.blink();

  } else if (show == 3) {
    lcd.clear();
    lcd.print("Cursor OFF");
    lcd.noBlink();
    lcd.noCursor();

  } else if (show == 4) {
    lcd.clear();
    lcd.print("Display Off");
    lcd.noDisplay();

  } else if (show == 5) {
    lcd.clear();
    lcd.print("Display On");
    lcd.display();

  } else if (show == 7) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("*** first line.");
    lcd.setCursor(0, 1);
    lcd.print("*** second line.");

  } else if (show == 8) {
    lcd.scrollDisplayLeft();
  } else if (show == 9) {
    lcd.scrollDisplayLeft();
  } else if (show == 10) {
    lcd.scrollDisplayLeft();
  } else if (show == 11) {
    lcd.scrollDisplayRight();

  } else if (show == 12) {
    //    lcd.clear();
    lcd.print(LCD_CLEAR "write-");

  } else if (show > 12 && show < 16) {
    lcd.print(show - 13);
  } else if (show == 16) {
    lcd.println(LCD_CLEAR "Very long line of 30 chars ...");
    //lcd.println("Last Line");
  } else if (show == 17) {
    lcd.println(">-" LCD_CLREOL "clear");
  } else if (show == 18) {
    lcd.println(">-" LCD_CLREOL "1 line");
  } else if (show == 19) {
    lcd.println(">-" LCD_CLREOL "by");
  } else if (show == 20) {
    lcd.println(">-" LCD_CLREOL "one line");

  } // if

  delay(1400);
  show = (show + 1) % 22;
} // loop()
