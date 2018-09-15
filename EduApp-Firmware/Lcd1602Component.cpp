#include "Arduino.h"
#include "Component.h"
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

class Lcd1602Component : public Component {
  const char* name = "Lcd1602Component";
  char address[5];
  int i2cAddress;
  hd44780_I2Cexp lcd;
  
  public:
  Lcd1602Component(int i2cAddress)
  {
      this->i2cAddress = i2cAddress;
      sprintf(address, "0x%X", i2cAddress);
  }

  const char* getAddress() {
    return address;
  }

  const char* getName() {
    return name;
  }

  void begin() {
    lcd = hd44780_I2Cexp(i2cAddress, I2Cexp_MCP23008,7,6,5,4,3,2,1,HIGH);
    lcd.begin(16, 2);
  }

  void setText(const char* text) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(text);
  }
};

