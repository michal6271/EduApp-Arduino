const int PIN_BUZZER1 = 2;
const int PIN_LED1 = 13;
const int PIN_LED2 = 12;
const int PIN_RGB1_R = 9;
const int PIN_RGB1_G = 10;
const int PIN_RGB1_B = 11;
const int PIN_LED4 = 8;
const int PIN_LED5 = 7;
const int PIN_BTN1 = 4;
const int PIN_BTN2 = 5;
const int PIN_BTN3 = 3;
const int PIN_BTN4 = 6;
const int PIN_POT1 = A1;
const int PIN_TEMP1 = A2;
const int PIN_LIGHT1 = A3;

const int MAX_COMMAND_SIZE = 64;
const int MAX_RESPONSE_SIZE = 256;
const int TIMEOUT = 100;
const int NUMBER_BASE = 10;
const char* COMPONENT_SEPARATOR = ":";
const char* COMMAND_TOKEN_DELIMETER = ",";
const char* COMMAND_TERMINATOR = ";";

int commandCount = 0;

unsigned int rgbRedValue, rgbGreenValue, rgbBlueValue;

typedef struct {
  const char* name;
  int pin;
  int componentType;
} Component;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER1, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_RGB1_R, OUTPUT);
  pinMode(PIN_RGB1_G, OUTPUT);
  pinMode(PIN_RGB1_B, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);
  pinMode(PIN_LED5, OUTPUT);
  pinMode(PIN_BTN1, INPUT);
  pinMode(PIN_BTN2, INPUT);
  pinMode(PIN_BTN3, INPUT);
  pinMode(PIN_BTN4, INPUT);
  pinMode(PIN_POT1, INPUT);
  pinMode(PIN_TEMP1, INPUT);
  pinMode(PIN_LIGHT1, INPUT);
}

void loop() {
  processSerialData();
}

