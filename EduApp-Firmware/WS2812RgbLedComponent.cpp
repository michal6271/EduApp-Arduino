#include "Arduino.h"
#include "Component.h"
#include <Adafruit_NeoPixel.h>

class WS2812RgbLedComponent : public Component {
  const char* name = "WS2812RgbLedComponent";
  char address[16];
  Adafruit_NeoPixel* driver;
  uint8_t index;
  
  public:
  WS2812RgbLedComponent(Adafruit_NeoPixel* driver, uint8_t index)
  {
      this->driver = driver;
      this->index = index;
      sprintf(address, "Pixel %d @ D%d", index, driver->getPin());
      driver->begin();
  }

  const char* getAddress() {
    return address;
  }

  const char* getName() {
    return name;
  }

  uint8_t getRed() {
    return (driver->getPixelColor(index) >> 16) & 0xFF;
  }

  uint8_t getGreen() {
    return (driver->getPixelColor(index) >> 8) & 0xFF;
  }

  uint8_t getBlue() {
    return driver->getPixelColor(index) & 0xFF;
  }

  void setRed(uint8_t value) {
    driver->setPixelColor(index, value, getGreen(), getBlue());
  }

  void setGreen(uint8_t value) {
    driver->setPixelColor(index, getRed(), value, getBlue());
  }

  void setBlue(uint8_t value) {
    driver->setPixelColor(index, getRed(), getGreen(), value);
  }

  void update() {
    driver->show();
  }
};

