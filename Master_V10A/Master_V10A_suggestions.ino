//Master_V8.ino
//2023-04-05
//WN23 ME450 Group 3
//BTE

//preperation preamble
#include <SPI.h>
#include <SD.h>
#include <Wire.h> // neccesary for flow sensor & maybe SD module
#include <Encoder.h>



enum State {IDLE, WIPE, PUMP, PRESSURE, DATA, ERROR};
State state = IDLE;
struct Data {
  public:
  int time;
  int pressure;
  int pulseRate;
  int motorSpeed;
  int motorVoltage;
  int totalWipes;
};


void update()
{

  switch (state)
  {
    case IDLE:
      //do idle stuff
      break;
    case WIPE:
    if (limit_2)
      state = IDLE;
      //do wipe stuff
      break;
  }
}


class Wiper 
{
  private:
  int total_wipes;
  int motorSpeed;
  int encoderPinA;
  int encoderPinB;
  public:
  // Requires: motorSpeed, motorVoltage, totalWipes
  // Modifies:
  // Effects:
  Wiper ()
  {
    total_wipes = 0;
    encoderPinA
  }
  void wipe()
  {

    //do wiper stuff
    total_wipes++;
  }
}

Wiper foobar;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//ASSIGN PINS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
int MPX_MinusVout_pin 		= A1; // output pin for pressure transducer

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
float motor_voltage = 0;
int Time_check_data = 0;

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
Encoder myEncoder(encoderPinA, encoderPinB);
int Time_check_wipe = 0;
int fileNumber = 0;
int wipe_timer = 0;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Constants
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
const int Take_data_interval = 30000; //30 sec interval at which data will be collected from any operable sysytem sensor

const int non_repeat_const = 200; //value of milliseconds meant to prevent accidental double writes of data

