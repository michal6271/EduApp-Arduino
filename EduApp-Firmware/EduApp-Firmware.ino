#include <ArduinoJson.h>

const int JSON_BUFFER_CAPACITY = JSON_OBJECT_SIZE(16);
const int MAX_COMMAND_SIZE = 64;
const int TIMEOUT = 100;

StaticJsonBuffer<JSON_BUFFER_CAPACITY> JSONBuffer;

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
