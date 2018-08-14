#include <ctype.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  processSerialData();
}

void processSerialData() {
  char temp;
  char command[64] = "";
  int charPosition = 0;
  unsigned long timeout = millis();
  while(millis() - timeout <= 100) {
    while(Serial.available()) {
      temp = Serial.read();
      timeout = millis();
      if(temp == '\r' || temp == '\n') continue;
      command[charPosition++] = temp;
    }
  }
 
  if(strlen(command) == 0 || strchr(command, ';') == NULL) return;

  char* currentCommand = strtok(command, ";");
  char* commandForParamsCount = strdup(currentCommand);
  
  Serial.print("Command: \"");
  Serial.print(command);
  Serial.println("\"");

  char* component = strtok(command, ":");
  char* commandIdString = strtok(NULL, ",");
  char* conversionError;
  int commandId = strtol(commandIdString, &conversionError, 10);
  if (*conversionError) return; 
  char* type = strtok(NULL, ",");
  char* action = strtok(NULL, ",");

  int paramsCount = 0;
  while(commandForParamsCount != NULL) {
    commandForParamsCount = strchr(commandForParamsCount, ',');
    if (commandForParamsCount) {
      commandForParamsCount++;
      paramsCount++;
    }
  }
  // First two commas separate command id and type from params
  paramsCount = paramsCount - 2;

  char* params[paramsCount];

  for (int i = 0; i < paramsCount; i++) {
    params[i] = strtok(NULL, ",");
  }
  
  Serial.print("Component: \"");
  Serial.print(component);
  Serial.print("\", command: \"");
  Serial.print(commandId);
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