void processSerialData() {
  char temp;
  char command[MAX_COMMAND_SIZE] = "";
  int charPosition = 0;
  boolean gotCommand = false;
  unsigned long startTime = millis();
  
  while(millis() - startTime <= TIMEOUT) {
    while(Serial.available() && !gotCommand) {
      temp = Serial.read();
      startTime = millis();
      if(temp == '\r' || temp == '\n') continue;
      else if (temp == *COMMAND_TERMINATOR) gotCommand = true;
      command[charPosition++] = temp;
    }
  }
 
  if(strlen(command) == 0 || strchr(command, *COMMAND_TERMINATOR) == NULL) return;

  commandCount++;
  char* currentCommand = strtok(command, COMMAND_TERMINATOR);
  char* commandForParamsCount = strdup(currentCommand);
  
  Serial.print("Command #");
  Serial.print(commandCount);
  Serial.print(": \"");
  Serial.print(command);
  Serial.println("\"");

  char* component = strtok(command, COMPONENT_SEPARATOR);
  char* commandIDString = strtok(NULL, COMMAND_TOKEN_DELIMETER);
  char* type = strtok(NULL, COMMAND_TOKEN_DELIMETER);
  char* action = strtok(NULL, COMMAND_TOKEN_DELIMETER);

  int paramsCount = 0;
  while(commandForParamsCount != NULL) {
    commandForParamsCount = strchr(commandForParamsCount, *COMMAND_TOKEN_DELIMETER);
    if (commandForParamsCount) {
      commandForParamsCount++;
      paramsCount++;
    }
  }
  // First two commas separate command id and type from params
  paramsCount = paramsCount - 2;

  char* params[paramsCount];

  for (int i = 0; i < paramsCount; i++) {
    params[i] = strtok(NULL, COMMAND_TOKEN_DELIMETER);
  }

  int componentIDStart = strcspn(component, "1234567890");
  int componentLength = strlen(component);
  char componentType[componentIDStart + 1];
  char componentIDString[componentLength - componentIDStart + 1];
  // "+ 1" each, since we need space for null terminator

  strncpy(componentType, component, componentIDStart);
  componentType[componentIDStart] = '\0';

  strncpy(componentIDString, component + componentIDStart, componentLength - componentIDStart);
  componentIDString[componentLength - componentIDStart] = '\0';

  char* conversionError;

  int componentID = strtol(componentIDString, &conversionError, NUMBER_BASE);
  if (*conversionError) return;

  int commandID = strtol(commandIDString, &conversionError, NUMBER_BASE);
  if (*conversionError) return;

  

  if(strcmp(component, "arduino1") == 0) {
    if(strcmp(type, "get") == 0) {
      printGetComponentsResponse(component, commandID);
    }
  } else if(strcmp(componentType, "led") == 0) {
    int componentPin;
    switch(componentID) {
      case 1: componentPin = PIN_LED1; break;
      case 2: componentPin = PIN_LED2; break;
      case 4: componentPin = PIN_LED4; break;
      case 5: componentPin = PIN_LED5; break;
      
      default: return;
    }
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetValueResponse(component, commandID, componentPin);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, componentPin);
      } else {
        printGetValueResponse(component, commandID, componentPin);
        printGetPinResponse(component, commandID, componentPin);
      }
    } else if(strcmp(type, "set") == 0) {
      int newComponentValue = atoi(action);
      digitalWrite(componentPin, newComponentValue != 0);
      printGetValueResponse(component, commandID, componentPin);
    }
  } else if(strcmp(componentType, "rgb") == 0) {
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetRGBValueResponse(component, commandID, PIN_RGB1_R, PIN_RGB1_G, PIN_RGB1_B);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, "x");
      } else {
        printGetRGBValueResponse(component, commandID, PIN_RGB1_R, PIN_RGB1_G, PIN_RGB1_B);
        printGetPinResponse(component, commandID, "x");
      }
    } else if(strcmp(type, "set") == 0) {
      unsigned long newComponentValue = strtol(action, &conversionError, NUMBER_BASE);
      if (*conversionError) return;
      rgbRedValue = (newComponentValue >> 16) & 0xFF;
      rgbGreenValue = (newComponentValue >> 8) & 0xFF;
      rgbBlueValue = newComponentValue & 0xFF;
      analogWrite(PIN_RGB1_R, rgbRedValue);
      analogWrite(PIN_RGB1_G, rgbGreenValue);
      analogWrite(PIN_RGB1_B, rgbBlueValue);
      printGetRGBValueResponse(component, commandID, PIN_RGB1_R, PIN_RGB1_G, PIN_RGB1_B);
    }
  } else if(strcmp(componentType, "buz") == 0) {
    int componentPin;
    switch(componentID) {
      case 1: componentPin = PIN_BUZZER1; break;
      default: return;
    }
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetValueResponse(component, commandID, componentPin);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, componentPin);
      } else {
        printGetValueResponse(component, commandID, componentPin);
        printGetPinResponse(component, commandID, componentPin);
      }
    } else if(strcmp(type, "set") == 0) {
      int newComponentValue = atoi(action);
      digitalWrite(componentPin, newComponentValue != 0);
      printGetValueResponse(component, commandID, componentPin);
    }
  } else if(strcmp(componentType, "btn") == 0) {
    int componentPin;
    switch(componentID) {
      case 1: componentPin = PIN_BTN1; break;
      case 2: componentPin = PIN_BTN2; break;
      case 3: componentPin = PIN_BTN3; break;
      case 4: componentPin = PIN_BTN4; break;
      default: return;
    }
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetValueResponse(component, commandID, componentPin);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, componentPin);
      } else {
        printGetValueResponse(component, commandID, componentPin);
        printGetPinResponse(component, commandID, componentPin);
      }
    }
  } else if(strcmp(componentType, "temp") == 0) {
    int componentPin;
    switch(componentID) {
      case 1: componentPin = PIN_TEMP1; break;
      default: return;
    }
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetAnalogValueResponse(component, commandID, componentPin);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, componentPin);
      } else {
        printGetAnalogValueResponse(component, commandID, componentPin);
        printGetPinResponse(component, commandID, componentPin);
      }
    }
  } else if(strcmp(componentType, "light") == 0) {
    int componentPin;
    switch(componentID) {
      case 1: componentPin = PIN_LIGHT1; break;
      default: return;
    }
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetAnalogValueResponse(component, commandID, componentPin);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, componentPin);
      } else {
        printGetAnalogValueResponse(component, commandID, componentPin);
        printGetPinResponse(component, commandID, componentPin);
      }
    }
  } else if(strcmp(componentType, "pot") == 0) {
    int componentPin;
    switch(componentID) {
      case 1: componentPin = PIN_POT1; break;
      default: return;
    }
    if(strcmp(type, "get") == 0) {
      if(strcmp(action, "value") == 0) {
        printGetAnalogValueResponse(component, commandID, componentPin);
      } else if(strcmp(action, "pin") == 0) {
        printGetPinResponse(component, commandID, componentPin);
      } else {
        printGetAnalogValueResponse(component, commandID, componentPin);
        printGetPinResponse(component, commandID, componentPin);
      }
    }
  }
  
  Serial.print("Component: \"");
  Serial.print(component);
  Serial.print("\", component type: \"");
  Serial.print(componentType);
  Serial.print("\", component ID: \"");
  Serial.print(componentID);
  Serial.print("\", command: \"");
  Serial.print(commandID);
  Serial.print("\", type: \"");
  Serial.print(type);
  Serial.print("\", action: \"");
  Serial.print(action);
  Serial.print("\", params[");
  Serial.print(paramsCount);
  Serial.print("]: {");
  for (int i = 0; i < paramsCount; i++) {
    Serial.print("\"");
    Serial.print(params[i]);
    Serial.print("\", ");
  }
  Serial.println("}");
}

