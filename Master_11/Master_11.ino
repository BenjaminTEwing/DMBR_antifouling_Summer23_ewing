//Master_V11.ino
//2023-05-18
//
//BTE

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
const int Relay_A1    			  = 34; //H-bridge Forward
const int Relay_A2    			  = 36; //H-bridge Forward 
const int Relay_B1    		  	= 38; //H-bridge Reverse
const int Relay_B2    		  	= 40; //H-bridge Reverse

//pressure transducer
int MPX_PlusVout_pin 			    = A0;// output pin for pressure transducer
int MPX_MinusVout_pin 		    = A1; // output pin for pressure transducer

//Scale Serial reading and control
SoftwareSerial scaleSerial(10, 11);  // RX, TX pins PICK NEW FOR MEGA

//Pump Control
const int Pump_serial_TX 		= 1;
const int Pump_serial_RX 		= 0;

//SD Card Module
const int chipSelect  			= 53;
File myFile;


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


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//FUNCTIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//READ_PRESSURE - MAX DIFFERENTIAL PRESSURE: 175kPa - BURST AT 200kPa
float Func_PressRead (int Vp) // Pressure take data function declaration 
{
  int i = 0;
  float Pressure_Read_TOTAL = 0.0; 
  float Pressure_Read_NEW = 0.0; 
  float Pressure_Read_AVG = 0.00;
  //while for 10 loops to average out 10 consecutive values
  while(i < 9)
  {
    double valuesensor1 = analogRead(Vp); //read analong and convert to voltage
    double voltage = valuesensor1/1024 * 5;
    double Voff = 0.0; // [mV] max value is 35 per table specs
    double Press_Sensitivity = 1; // delta V / delta P [mV/kPa] NEED TO CALIBRATE
    double Pressure_Read_NEW = (voltage - 0.65); // [volts]
    // Vout = Voff + (Sensitivity x Pressure)
    Pressure_Read_TOTAL += Pressure_Read_NEW; 
    i ++; // increment counter by 1
  }// Close While
  Pressure_Read_AVG = Pressure_Read_TOTAL / 10;

return Pressure_Read_AVG;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//Drive cleaning votlage
void Set_cleaning_Voltage(float VOLTS) 
{
  int CleaningPWM = 0
  if (VOLTS < VOLTAGE_LIMIT & VOLTS > 0)
    {
    CleaningPWM = VOLTS/1024;
    digitalWrite(Relay_A1, HIGH); // 
    digitalWrite(Relay_A2, HIGH); // 
    digitalWrite(Relay_B1, LOW);  // 
    digitalWrite(Relay_B2, LOW);  // 
    analogWrite(cleaningPWMPin, CleaningPWM);
    }
  if(VOLTS >= VOLTAGE_LIMIT)
    {
    CleaningPWM = VOLTAGE_LIMIT
    digitalWrite(Relay_A1, HIGH);  
    digitalWrite(Relay_A2, HIGH); // 
    digitalWrite(Relay_B1, LOW);  // 
    digitalWrite(Relay_B2, LOW);  // 
    analogWrite(cleaningPWMPin, CleaningPWM);
    } 
  else 
    {
    CleaningPWM = 0
    digitalWrite(Relay_A1, LOW); // 
    digitalWrite(Relay_A2, LOW); // 
    digitalWrite(Relay_B1, LOW); // 
    digitalWrite(Relay_B2, LOW); // 
    analogWrite(cleaningPWMPin, 0);
    }
return;
}
// Close Function
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// Read weight function
string Read_Weight()
{

  scaleSerial.print("IP\r");
  if scaleSerial.available()
  {
    data = scaleSerial.readStringUntil("\n");
  }

  //Clean data of misc info

  return data;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//MAIN SETUP BLOCK
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup()
{
//Set IO for pins in use
pinMode(cleaningPWMPin, OUTPUT); // This is for motor
pinMode(Relay_A1,  OUTPUT); //H-bridge Forward 
pinMode(Relay_A2,  OUTPUT); //H-bridge Forward 
pinMode(Relay_B1,  OUTPUT); //H-bridge Reverse
pinMode(Relay_B2,  OUTPUT); //H-bridge Reverse

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

//FIND NEXT AVAILABLE FILENAME/NUMBER
  while (SD.exists(String(fileNumber) + ".csv")) {
    fileNumber++;
  } // Close while

 File dataFile = SD.open((String(fileNumber) + ".csv"), FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error creating new file at time (s) =" + String(millis()/1000));
    return;
    } //close if
  Serial.println("Created new file: ");
  Serial.println(String(fileNumber) + ".csv" );
//END FILE PREPERATION

  Serial.println("Take Data Interval (seconds)= " + String(Take_data_interval/1000) + " seconds  Wiper speed = " + String(targetPulseRate) + " Wiper Interval(seconds) = " + String(Take_data_interval / 1000 * wipe_interval));
// --------- End of Initialization of SD card ------------

// Write Preamble block onto data output file on SD Card - NEED TO CHECK FUNCTION BEFORE VALIDATING LOOP
//File dataFile = SD.open("datalog.csv", FILE_WRITE); ACTION REPLACED BE INCREMENTING NAMING LINES

if (dataFile){
 
dataFile.println("TESTING OF DMBR FILTRATION WITH FOULING MITIGATION ,");
dataFile.println("Program and mechanical strucures designed by the WN2023 ME450 Group 3 Project team ,");
//Test specific parameters
dataFile.println("Take Data Interval (seconds)= ," + String(Take_data_interval/1000) + ",Wiper speed (pulse rate) = ," + String(targetPulseRate) + ",Wiper Interval(seconds) = ," + String(Take_data_interval / 1000 * wipe_interval) + ",");
//The format the output data will follow in the columns after the loop
dataFile.print("TIME,"); 
//dataFile.print(",");
dataFile.print("Pressure (Vacuum),"); 
//dataFile.print(",");
dataFile.print("Filtrant Weight");
dataFile.println();
dataFile.close();
}
else
{
  Serial.println("ERROR WRITING TO FILE");
  while(1);
}
//Set initial pump speed and directions - (SPEED), Forward 
Serial.println("Calibrating Wiper - SKIPPED");

//wipe_forward();
//Serial.println("Waiting 3 seconds");
//delay(3000);
//wipe_reverse();
//Serial.println("Calibration complete, begining Testing");
}//close setup



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN LOOP BLOCK
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop()
{
 
  if(((millis() % Take_data_interval) == 0) &  (millis() - Time_check_data)  >  non_repeat_const)
    {
    Serial.println("Taking measurment at time = " + String(millis()/1000) + " Seconds"); //can be commented out to improve efficiency after program is validated
    Pressure_reading = Func_PressRead(MPX_PlusVout_pin);
    filtrant_weight = Read_Weight();
    
    File dataFile = SD.open((String(fileNumber) + ".csv"), FILE_WRITE);
    if (dataFile)
      {
      dataFile.println(String(millis()/1000) + "," + String(Pressure_reading) + "," + String(total_wipes));
      dataFile.close();
      } //close if
    else
      {
      Serial.println("ERROR WRITING TO FILE AT TIME (s) = " + String(millis()/1000));
      //while(1); //dont stop main loop, just make note of error
      } //close else

    Time_check_data = millis();
    }
  if millis() > 3600000 & millis < 7200000 // use electrolysis 
    {
      Set_cleaning_Voltage(cleaning_voltage);
    }//close if
  else
    {
      Set_cleaning_Voltage(0);

    }//close else




}// Close LOOP