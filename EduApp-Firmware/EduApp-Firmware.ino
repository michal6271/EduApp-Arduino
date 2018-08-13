void setup() {
  Serial.begin(9600);
}

void loop() {
  processSerialData();
}

void processSerialData() {
  char temp;
  char command[32] = "";
  int char_pos = 0;
  unsigned long timeout = millis();
  while(millis() - timeout <= 100) {
    while(Serial.available()) {
      temp = Serial.read();
      timeout = millis();
      if(temp == '\r' || temp == '\n') continue;
      command[char_pos++] = temp;
    }
  }
 
  if(strlen(command) == 0) return;

  Serial.print("Command: \"");
  Serial.print(command);
  Serial.println("\"");

  char *component = strtok(command, ":");
  char *command_id = strtok(NULL, ",");
  char *type = strtok(NULL, ",");
  char *action = strtok(NULL, ",");

  Serial.print("Component: \"");
  Serial.print(component);
  Serial.print("\", command: \"");
  Serial.print(command_id);
  Serial.print("\", type: \"");
  Serial.print(type);
  Serial.print("\", action: \"");
  Serial.print(action);
  Serial.println("\"");
}

