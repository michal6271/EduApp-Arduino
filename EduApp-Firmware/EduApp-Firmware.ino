#include <ArduinoJson.h>
#include "BooleanLedComponent.cpp"
#include "PwmRgbLedComponent.cpp"
#include "WS2812RgbLedComponent.cpp"

const int JSON_BUFFER_CAPACITY = JSON_OBJECT_SIZE(16);
const int MAX_COMMAND_SIZE = 256;
const int TIMEOUT = 100;

Adafruit_NeoPixel ws2812Driver = Adafruit_NeoPixel(5, 12, NEO_GRB + NEO_KHZ800);

BooleanLedComponent booleanLeds[] = {
  BooleanLedComponent(13),

  // PWM RGB LEDs as BooleanLeds for testing purposes
  BooleanLedComponent(9),
  BooleanLedComponent(10),
  BooleanLedComponent(11)
};
const int booleanLedsCount = sizeof(booleanLeds)/sizeof(BooleanLedComponent);

PwmRgbLedComponent pwmRgbLeds[] = {
  PwmRgbLedComponent(9, 10, 11)
};
const int pwmRgbLedsCount = sizeof(pwmRgbLeds)/sizeof(PwmRgbLedComponent);

WS2812RgbLedComponent ws2812RgbLeds[] = {
  WS2812RgbLedComponent(&ws2812Driver, 0),
  WS2812RgbLedComponent(&ws2812Driver, 1),
  WS2812RgbLedComponent(&ws2812Driver, 2),
  WS2812RgbLedComponent(&ws2812Driver, 3),
  WS2812RgbLedComponent(&ws2812Driver, 4)
};
const int ws2812RgbLedsCount = sizeof(ws2812RgbLeds)/sizeof(WS2812RgbLedComponent);

void setup() {
  Serial.begin(9600);
}

void loop() {
  StaticJsonBuffer<JSON_BUFFER_CAPACITY> JSONBuffer;
  
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
    if(root.containsKey("commandId")) {
      processJsonCommand(root);
    }
    gotCommand = false;
  }
}

void processJsonCommand(JsonObject& command) {
  StaticJsonBuffer<JSON_BUFFER_CAPACITY> JSONBuffer;
  const char* componentName = command["componentName"];
  const bool setValue = command.containsKey("data");
  const int componentId = command["componentId"];
  const int commandId = command["commandId"];
  

  JsonObject& response = JSONBuffer.createObject();
  JsonObject& responseData = JSONBuffer.createObject();
  response["commandId"] = commandId;
  response["componentName"] = componentName;
  response["componentId"] = componentId;
  response["isError"] = true;
  
  if(checkComponent(componentName, "BooleanLedComponent", componentId, booleanLedsCount)) {
    BooleanLedComponent& booleanLedComponent = booleanLeds[componentId];
    response["componentAddr"] = booleanLedComponent.getAddress();
    if(setValue) {
      booleanLedComponent.setState(command["data"]["state"]);
    }
    responseData["state"] = booleanLedComponent.getState();
    response["data"] = responseData;
    response["isError"] = false;
  } else if(checkComponent(componentName, "PwmRgbLedComponent", componentId, pwmRgbLedsCount)) {
    PwmRgbLedComponent& pwmRgbLedComponent = pwmRgbLeds[componentId];
    response["componentAddr"] = pwmRgbLedComponent.getAddress();
    if(setValue) {
      pwmRgbLedComponent.setRed(command["data"]["red"]);
      pwmRgbLedComponent.setGreen(command["data"]["green"]);
      pwmRgbLedComponent.setBlue(command["data"]["blue"]);
    }
    responseData["red"] = pwmRgbLedComponent.getRed();
    responseData["green"] = pwmRgbLedComponent.getGreen();
    responseData["blue"] = pwmRgbLedComponent.getBlue();
    response["data"] = responseData;
    response["isError"] = false;
  } else if(checkComponent(componentName, "WS2812RgbLedComponent", componentId, ws2812RgbLedsCount)) {
    WS2812RgbLedComponent& ws2812RgbLedComponent = ws2812RgbLeds[componentId];
    response["componentAddr"] = ws2812RgbLedComponent.getAddress();
    if(setValue) {
      ws2812RgbLedComponent.setRed(command["data"]["red"]);
      ws2812RgbLedComponent.setGreen(command["data"]["green"]);
      ws2812RgbLedComponent.setBlue(command["data"]["blue"]);
      ws2812RgbLedComponent.update();
    }
    responseData["red"] = ws2812RgbLedComponent.getRed();
    responseData["green"] = ws2812RgbLedComponent.getGreen();
    responseData["blue"] = ws2812RgbLedComponent.getBlue();
    response["data"] = responseData;
    response["isError"] = false;
  }
  response.prettyPrintTo(Serial);
}

bool equalComponentNames(const char * component1, const char * component2) {
  return strcmp(component1, component2) == 0;
}

bool componentInArray(int componentId, int arraySize) {
  return componentId >= 0 && componentId < arraySize;
}

bool checkComponent(const char* currentComponentName, const char* targetComponentName, int componentId, int componentArraySize) {
  return equalComponentNames(currentComponentName, targetComponentName) && componentInArray(componentId, componentArraySize);
}

