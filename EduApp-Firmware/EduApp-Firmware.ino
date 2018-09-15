#include <ArduinoJson.h>
#include "BooleanLedComponent.cpp"
#include "PwmRgbLedComponent.cpp"

const int JSON_BUFFER_CAPACITY = JSON_OBJECT_SIZE(16);
const int MAX_COMMAND_SIZE = 256;
const int TIMEOUT = 100;

typedef enum CommandType {
  CMD_GET, CMD_SET
};

const BooleanLedComponent booleanLeds[] = {
  BooleanLedComponent(13),

  // PWM RGB LEDs as BooleanLeds for testing purposes
  BooleanLedComponent(9),
  BooleanLedComponent(10),
  BooleanLedComponent(11)
};
const int booleanLedsCount = sizeof(booleanLeds)/sizeof(BooleanLedComponent);

const PwmRgbLedComponent pwmRgbLeds[] = {
  PwmRgbLedComponent(9, 10, 11)
};
const int pwmRgbLedsCount = sizeof(pwmRgbLeds)/sizeof(PwmRgbLedComponent);

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
  const CommandType commandType = (CommandType) command.get<int>("commandType");
  const int componentId = command["componentId"];
  const int commandId = command["commandId"];
  

  JsonObject& response = JSONBuffer.createObject();
  response["commandId"] = commandId;
  response["componentName"] = componentName;
  response["componentId"] = componentId;
  response["isError"] = true;
  
  if(checkComponent(componentName, "BooleanLedComponent", componentId, booleanLedsCount)) {
    BooleanLedComponent booleanLedComponent = booleanLeds[componentId];
    response["componentAddr"] = booleanLedComponent.getAddress();
    switch(commandType) {
      case CMD_SET:
        booleanLedComponent.setState(command["data"]["state"]);
      case CMD_GET:
        JsonObject& data = JSONBuffer.createObject();
        data["state"] = booleanLedComponent.getState();
        response["data"] = data;
        response["isError"] = false;
        break;
    }
  } else if(checkComponent(componentName, "PwmRgbLedComponent", componentId, pwmRgbLedsCount)) {
    PwmRgbLedComponent pwmRgbLedComponent = pwmRgbLeds[componentId];
    response["componentAddr"] = pwmRgbLedComponent.getAddress();
    switch(commandType) {
      case CMD_SET:
        pwmRgbLedComponent.setRed(command["data"]["red"]);
        pwmRgbLedComponent.setGreen(command["data"]["green"]);
        pwmRgbLedComponent.setBlue(command["data"]["blue"]);
      case CMD_GET:
        JsonObject& data = JSONBuffer.createObject();
        data["red"] = pwmRgbLedComponent.getRed();
        data["green"] = pwmRgbLedComponent.getGreen();
        data["blue"] = pwmRgbLedComponent.getBlue();
        response["data"] = data;
        response["isError"] = false;
        break;
    }
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

