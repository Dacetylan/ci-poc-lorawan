#include <SoftwareSerial.h>

SoftwareSerial mySerial(4, 5); // RX, TX

void setup() {
  Serial.begin(9600);
  mySerial.begin(57600);
}

void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
