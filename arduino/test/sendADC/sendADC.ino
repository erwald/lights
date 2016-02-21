/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */


void setup() {
  
  Serial.begin(115200);

  //handshake();
}

void loop() {

  byte data[2];

  data[0] = 111;
  data[2] = 222;
  
  Serial.write(data,2);
}

void handshake() {
   while (Serial.available() <= 0) {
      Serial.print('A'); // send a capital A
      delay(300);
   }
}
