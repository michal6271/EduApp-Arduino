#include "Arduino.h"
#include "Component.h"

class PwmRgbLedComponent : public Component {
  const char* name = "PwmRgbLedComponent";
  char address[20];
  uint8_t pinRed, pinGreen, pinBlue;
  uint8_t valRed, valGreen, valBlue;
  
  public:
  PwmRgbLedComponent(uint8_t r, uint8_t g, uint8_t b)
  {
      this->pinRed = r;
      this->pinGreen = g;
      this->pinBlue = b;
      sprintf(address, "D%d, D%d, D%d", r, g, b);
      pinMode(r, OUTPUT);
      pinMode(g, OUTPUT);
      pinMode(b, OUTPUT);
  }

  const char* getAddress() {
    return address;
  }

  const char* getName() {
    return name;
  }

  uint8_t getRed() {
    return valRed;
  }

  uint8_t getGreen() {
    return valGreen;
  }

  uint8_t getBlue() {
    return valBlue;
  }

  void setRed(uint8_t value) {
    this->valRed = value;
    analogWrite(this->pinRed, value);
  }

  void setGreen(uint8_t value) {
    this->valGreen = value;
    analogWrite(this->pinGreen, value);
  }

  void setBlue(uint8_t value) {
    this->valBlue = value;
    analogWrite(this->pinBlue, value);
  }
};

