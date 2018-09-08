#include <ArduinoJson.h>
enum ComponentType { BooleanOut, RgbPwmLed, WS2812RgbLed, Oled, Lcd1602, Button, Disp7Seg, AnalogInput, BooleanInput, IRReceiver };
class Component {
  const char* address;
  ComponentType type;

  public:
  Component(const char* compAddress, ComponentType compType) : address(compAddress), type(compType) {}
};

const int JSON_BUFFER_CAPACITY = JSON_OBJECT_SIZE(16);
const int MAX_COMMAND_SIZE = 64;
const int TIMEOUT = 100;

StaticJsonBuffer<JSON_BUFFER_CAPACITY> JSONBuffer;

Component led1("D13", ComponentType::BooleanOut);

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long startTime = millis();
  char command[MAX_COMMAND_SIZE] = "";
  char temp;
  int charPosition = 0;
  bool gotCommand = false;
  
  while(millis() - startTime <= TIMEOUT) {
    while(Serial.available() > 0 && !gotCommand) {
      temp = Serial.read();
      startTime = millis();
      if(temp == '\r' || temp == '\n') gotCommand = true;
      command[charPosition++] = temp;
    }
  }
  if (gotCommand) {
    JsonObject& root = JSONBuffer.parseObject(command);
  
    root.prettyPrintTo(Serial);
    gotCommand = false;
  }
}
