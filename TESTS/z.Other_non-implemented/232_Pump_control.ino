#include <SoftwareSerial.h>

SoftwareSerial pumpSerial(0, 1); // RX, TX pins for Serial1 interface

void setup() {
  Serial.begin(9600); // initialize serial communication
  pumpSerial.begin(9600); // initialize RS232 communication with the pump
  delay(1000); // wait for the pump to initialize
  setDirection(1); // set initial direction to forward
  setSpeed(0); // set initial speed to 0
}

void loop() {
  // do nothing in the main loop
}

void setDirection(int dir) {
  if (dir == 1) { // if the direction is forward
    pumpSerial.print("FOR"); // send the forward command
  } else if (dir == -1) { // if the direction is reverse
    pumpSerial.print("REV"); // send the reverse command
  } else {
    Serial.println("Invalid direction"); // if the direction value is invalid
    return;
  }
  delay(50); // wait for the pump to respond
  while (pumpSerial.available() > 0) { // read and discard any response from the pump
    pumpSerial.read();
  }
}

void setSpeed(int speed) {
  if (speed >= 0 && speed <= 60) { // if the speed value is within range
    pumpSerial.print("SP"); // send the speed command
    if (speed < 10) {
      pumpSerial.print("0"); // add leading zero if speed is less than 10
    }
    pumpSerial.print(speed); // send the speed value
  } else {
    Serial.println("Invalid speed"); // if the speed value is invalid
    return;
  }
  delay(50); // wait for the pump to respond
  while (pumpSerial.available() > 0) { // read and discard any response from the pump
    pumpSerial.read();
  }
}

//Here's an example Arduino program that uses the Serial1 interface to send RS232 commands for controlling the direction and speed of a MasterFlex 7523-60 pump.
//Note: This code assumes that you have connected the pump's RS232 port to the Serial1 pins on the Arduino Mega 2560.

// To use this program, you can open the Serial Monitor in the Arduino IDE, select a baud rate of 9600, and send commands in the following format:

// To set the direction to forward: setDirection(1)
// To set the direction to reverse: setDirection(-1)
// To set the speed to a value between 0 and 60: setSpeed(30)
// You can adjust the RX and TX pin numbers and baud rate to match your specific setup. Once the program is uploaded to the Arduino, you can call these functions in your main code to control the pump over RS232.

