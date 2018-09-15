#include "Arduino.h"
#include "Component.h"

class ButtonComponent : public Component {
  const char* name = "ButtonComponent";
  char address[5];
  int pin;
  
  public:
  ButtonComponent(int pin)
  {
      this->pin = pin;
      sprintf(address, "D%d", pin);
      pinMode(pin, INPUT);
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
};

