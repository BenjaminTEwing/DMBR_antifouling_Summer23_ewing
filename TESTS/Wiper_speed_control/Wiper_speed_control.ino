//wiper_speed_control.ino
#include <Encoder.h> //will need to install this library on whatever computer we use//Pin Assignments


const int motorPWMPin 			= 9; // Connect the motor's PWM input pin to pin 9
const int encoderPinA 			= 2; // Connect the encoder's A output pin to pin 2
const int encoderPinB 			= 3; // Connect the encoder's B output pin to pin 3
const int Relay_A1    			= 34; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Relay_A2    			= 36; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Relay_B1    			= 38; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Relay_B2    			= 40; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Wiper_LIMIT_SWITCH_1 	= 24; // Connected to limit switch *MAY NEED TO SWITCH*
const int Wiper_LIMIT_SWITCH_2 	= 25; // Connected to limit switch *MAY NEED TO SWITCH*


//variables
int Take_data_interval = 500;
String Data_output_string;
float motor_voltage = 0;

const float SUPPLY_VOLTAGE = 15.63;

//Motor and Encoder Setup
//Target encoder pulses per second
const long targetPulseRate = 1000; //SETS SPEED OF WIPER!
//Proportional gain constant
const float Kp = 0.005; //control ossilation 
//Encoder setup
Encoder myEncoder(encoderPinA, encoderPinB);
//Variables for controlling the motor and encoder
long prevEncoderCount = 0;
long currentEncoderCount = 0;
long encoderPulseRate = 0;
float motorSpeed = 0;
int total_wipes = 0; //total number of wipes performed by system
//Time variables for motor/encoder
unsigned long prevTime = 0;
unsigned long currentTime = 0;
unsigned long timeInterval = 100; // Time interval for pulse rate calculation (ms)
long wipe_interval = 300000;  //How long we want to wait between wipes (in milliseconds) (5 minutes right now)
bool Time_to_wipe = false; //Boolean that determines when the code should control motor speed on initial wipe
bool Time_to_reverse = false; //Boolean that determines when the code should control motor speed on the wipe back

void setup(){
Serial.begin(9600);
pinMode(motorPWMPin, OUTPUT); // This is for motor
pinMode(Relay_A1,  OUTPUT); //H Bridge connections to control direction
pinMode(Relay_A2,  OUTPUT);
pinMode(Relay_B1,  OUTPUT);
pinMode(Relay_B2,  OUTPUT);
pinMode(Wiper_LIMIT_SWITCH_1,    INPUT);  //Limit Switch 1
pinMode(Wiper_LIMIT_SWITCH_2,    INPUT);  //Limit Switch 2 
Serial.println();
Serial.println("Begining test of Motor speed Control");
}

void loop(){
  
currentTime = millis();

if(digitalRead(Wiper_LIMIT_SWITCH_1) == HIGH 	&&	 digitalRead(Wiper_LIMIT_SWITCH_2) != HIGH)
{
    
	//while(digitalRead(Wiper_LIMIT_SWITCH_1) == HIGH && digitalRead(Wiper_LIMIT_SWITCH_2) != HIGH){
		if (currentTime - prevTime >= timeInterval) { //NEED TO CHANGE THIS BIT OF CODE SO THAT IT IS IN REVERSE DIRECTION
	      currentEncoderCount = myEncoder.read();
	      encoderPulseRate = abs(currentEncoderCount - prevEncoderCount) * (1000 / timeInterval);
	      prevEncoderCount = currentEncoderCount;
	      prevTime = currentTime;
	  
	      // Calculate the error and adjust motor speed
	      long error = targetPulseRate - encoderPulseRate;
	      motorSpeed += Kp * error;
	      motorSpeed = constrain(motorSpeed, 0, 255);
  
      // Set motor speed and direction - PLEASE CHECK IF THIS LOOKS RIGHT
	   	digitalWrite(Relay_A1, HIGH);  // rotate backwards
	   	digitalWrite(Relay_A2, HIGH); // rotate backwards
	   	digitalWrite(Relay_B1, LOW);  // rotate backwards
	   	digitalWrite(Relay_B2, LOW); // rotate backwards
	  	analogWrite(motorPWMPin, motorSpeed);
 		} //close if
 		motor_voltage = motorSpeed / 255 * SUPPLY_VOLTAGE;
 		Serial.println("Drive Motor REVERSE at " + String(motor_voltage) + " VOLTS ");
 	//}//close While
} //close if


else if(digitalRead(Wiper_LIMIT_SWITCH_2) == HIGH 	&&	 digitalRead(Wiper_LIMIT_SWITCH_1) != HIGH)
{ 
	//while(digitalRead(Wiper_LIMIT_SWITCH_2) == HIGH && digitalRead(Wiper_LIMIT_SWITCH_1) != HIGH){
		if (currentTime - prevTime >= timeInterval) { //NEED TO CHANGE THIS BIT OF CODE SO THAT IT IS IN REVERSE DIRECTION
	      currentEncoderCount = myEncoder.read();
	      encoderPulseRate = abs(currentEncoderCount - prevEncoderCount) * (1000 / timeInterval);
	      prevEncoderCount = currentEncoderCount;
	      prevTime = currentTime;
	  
	      // Calculate the error and adjust motor speed
	      long error = targetPulseRate - encoderPulseRate;
	      motorSpeed += Kp * error;
	      motorSpeed = constrain(motorSpeed, 0, 255);
  
      // Set motor speed and direction - PLEASE CHECK IF THIS LOOKS RIGHT
	   	digitalWrite(Relay_A1, LOW);  // rotate backwards
	   	digitalWrite(Relay_A2, LOW); // rotate backwards
	   	digitalWrite(Relay_B1, HIGH);  // rotate backwards
	   	digitalWrite(Relay_B2, HIGH); // rotate backwards
	  	analogWrite(motorPWMPin, motorSpeed);
 		} //close if
 		motor_voltage = motorSpeed / 255 * SUPPLY_VOLTAGE;
 		Serial.println("Drive Motor REVERSE at " + String(motor_voltage) + " VOLTS ");
 	//}//close While
} //close elseif
 else{
  digitalWrite(Relay_A1, LOW);// STOP
  digitalWrite(Relay_A1, LOW);// STOP
  digitalWrite(Relay_B1, LOW);// STOP
  digitalWrite(Relay_B2, LOW);// STOP
  analogWrite(motorSpeed, 0);
  Serial.println("STOP MOTOR");
}


}
