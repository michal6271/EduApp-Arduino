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
  
  Serial.print("Component: \"");
  Serial.print(component);
  Serial.println("\"");
}

