#include "Arduino.h"
#include "Component.h"
#include "AnalogInputComponent.cpp"

class ThermometerComponent : public AnalogInputComponent {

  public:
  ThermometerComponent(int pin) : AnalogInputComponent(pin, "ThermometerComponent") {}

  float getConvertedValueCelsius() {
    return getValue() * 0.125 - 22.0;
  }
};

