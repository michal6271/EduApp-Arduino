#include "Arduino.h"
#include "Component.h"

class BooleanLedComponent : public Component {
  const char* name = "BooleanLedComponent";
  char address[5];
  int pin;
  
  public:
  BooleanLedComponent(int pin)
  {
      this->pin = pin;
      sprintf(address, "D%d", pin);
      pinMode(pin, OUTPUT);
  }

  const char* getAddress() {
    return address;
  }

  const char* getName() {
    return name;
  }

  bool getState() {
    return digitalRead(this->pin);
  }

  void setState(bool state) {
    digitalWrite(this->pin, state);
  }
};

