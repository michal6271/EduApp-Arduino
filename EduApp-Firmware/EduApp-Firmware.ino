int commandCount = 0;

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

  commandCount++;
  char* currentCommand = strtok(command, ";");
  char* commandForParamsCount = strdup(currentCommand);
  
  Serial.print("Command #");
  Serial.print(commandCount);
  Serial.print(": \"");
  Serial.print(command);
  Serial.println("\"");

  char* component = strtok(command, ":");
  char* commandIDString = strtok(NULL, ",");
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

  int componentID = strtol(componentIDString, &conversionError, 10);
  if (*conversionError) return;

  int commandID = strtol(commandIDString, &conversionError, 10);
  if (*conversionError) return;

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

