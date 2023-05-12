//Master_V8.ino
//2023-04-05
//WN23 ME450 Group 3
//BTE

//preperation preamble
#include <SPI.h>
#include <SD.h>
#include <Wire.h> // neccesary for flow sensor & maybe SD module
#include <Encoder.h>

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//ASSIGN PINS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//wiper motor controlling
const int motorPWMPin 			= 9;  // Connect the motor's PWM input pin to pin 9
const int encoderPinA 			= 2;  // Connect the encoder's A output pin to pin 2
const int encoderPinB 			= 3;  // Connect the encoder's B output pin to pin 3
const int Relay_A1    			= 34; //H-bridge Forward
const int Relay_A2    			= 36; //H-bridge Forward 
const int Relay_B1    			= 38; //H-bridge Reverse
const int Relay_B2    			= 40; //H-bridge Reverse
const int Wiper_LIMIT_SWITCH_1 	= 24; //Define the bounds of the wiper travel
const int Wiper_LIMIT_SWITCH_2 	= 25; //Define the bounds of the wiper travel

//pressure transducer
int MPX_PlusVout_pin 			= A0;// output pin for pressure transducer
int MPX_MinusVout_pin 			= A1; // output pin for pressure transducer

//Pump Control
const int Pump_serial_TX 		= 1;
const int Pump_serial_RX 		= 0;

//SD Card Module
const int chipSelect  			= 53;
File myFile;



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Variables 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double Pressure_reading = 0.0; // pressure reading that is used for data
String Data_output_string;
float motor_voltage = 0;

//Variables for controlling the motor and encoder
long prevEncoderCount = 0;
long currentEncoderCount = 0;
long encoderPulseRate = 0;
float motorSpeed = 0;
int total_wipes = 0;
	//Time variables for motor/encoder
unsigned long prevTime = 0;
unsigned long currentTime = 0;
unsigned long timeInterval = 100; // Time interval for pulse rate calculation (ms)
long wipe_interval = 300000;  //How long we want to wait between wipes (in milliseconds) (5 minutes right now)
//Maybe remove with a change in program logic
bool Time_to_wipe = false; //Boolean that determines when the code should control motor speed on initial wipe
bool Time_to_reverse = false; //Boolean that determines when the code should control motor speed on the wipe back
Encoder myEncoder(encoderPinA, encoderPinB);




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Constants
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
const int Take_data_interval = 1000;
const long targetPulseRate = 1000; //SETS SPEED OF WIPER!
const float Kp = 0.005; //control ossilation 
const float SUPPLY_VOLTAGE = 15.63;





//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//FUNCTIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//READ_PRESSURE - MAX DIFFERENTIAL PRESSURE: 175kPa - BURST AT 200kPa
float Func_PressRead (int Vp, int Vn) // Pressure take data function declaration 
{
  double valuesensor1 = analogRead(Vp); //read analong and convert to voltage
  double valuesensor2 = analogRead(Vn); //read analong and convert to voltage
  valuesensor1 = valuesensor1/1024 * 5;
  valuesensor2 = valuesensor2/1024 * 5;
  //Serial.println("(analog Val sense 1 = " + String(analogRead(Vp)) + ") (analog Val sense 2 = " + String(analogRead(Vn)) + ") ");
  //Serial.println("(Voltage Val sense 1 = " + String(valuesensor1) + ") (Voltage Val sense 2 = " + String(valuesensor2) + ") ");
  double Voff = 0.0; // [mV] max value is 35 per table specs
  double Press_Sensitivity = 1.2; // delta V / delta P [mV/kPa]
  double voltage = valuesensor1 - valuesensor2;
  double Pressure_Read = (voltage - Voff) / Press_Sensitivity; // [kPa]
  // Vout = Voff + (Sensitivity x Pressure)
return Pressure_Read;
}


//Drive Wiper - Forward


//Drive Wiper - Reverse


//Read Flow Sensor



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//MAIN SETUP BLOCK
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void setup()
{
//Set IO for pins in use
pinMode(motorPWMPin, OUTPUT); // This is for motor
pinMode(Relay_A1,  OUTPUT); //H-bridge Forward 
pinMode(Relay_A2,  OUTPUT); //H-bridge Forward 
pinMode(Relay_B1,  OUTPUT); //H-bridge Reverse
pinMode(Relay_B2,  OUTPUT); //H-bridge Reverse
pinMode(Wiper_LIMIT_SWITCH_1,    INPUT);  //Limit Switch 1
pinMode(Wiper_LIMIT_SWITCH_2,    INPUT);  //Limit Switch 2 

pinMode(MPX_PlusVout_pin, INPUT_PULLUP);
pinMode(MPX_MinusVout_pin, INPUT_PULLUP);

pinMode(chipSelect, INPUT);

Serial.begin(9600);
Serial.println();
// see if the card is present and can be initialized:
  if (SD.begin(chipSelect) != 1) 
  {
    Serial.println("Card failed, or not present - shit");
    // don't do anything more:
    while (1);
  }

  Serial.println("card initialized.");
  Serial.println("End setup");
  Serial.println("Begin Loop")
  Serial.println("Take Data Interval (seconds)= " + String(Take_data_interval/1000) + " seconds  Wiper speed = " + String(targetPulseRate) + " Wiper Interval(minutes) = " + String(wipe_interval/1000/60));
// --------- End of Initialization of SD card ------------


// Write Preamble block onto data output file on SD Card - NEED TO CHECK FUNCTION BEFORE VALIDATING LOOP
File dataFile = SD.open("datalog.csv", FILE_WRITE);
dataFile.println("TESTING OF DMBR FILTRATION WITH FOULING MITIGATION");
dataFile.println("Program and mechanical strucures designed by the WN2023 ME450 Group 3 Project team");
dataFile.println("Take Data Interval (seconds)= " + String(Take_data_interval/1000) + " seconds  Wiper speed = " + String(targetPulseRate) + " Wiper Interval(minutes) = " + String(wipe_interval/1000/60));
dataFile.println("TIME" + "," + "Pressure (Vacuum)" + "," + "Number of wipe cycles performed");


//Set initial pump speed and directions - (SPEED), Forward 
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN LOOP BLOCK
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop()
{
//Compare time to take data interval - and collect if time
//  If time % inteval == 0 && time from last collect > 20 (non-repeat Value)
//    Collect pressure and write: Time, Pressure, and wipe cycles to file
//  Else
//     Do nothing

// Check for wipe time and if time, perform wipe function using motor control and limit switches
//  If time % inteval == 0 && time from last collect > 20 (non-repeat Value) 
//    Write to serial Monitor
//    Call Drive Wiper forward function
//    Wait for check (while loop)
//    Write to serial Monitor
//    Call Drive motor Backwards
//    Wait for check (while loop)
//    Increment Whip counter once wiping complete.
//    Write to serial Monitor  

//  Else
//    Do nothing



	
}
