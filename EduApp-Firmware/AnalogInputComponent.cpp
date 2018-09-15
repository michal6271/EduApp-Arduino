#ifndef ANALOG_INPUT_COMPONENT_H
#define ANALOG_INPUT_COMPONENT_H

#include "Arduino.h"
#include "Component.h"

class AnalogInputComponent : public Component {
  const char* name;
  char address[5];
  int pin;
  
  public:
  AnalogInputComponent(int pin, const char* componentName) : name(componentName)
  {
      this->pin = pin;
      sprintf(address, "A%d", pin - A0);
      pinMode(pin, INPUT);
  }

  const char* getAddress() {
    return address;
  }

  const char* getName() {
    return name;
  }

  uint16_t getValue() {
    return analogRead(this->pin);
  }
};

#endif

