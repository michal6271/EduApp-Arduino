const int MAX_COMMAND_SIZE = 64;
const int TIMEOUT = 100;
const int NUMBER_BASE = 10;
const char* COMPONENT_SEPARATOR = ":";
const char* COMMAND_TOKEN_DELIMETER = ",";
const char* COMMAND_TERMINATOR = ";";

int commandCount = 0;

void setup() {
  Serial.begin(9600);
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

