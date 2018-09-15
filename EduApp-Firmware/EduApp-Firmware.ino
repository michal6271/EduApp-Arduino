#include <ArduinoJson.h>
#include "BooleanLedComponent.cpp"
#include "PwmRgbLedComponent.cpp"
#include "WS2812RgbLedComponent.cpp"
#include "ButtonComponent.cpp"
#include "BuzzerComponent.cpp"
#include "AnalogInputComponent.cpp"
#include "ThermometerComponent.cpp"
#include "SevenSegComponent.cpp"
#include "Lcd1602Component.cpp"

#define JSON_BUFFER_CAPACITY  JSON_OBJECT_SIZE(10)
#define MAX_COMMAND_SIZE      200
#define TIMEOUT               100

Adafruit_NeoPixel ws2812Driver = Adafruit_NeoPixel(5, 12, NEO_GRB + NEO_KHZ800);

BooleanLedComponent booleanLeds[] = {
  BooleanLedComponent(13)
};
const uint8_t booleanLedsCount = sizeof(booleanLeds)/sizeof(BooleanLedComponent);

PwmRgbLedComponent pwmRgbLeds[] = {
  PwmRgbLedComponent(9, 10, 11)
};
const uint8_t pwmRgbLedsCount = sizeof(pwmRgbLeds)/sizeof(PwmRgbLedComponent);

WS2812RgbLedComponent ws2812RgbLeds[] = {
  WS2812RgbLedComponent(&ws2812Driver, 0),
  WS2812RgbLedComponent(&ws2812Driver, 1),
  WS2812RgbLedComponent(&ws2812Driver, 2),
  WS2812RgbLedComponent(&ws2812Driver, 3),
  WS2812RgbLedComponent(&ws2812Driver, 4)
};
const uint8_t ws2812RgbLedsCount = sizeof(ws2812RgbLeds)/sizeof(WS2812RgbLedComponent);

ButtonComponent buttons[] = {
  ButtonComponent(4),
  ButtonComponent(5),
  ButtonComponent(6),
  ButtonComponent(7),
  ButtonComponent(8)
};
const uint8_t buttonsCount = sizeof(buttons)/sizeof(ButtonComponent);

BuzzerComponent buzzers[] = {
  BuzzerComponent(2)
};
const uint8_t buzzersCount = sizeof(buzzers)/sizeof(BuzzerComponent);

AnalogInputComponent potentiometers[] = {
  AnalogInputComponent(A1, "PotentiometerComponent")
};
const uint8_t potentiometersCount = sizeof(potentiometers)/sizeof(AnalogInputComponent);

ThermometerComponent thermometers[] = {
  ThermometerComponent(A2)
};
const uint8_t thermometersCount = sizeof(thermometers)/sizeof(ThermometerComponent);

AnalogInputComponent microphones[] = {
  AnalogInputComponent(A0, "MicrophoneComponent")
};
const uint8_t microphonesCount = sizeof(microphones)/sizeof(AnalogInputComponent);

AnalogInputComponent lightSensors[] = {
  AnalogInputComponent(A3, "LightSensorComponent")
};
const uint8_t lightSensorsCount = sizeof(lightSensors)/sizeof(AnalogInputComponent);

SevenSegComponent sevenSegs[] = {
  SevenSegComponent(0x24)
};
const uint8_t sevenSegsCount = sizeof(sevenSegs)/sizeof(SevenSegComponent);

Lcd1602Component lcds1602[] = {
  Lcd1602Component(0x20)
};
const uint8_t lcds1602Count = sizeof(lcds1602)/sizeof(Lcd1602Component);

