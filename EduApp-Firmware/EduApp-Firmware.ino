void setup() {
  Serial.begin(9600);
}

void loop() {
  processSerialData();
}

void processSerialData() {
  char temp;
  String data = "";
  unsigned long timeout = millis();
  while(millis() - timeout <= 100) {
    while(Serial.available()) {
      temp = Serial.read();
      timeout = millis();
      if(temp == '\r' || temp == '\n') continue;
      data += temp;
    }
  }
 
  if(data.length() == 0) return;
 
  data.toUpperCase();
 
  Serial.print("GOT: \"");
  Serial.print(data);
  Serial.println("\"");
}

