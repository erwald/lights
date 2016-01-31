/**
 * Displays the serial Input as a Soundwave
 */

import processing.serial.*;

Serial serial;
byte[] serialBuffer;

// Constants
final int BUFFER_SIZE = 640;
final int MAX_VALUE = 1024;

float[] data;
int dataCursor;
float lastX, lastY;

boolean connected = false;

void setup() {
  size(640, 360, OPENGL);
  stroke(255);

  //make serial connection
  printArray(Serial.list());
  String portName = Serial.list()[2];
  serial = new Serial(this, portName, 115200);

  serialBuffer = new byte[2];
  serial.buffer(2);

  // init values
  data = new float[BUFFER_SIZE];
  dataCursor = 0;
  
  //fill data
  for (int i=0;i<BUFFER_SIZE;i++)
    data[i] = 0;

}

void draw() {

  /*while ( serial.available() > 0) {  // If data is available,
    byte[] val = new byte[2];
    serial.readBytesUntil(255, val);
    print("1:" + val[0] + ",");
    print("2:" + val[1]);

     //add it to buffer
    //data[dataCursor] = (float)val;

    // increment cursor
    //dataCursor = (dataCursor + 1) % BUFFER_SIZE;

  }
  println("end");*/

  background(0);

  // //draw buffer
  // for (int i=1;i<BUFFER_SIZE;i++) {

  //   int last = (dataCursor + i) % BUFFER_SIZE;
  //   int next = (last + 1) % BUFFER_SIZE;

  //   line(
  //     (float)i / BUFFER_SIZE * width,
  //     height - data[last] / MAX_VALUE * height,
  //     (float)(i-1) / BUFFER_SIZE * width,
  //     height - data[next] / MAX_VALUE * height
  //   );
  // }

}
  void serialEvent(Serial myPort) {
    serialBuffer = serial.readBytes();

    print("1:" + serialBuffer[0] + ",");
    println("2:" + serialBuffer[1]);

  }