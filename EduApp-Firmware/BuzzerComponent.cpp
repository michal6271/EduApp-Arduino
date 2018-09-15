#include "Arduino.h"
#include "Component.h"

class BuzzerComponent : public Component {
  const char* name = "BuzzerComponent";
  char address[5];
  int pin;
  
  public:
  BuzzerComponent(int pin)
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