void printGetComponentsResponse(char* component, int commandID) {
  char response[MAX_RESPONSE_SIZE] = "";
  snprintf(response, MAX_RESPONSE_SIZE, "response:%s,%d,led1,led2,led4,led5,rgb1,buz1,btn1,btn2,btn3,btn4,pot1,temp1,light1;", component, commandID);
  Serial.println(response);
}

void printGetValueResponse(char* component, int commandID, int pin) {
  char response[MAX_RESPONSE_SIZE] = "";
  int componentValue = digitalRead(pin);
  snprintf(response, MAX_RESPONSE_SIZE, "response:%s,%d,raw,%d;", component, commandID, componentValue);
  Serial.println(response);
}

void printGetAnalogValueResponse(char* component, int commandID, int pin) {
  char response[MAX_RESPONSE_SIZE] = "";
  int componentValue = analogRead(pin);
  snprintf(response, MAX_RESPONSE_SIZE, "response:%s,%d,raw,%d;", component, commandID, componentValue);
  Serial.println(response);
}

void printGetRGBValueResponse(char* component, int commandID, int pinR, int pinG, int pinB) {
  char response[MAX_RESPONSE_SIZE] = "";
  unsigned long componentValue = (rgbRedValue << 16) | (rgbGreenValue << 8) | rgbBlueValue;
  snprintf(response, MAX_RESPONSE_SIZE, "response:%s,%d,raw,%d;", component, commandID, componentValue);
  Serial.println(response);
}

void printGetPinResponse(char* component, int commandID, int pin) {
  char response[MAX_RESPONSE_SIZE] = "";
  snprintf(response, MAX_RESPONSE_SIZE, "response:%s,%d,pin,%d;", component, commandID, pin);
  Serial.println(response);
}

void printGetPinResponse(char* component, int commandID, char* pin) {
  char response[MAX_RESPONSE_SIZE] = "";
  snprintf(response, MAX_RESPONSE_SIZE, "response:%s,%d,pin,%s;", component, commandID, pin);
  Serial.println(response);
}