const long wipe_interval = 10;  // How many data logs between wipes -  
//Motor Controller parameters
const long targetPulseRate = 1000; //SETS SPEED OF WIPER!
const float Kp = 0.005; //control ossilation 
const float Kd = 0.01; // Derivative control of motor controller - not currently in use
const float SUPPLY_VOLTAGE = 15.63; //Measured voltage
const int VOLTAGE_LIMIT = 6; //limit the max voltage able to be sent to the motor. 
//^ kinda guessed this by slowing the bull gear by hand until 6v was commanded to the motor... it's a good gronk.


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//FUNCTIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//READ_PRESSURE - MAX DIFFERENTIAL PRESSURE: 175kPa - BURST AT 200kPa
float Func_PressRead (int Vp, int Vn) // Pressure take data function declaration 
{
  int i = 0;
  double Pressure_Read_TOTAL = 0.00; 
  double Pressure_Read_NEW = 0.00; 
  double Pressure_Read_AVG = 0.00;
  //while for 10 loops to average out 10 consecutive values
  while(i < 9){
    double valuesensor1 = analogRead(Vp); //read analong and convert to voltage
    double valuesensor2 = analogRead(Vn); //read analong and convert to voltage
    valuesensor1 = valuesensor1/1024 * 5;
    valuesensor2 = valuesensor2/1024 * 5;
    //Serial.println("(analog Val sense 1 = " + String(analogRead(Vp)) + ") (analog Val sense 2 = " + String(analogRead(Vn)) + ") ");
    //Serial.println("(Voltage Val sense 1 = " + String(valuesensor1) + ") (Voltage Val sense 2 = " + String(valuesensor2) + ") ");
    double Voff = 0.0; // [mV] max value is 35 per table specs
    double Press_Sensitivity = 1.2; // delta V / delta P [mV/kPa]
    double voltage = valuesensor1 - valuesensor2;
    double Pressure_Read_NEW = (voltage - Voff) / Press_Sensitivity; // [kPa]
    // Vout = Voff + (Sensitivity x Pressure)
    Pressure_Read_TOTAL += Pressure_Read_NEW; 
    i ++; // increment counter by 1
  }// Close While
  Pressure_Read_AVG = Pressure_Read_TOTAL / 10;

return Pressure_Read_AVG;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//Drive Wiper - Forward
void wipe_forward() 
{
  bool wipe_complete = false; //inilize complete as false
  double last_Error = 0;
  double Current_Error = 0;
  int limit_2 = 0;
  while( wipe_complete == false)
    {
       currentTime = millis();
    if (currentTime - prevTime >= timeInterval) 
      { 
          currentEncoderCount = myEncoder.read();
          encoderPulseRate = abs(currentEncoderCount - prevEncoderCount) * (1000 / timeInterval);
          prevEncoderCount = currentEncoderCount;
          prevTime = currentTime;
      
          // Calculate the error and adjust motor speed
          long error = targetPulseRate - encoderPulseRate;
          motorSpeed += Kp * error;
          motorSpeed = constrain(motorSpeed, 0, 255);
          if (motorSpeed > VOLTAGE_LIMIT/SUPPLY_VOLTAGE * 255)
            {
            motorSpeed = VOLTAGE_LIMIT/SUPPLY_VOLTAGE * 255;
            } //limit max voltage to protect motor
          else
          {}  
        // Set motor speed and direction - PLEASE CHECK IF THIS LOOKS RIGHT
        digitalWrite(Relay_A1, HIGH);  // rotate backwards
        digitalWrite(Relay_A2, HIGH); // rotate backwards
        digitalWrite(Relay_B1, LOW);  // rotate backwards
        digitalWrite(Relay_B2, LOW); // rotate backwards
        analogWrite(motorPWMPin, motorSpeed);
        } //close if
    motor_voltage = motorSpeed / 255 * SUPPLY_VOLTAGE;
    //Serial.println("Drive Motor FORWARD at " + String(motor_voltage) + " VOLTS ");
    limit_2 = digitalRead(Wiper_LIMIT_SWITCH_2);
    if (limit_2 == 1)
    {
      //Serial.println("Wiper limit reached - end drive forward");
      digitalWrite(Relay_A1, LOW);  // rotate backwards
      digitalWrite(Relay_A2, LOW); // rotate backwards
      digitalWrite(Relay_B1, LOW);  // rotate backwards
      digitalWrite(Relay_B2, LOW); // rotate backwards
      analogWrite(motorPWMPin, 0);
      delay(200);
      digitalWrite(Relay_A1, LOW);  // rotate backwards
      digitalWrite(Relay_A2, LOW); // rotate backwards
      digitalWrite(Relay_B1, HIGH);  // rotate backwards
      digitalWrite(Relay_B2, HIGH); // rotate backwards
      analogWrite(motorPWMPin, 25);
      delay(200);
      Serial.println("Wiper limit reached - end drive forward");
      digitalWrite(Relay_A1, LOW);  // rotate backwards
      digitalWrite(Relay_A2, LOW); // rotate backwards
      digitalWrite(Relay_B1, LOW);  // rotate backwards
      digitalWrite(Relay_B2, LOW); // rotate backwards
      analogWrite(motorPWMPin, 0);
      wipe_complete = true;
    }
    else{}
    } //close while
return;
} // Close Function
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//Drive Wiper - Reverse
void wipe_reverse()
{
  bool wipe_complete = false; //inilize complete as false
  double last_Error = 0;
  double Current_Error = 0;
  int limit_1 = 0;
  while( wipe_complete == false)
    {
       currentTime = millis();
    if (currentTime - prevTime >= timeInterval) 
      { 
          currentEncoderCount = myEncoder.read();
          encoderPulseRate = abs(currentEncoderCount - prevEncoderCount) * (1000 / timeInterval);
          prevEncoderCount = currentEncoderCount;
          prevTime = currentTime;
      
          // Calculate the error and adjust motor speed
          long error = targetPulseRate - encoderPulseRate;
          motorSpeed += Kp * error;
          motorSpeed = constrain(motorSpeed, 0, 255);
          if (motorSpeed > VOLTAGE_LIMIT/SUPPLY_VOLTAGE * 255)
            {
            motorSpeed = VOLTAGE_LIMIT/SUPPLY_VOLTAGE * 255;
            } //limit max voltage to protect motor
          else
          {}  
    
        // Set motor speed and direction - PLEASE CHECK IF THIS LOOKS RIGHT
        digitalWrite(Relay_A1, LOW);  // rotate backwards
        digitalWrite(Relay_A2, LOW); // rotate backwards
        digitalWrite(Relay_B1, HIGH);  // rotate backwards
        digitalWrite(Relay_B2, HIGH); // rotate backwards
        analogWrite(motorPWMPin, motorSpeed);
        } //close if
    motor_voltage = motorSpeed / 255 * SUPPLY_VOLTAGE;
    //Serial.println("Drive Motor REVERSE at " + String(motor_voltage) + " VOLTS ");
    limit_1 = digitalRead(Wiper_LIMIT_SWITCH_1);
    if (limit_1 == 1)
    {
      //Serial.println("Wiper limit reached - end drive forward");
      digitalWrite(Relay_A1, LOW);  // rotate backwards
      digitalWrite(Relay_A2, LOW); // rotate backwards
      digitalWrite(Relay_B1, LOW);  // rotate backwards
      digitalWrite(Relay_B2, LOW); // rotate backwards
      analogWrite(motorPWMPin, 0);
      delay(200);
      digitalWrite(Relay_A1, HIGH);  // rotate backwards
      digitalWrite(Relay_A2, HIGH); // rotate backwards
      digitalWrite(Relay_B1, LOW);  // rotate backwards
      digitalWrite(Relay_B2, LOW); // rotate backwards
      analogWrite(motorPWMPin, 25);
      delay(200);
      Serial.println("Wiper limit reached - end drive REVERSE");
      digitalWrite(Relay_A1, LOW);  // rotate backwards
      digitalWrite(Relay_A2, LOW); // rotate backwards
      digitalWrite(Relay_B1, LOW);  // rotate backwards
      digitalWrite(Relay_B2, LOW); // rotate backwards
      analogWrite(motorPWMPin, 0);
 
      wipe_complete = true;
    }
    else{}
    } //close while
    
return;
} // Close Function
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//Read Flow Sensor
//float Read_flow
//{
//
//}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//MAIN SETUP BLOCK
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
dataFile.print("Number of wipe cycles performed,");
dataFile.println();
dataFile.close();
//Serial.println("I STG if this shit didn't write to the file again...");
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
    Pressure_reading = Func_PressRead(MPX_PlusVout_pin, MPX_MinusVout_pin);
    
    File dataFile = SD.open((String(fileNumber) + ".csv"), FILE_WRITE);
    if (dataFile)
      {
      dataFile.println(String(millis()/1000) + "," + String(Pressure_reading) + "," + String(total_wipes));
      dataFile.close();
      }
      else
      {
      Serial.println("ERROR WRITING TO FILE AT TIME (s) = " + String(millis()/1000));
      //while(1); //dont stop main loop, just make note of error
      }
    Time_check_data = millis();
    //wipe_timer ++;

  }

// if(wipe_timer == wipe_interval )
//  {
//    total_wipes ++;
//    Serial.println("Begining wipe cycle");//can be commented out to improve efficiency after program is validated
//    wipe_forward();
//    delay(3000);
//    wipe_reverse();
//    Serial.println("Wipe Cycle " + String(total_wipes) + " complete");//can be commented out to improve efficiency after program is validated
//    wipe_timer = 0;
   
//  }



}
