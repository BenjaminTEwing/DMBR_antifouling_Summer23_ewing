//Here's an example Arduino program that uses TSS (Terminating String Sequence) commands to control the direction and speed of a MasterFlex 7523-60 pump. This program assumes that you have connected the pump's direction control wire to digital pin 2 on the Arduino, and the speed control wire to digital pin 3

int dirPin = 2; // direction control pin
int speedPin = 3; // speed control pin

void setup() {
  Serial.begin(9600); // initialize serial communication
  pinMode(dirPin, OUTPUT); // set direction control pin as output
  pinMode(speedPin, OUTPUT); // set speed control pin as output
  digitalWrite(dirPin, LOW); // set initial direction to forward
  analogWrite(speedPin, 0); // set initial speed to 0
}

void loop() {
  if (Serial.available() > 0) { // if there's data in the serial buffer
    String command = ""; // create an empty string to hold the command
    while (Serial.available() > 0) { // read all available bytes until the TSS is received
      char c = Serial.read();
      if (c == '\r') { // if the current character is a carriage return
        break; // stop reading and exit the loop
      } else {
        command += c; // append the character to the command string
      }
    }
    command.trim(); // remove leading and trailing whitespace

    if (command.startsWith("D")) { // if the command starts with "D"
      int dir = command.substring(1).toInt(); // extract the direction value
      if (dir == 1) { // if the direction is forward
        digitalWrite(dirPin, LOW); // set direction pin to LOW
        Serial.print("D1OK\r"); // send confirmation response
      } else if (dir == -1) { // if the direction is reverse
        digitalWrite(dirPin, HIGH); // set direction pin to HIGH
        Serial.print("D-1OK\r"); // send confirmation response
      } else {
        Serial.print("DERR\r"); // send error response if the direction value is invalid
      }
    } else if (command.startsWith("S")) { // if the command starts with "S"
      int speed = command.substring(1).toInt(); // extract the speed value
      if (speed >= 0 && speed <= 999) { // if the speed value is within range
        analogWrite(speedPin, speed/4); // set the speed using PWM (divide by 4 to scale from 0-999 to 0-255)
        Serial.print("SOK\r"); // send confirmation response
      } else {
        Serial.print("SERR\r"); // send error response if the speed value is invalid
      }
    } else {
      Serial.print("ERR\r"); // send error response if the command is not recognized
    }
  }
}


To use this program, you can open the Serial Monitor in the Arduino IDE, select a baud rate of 9600, and send commands in the following format:

To set the direction to forward: D1\r
To set the direction to reverse: D-1\r
To set the speed to a value between 0 and 999: S500\r
The program will respond with one of the following TSS commands:

If the direction command is successful: D1OK\r or D-1OK\r
If the speed command is successful: SOK\r
If there's an error with the direction command: DERR\r
If there's an error with




