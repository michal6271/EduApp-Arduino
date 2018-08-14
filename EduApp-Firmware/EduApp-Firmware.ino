
void setup() {
  Serial.begin(9600);
}

void loop() {
  processSerialData();
}

void processSerialData() {
  char temp;
  char command[32] = "";
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
 
  if(strlen(command) == 0) return;

  char* commandCopy = strdup(command);
  Serial.print("Command: \"");
  Serial.print(command);
  Serial.println("\"");

  char* component = strtok(command, ":");
  char* commandId = strtok(NULL, ",");
  char* type = strtok(NULL, ",");
  char* action = strtok(NULL, ",");

  char* paramsCounterTemp = commandCopy;
  int paramsCount = 0;

  while(paramsCounterTemp != NULL) {
    paramsCounterTemp = strchr(paramsCounterTemp, ',');
    if (paramsCounterTemp) {
      paramsCounterTemp++;
      paramsCount++;
    }
  }
  paramsCount = paramsCount - 2;

  Serial.print("Component: \"");
  Serial.print(component);
  Serial.print("\", command: \"");
  Serial.print(commandId);
  Serial.print("\", type: \"");
  Serial.print(type);
  Serial.print("\", action: \"");
  Serial.print(action);
  Serial.print("\", params: \"");
  Serial.print(paramsCount);
  Serial.println("\"");
}

