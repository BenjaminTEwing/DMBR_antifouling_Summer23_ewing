//preperation preamble
#include <SPI.h>
#include <SD.h>
#include <Wire.h> // neccesary for flow sensor & maybe SD module
#include <SoftwareSerial.h>


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//ASSIGN PINS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//wiper motor controlling
const int cleaningPWMPin 			= 9;  // Connect the motor's PWM input pin to pin 9
const int Relay_A1    			  = 38; //38H-bridge Forward
const int Relay_A2    			  = 40; //40H-bridge Forward 
const int Relay_B1    		  	= 34; //34H-bridge Reverse
const int Relay_B2    		  	= 36; //36H-bridge Reverse


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Variables 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double Pressure_reading = 0.000; // pressure reading that is used for data
String Data_output_string;

int Time_check_data = 0;

int Time_check_wipe = 0;
int fileNumber = 0;
int wipe_timer = 0;
float cleaning_voltage = 1.5; // sets the drive voltage for electrolysis mesh cleaning
float filtrant_weight = 0.0;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Constants
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
const int Take_data_interval = 10000; //10 sec interval at which data will be collected from any operable sysytem sensor

const int non_repeat_const = 200; //value of milliseconds meant to prevent accidental double writes of data

//Motor Controller parameters

const float SUPPLY_VOLTAGE = 15.63; //Measured voltage
const int VOLTAGE_LIMIT = 2; //limit the max voltage able to be sent to the motor. 
//^ kinda guessed this by slowing the bull gear by hand until 6v was commanded to the motor... it's a good gronk.

//Drive cleaning votlage
void Set_cleaning_Voltage(float VOLTS) 
{
  int CleaningPWM = 0;
  if (VOLTS < VOLTAGE_LIMIT & VOLTS > 0)
    {
    CleaningPWM = VOLTS/SUPPLY_VOLTAGE*256;
    Serial.println(String(CleaningPWM));
    digitalWrite(Relay_A1, HIGH); // 
    digitalWrite(Relay_A2, HIGH); // 
    digitalWrite(Relay_B1, LOW);  // 
    digitalWrite(Relay_B2, LOW);  // 
    analogWrite(cleaningPWMPin, CleaningPWM);
    }
  else if(VOLTS >= VOLTAGE_LIMIT)
    {
    CleaningPWM = VOLTAGE_LIMIT;
    digitalWrite(Relay_A1, HIGH);  
    digitalWrite(Relay_A2, HIGH); // 
    digitalWrite(Relay_B1, LOW);  // 
    digitalWrite(Relay_B2, LOW);  // 
    analogWrite(cleaningPWMPin, CleaningPWM);
    } 
  else 
    {
    CleaningPWM = 0;
    digitalWrite(Relay_A1, LOW); // 
    digitalWrite(Relay_A2, LOW); // 
    digitalWrite(Relay_B1, LOW); // 
    digitalWrite(Relay_B2, LOW); // 
    analogWrite(cleaningPWMPin, 0);
    }
return;
}


void setup() {
  Serial.begin(9600);      // Serial monitor baud rate
  // put your setup code here, to run once:
  //Set IO for pins in use
  pinMode(cleaningPWMPin, OUTPUT); // This is for motor
  pinMode(Relay_A1,  OUTPUT); //H-bridge Forward 
  pinMode(Relay_A2,  OUTPUT); //H-bridge Forward 
  pinMode(Relay_B1,  OUTPUT); //H-bridge Reverse
  pinMode(Relay_B2,  OUTPUT); //H-bridge Reverse

  Serial.println("Testing voltage regulation function of arduino PWM plus Low pass filter");
  Serial.println(" Switch between 1.5 volts and 0 volts every 5 seconds");
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println("set to 1.5V");
Set_cleaning_Voltage(1.5);
delay(5000);
Serial.println("set to 0V");
Set_cleaning_Voltage(0);
delay(5000);

}