void setup() {
  Serial.begin(9600);
  for(int x = 0; x < ws2812RgbLedsCount; x++) {
    ws2812RgbLeds[x].setRed(0);
    ws2812RgbLeds[x].setGreen(0);
    ws2812RgbLeds[x].setBlue(0);
    ws2812RgbLeds[x].update();
  }
  for(int x = 0; x < sevenSegsCount; x++) {
    sevenSegs[x].begin();
  }
  for(int x = 0; x < lcds1602Count; x++) {
    lcds1602[x].begin();
  }
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
  if(componentName == NULL) {
    Serial.println(F("{\"commandId\":0,\"componentName\": null,\"data\": \"BooleanLed-1,PwmRgbLed-1,WS2812RgbLed-5,Button-5,Buzzer-1,Thermometer-1,Potentiometer-1,Microphone-1,LightSensor-1,SevenSeg-1,Lcd1602-1\"}"));
    return; 
  }
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
  } else if(checkComponent(componentName, "SevenSegComponent", componentId, sevenSegsCount)) {
    SevenSegComponent& sevenSegComponent = sevenSegs[componentId];
    response["componentAddr"] = sevenSegComponent.getAddress();
    if(setValue) {
      sevenSegComponent.setSegments(command["data"]["segments"]);
    }
    responseData["segments"] = sevenSegComponent.getSegments();
    response["data"] = responseData;
    response["isError"] = false;
  } else if(checkComponent(componentName, "Lcd1602Component", componentId, lcds1602Count)) {
    Lcd1602Component& lcd1602Component = lcds1602[componentId];
    response["componentAddr"] = lcd1602Component.getAddress();
    if(setValue) {
      lcd1602Component.setText(command["data"]["text0"]);
    }
    response["isError"] = false;
  } else if(checkComponent(componentName, "BuzzerComponent", componentId, buzzersCount)) {
    BuzzerComponent& buzzerComponent = buzzers[componentId];
    response["componentAddr"] = buzzerComponent.getAddress();
    if(setValue) {
      buzzerComponent.setState(command["data"]["state"]);
    }
    responseData["state"] = buzzerComponent.getState();
    response["data"] = responseData;
    response["isError"] = false;
  } else if(checkComponent(componentName, "PotentiometerComponent", componentId, potentiometersCount)) {
    AnalogInputComponent& analogInputComponent = potentiometers[componentId];
    response["componentAddr"] = analogInputComponent.getAddress();
    if(!setValue) {
      responseData["value"] = analogInputComponent.getValue();
      response["data"] = responseData;
      response["isError"] = false;
    }
  } else if(checkComponent(componentName, "ThermometerComponent", componentId, thermometersCount)) {
    ThermometerComponent& thermometerComponent = thermometers[componentId];
    response["componentAddr"] = thermometerComponent.getAddress();
    if(!setValue) {
      responseData["value"] = thermometerComponent.getValue();
      responseData["valueCelsius"] = thermometerComponent.getConvertedValueCelsius();
      response["data"] = responseData;
      response["isError"] = false;
    }
  } else if(checkComponent(componentName, "MicrophoneComponent", componentId, microphonesCount)) {
    AnalogInputComponent& analogInputComponent = microphones[componentId];
    response["componentAddr"] = analogInputComponent.getAddress();
    if(!setValue) {
      responseData["value"] = analogInputComponent.getValue();
      response["data"] = responseData;
      response["isError"] = false;
    }
  } else if(checkComponent(componentName, "LightSensorComponent", componentId, lightSensorsCount)) {
    AnalogInputComponent& analogInputComponent = lightSensors[componentId];
    response["componentAddr"] = analogInputComponent.getAddress();
    if(!setValue) {
      responseData["value"] = analogInputComponent.getValue();
      response["data"] = responseData;
      response["isError"] = false;
    }
  } else if(checkComponent(componentName, "ButtonComponent", componentId, buttonsCount)) {
    ButtonComponent& buttonComponent = buttons[componentId];
    response["componentAddr"] = buttonComponent.getAddress();
    if(!setValue) {
      responseData["state"] = buttonComponent.getState();
      response["data"] = responseData;
      response["isError"] = false;
    }
  }
  response.printTo(Serial);
  Serial.println();
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

