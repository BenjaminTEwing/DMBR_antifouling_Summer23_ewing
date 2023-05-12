// Motor Control with Proportional and Derivative Control
// Using an Encoder

#include <Encoder.h>

// Define motor pins
int motorPin1 = 5;
int motorPin2 = 6;

// Define encoder pins
int encoderPinA = 2;
int encoderPinB = 3;

// Define encoder object
Encoder myEncoder(encoderPinA, encoderPinB);

// Define PID variables
double kp = 1.0; // Proportional gain
double kd = 0.5; // Derivative gain
double lastError = 0;
double currentError = 0;

// Define motor speed variables
int targetSpeed = 0;
int currentSpeed = 0;

// Define loop timing variables
unsigned long lastTime = 0;
unsigned long loopTime = 10; // milliseconds

void setup() {
  // Initialize motor pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  // Set up serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Calculate elapsed time
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTime;

  // Read encoder position and calculate speed
  int encoderPosition = myEncoder.read();
  currentSpeed = encoderPosition * 1000.0 / elapsedTime; // RPM = (encoder ticks / ms) * (60 s / min) / (ticks per revolution)

  // Calculate error
  currentError = targetSpeed - currentSpeed;

  // Calculate control output
  double output = kp * currentError + kd * (currentError - lastError) / elapsedTime;

  // Update last error for next loop
  lastError = currentError;

  // Update motor speed
  int motorSpeed = abs(output);
  if (output > 0) {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  } else {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  }

  // Print debugging information
  Serial.print("Target Speed: ");
  Serial.print(targetSpeed);
  Serial.print(", Current Speed: ");
  Serial.print(currentSpeed);
  Serial.print(", Output: ");
  Serial.println(output);

  // Wait until next loop
  while (millis() - currentTime < loopTime) {
    // do nothing
  }
}
