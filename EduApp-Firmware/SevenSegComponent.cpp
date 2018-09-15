#include "Arduino.h"
#include "Component.h"
#include "Adafruit_MCP23008.h"

class SevenSegComponent : public Component {
  const char* name = "SevenSegComponent";
  char address[5];
  int i2cAddress;
  Adafruit_MCP23008 expander;
  
  public:  
  SevenSegComponent(int i2cAddress)
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
    expander.begin(i2cAddress - 0x20);
    for(int x = 0; x < 8; x++) {
      expander.pinMode(x, OUTPUT);
      expander.digitalWrite(x, LOW);
    }
  }

  uint8_t getSegments() {
    return expander.readGPIO();
  }

  void setSegments(uint8_t segments) {
    expander.writeGPIO(segments);
  }
};

