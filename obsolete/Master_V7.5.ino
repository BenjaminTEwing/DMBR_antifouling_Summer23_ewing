/**
 * 
 * TO DO:
 * 
 * ASAP
 * - Initialize pins to switch direcion forward/backward motor control
 * - Initializa ALL pins (look at electronics diagram)
 * - Figure our now.Date() stuff and rtc.now() function
 * - logged = false and logged = true (does it make sense?)
 * - Time_to_reverse = FALSE check that if/else if loop 
 * 
 * LATER
 * - Calibrate Kp value
 * - Calculate targetPuleRate
 */

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Encoder.h> //will need to install this library on whatever computer we use
//#include <TimeLib.h> //will need to install this library on whatever computer we use

// DateTime now = rtc.now();
 //PIN LAYOUT FOR EQUIPMENT
 /**
  * Change Pin Layout and define all pins used
  * 
  */
const int MPX_PlusVout_pin  = A0;// output pin for pressure transducer
const int MPX_MinusVout_pin  = A1; // output pin for pressure transducer
const int chipSelect        = 53;
const int motorPWMPin       = 9; // Connect the motor's PWM input pin to pin 9
const int encoderPinA       = 2; // Connect the encoder's A output pin to pin 2
const int encoderPinB       = 3; // Connect the encoder's B output pin to pin 3
const int Relay_A1          = 34; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Relay_A2          = 36; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Relay_B1          = 38; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Relay_B2          = 40; //For encoder/motor and Connected to H bridge (controls motor direction)
const int Wiper_LIMIT_SWITCH_1  = 24; // Connected to limit switch *MAY NEED TO SWITCH*
const int Wiper_LIMIT_SWITCH_2  = 25; // Connected to limit switch *MAY NEED TO SWITCH*//For encoder/motor and Connected to H bridge (controls motor direction)

const int PIN_NRL_LIMIT_SWITCH1 = 24; // Connected to limit switch *MAY NEED TO SWITCH*
const int PIN_NRL_LIMIT_SWITCH2 = 25; // Connected to limit switch *MAY NEED TO SWITCH*
#define FLOW_I2C_ADDR 0x8  //For Flow Sensor


// Take Data time
const int Take_data_interval = 200; // time interval between data (seconds)
unsigned long Arduino_time;

float Press_Reading;
String dataString = "";
bool logged = false;


float flow_reading; 

//%%%%%%%%%%%%%% START SETUP TAKE DATA FUNCTIONS FOR EQUIPMENT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//Pressure Transducer - SEPERATELY CHECKED AND RAN - BTE

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
  double Actual_PressRead = (voltage - Voff) / Press_Sensitivity; // [kPa]
  // Vout = Voff + (Sensitivity x Pressure)
return Actual_PressRead;
}


//Motor and Encoder Setup

// Target encoder pulses per second
const long targetPulseRate = 200; //need to calculate!!!!

// Proportional gain constant
const float Kp = 0.1; //control ossilation 

// Encoder setup
Encoder myEncoder(encoderPinA, encoderPinB);

// Variables for controlling the motor and encoder
long prevEncoderCount = 0;
long currentEncoderCount = 0;
long encoderPulseRate = 0;
int motorSpeed = 0;
int total_wipes = 0; //total number of wipes performed by system


// Time variables for motor/encoder
unsigned long prevTime = 0;
unsigned long currentTime = 0;
unsigned long timeInterval = 100; // Time interval for pulse rate calculation (ms)
long wipe_interval = 300000;  //How long we want to wait between wipes (in milliseconds) (5 minutes right now)
bool Time_to_wipe = false; //Boolean that determines when the code should control motor speed on initial wipe
bool Time_to_reverse = false; //Boolean that determines when the code should control motor speed on the wipe back


// Serial will not compile on the uno because it only exists on the mega but I chaged to Serial so there is no compile errors
// -----------------------------------------------------------------------------
// void init_pump (int a) {
// Serial.write(0x05); // ENQ with a
//  Serial.write(0x02); // Let a know a command is coming
//  Serial.write("P0");
//  Serial.print(a);
//  Serial.write(0x0D);  // termination character.
//  Serial.write(0x02);
//  Serial.write("P0");
//  Serial.print(a);
//  Serial.write ("Z0V0000010.00");
//  Serial.write(0x0D);  // termination character.
//   delay(50);
//  Serial.println();
// }

// void pump_speed (int pump_number, int directn, float spd) {
// Serial.write(0x02);
// Serial.write("P0");
// Serial.print(pump_number);

//  Serial.write("S");
//   if (directn == 1) {
//    Serial.write("-");
//   } else if (directn == 0) {
//    Serial.write("+");
//   }
//   if (spd <= 9) {
//    Serial.print("0");
//   }
//   if (spd <= 99) {
//    Serial.print("0");
//   }
//   if (spd <= 999) {
//    Serial.print("0");
//   }
//  Serial.print(spd);
//  Serial.write("G");


// Serial.write(0x0D);
//  Serial.println();
// }


// //Flow Sensor Setup

// float FlowRead () //Take flow measurements from flow sensor
// {
//   float flowrate = -1;
//   uint8_t bytesReceived = Wire.requestFrom(FLOW_I2C_ADDR, 9); //TODO: make sure this actually works
//   if((bool)bytesReceived){ //If received more than zero bytes
//     uint8_t temp[bytesReceived];
//     Wire.readBytes(temp, bytesReceived);
//     int flow_raw = temp[1] || temp[0] << 8;
//     //log_print_buf(temp, bytesReceived);
//     //Serial.println(flow_raw);
//    // TODO: convert raw to flow reading
//    flowrate = flow_raw / 500; // This is in mL/min
//   }
//   return flowrate;
// }
 // ----------------------------------------------------------------------------------

//%%%%%%%% SETUP START %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {
  //  Wire.begin();
  //  Wire.beginTransmission(FLOW_I2C_ADDR);
  //  Wire.write(0x3608); // Command to log continuosly 
    pinMode(motorPWMPin, OUTPUT); // This is for motor
    pinMode(forward1,  OUTPUT); //H Bridge connections to control direction
    pinMode(forward2,  OUTPUT);
    pinMode(reverse1,  OUTPUT);
    pinMode(reverse2,  OUTPUT);
    pinMode(PIN_NRL_LIMIT_SWITCH1,    INPUT);  //Limit Switch 1
    pinMode(PIN_NRL_LIMIT_SWITCH2,    INPUT);  //Limit Switch 2 chipSelect
    pinMode(chipSelect,    INPUT);
    Serial.begin(9600); // start the serial port

// --------- Initialization of SD card ---------------
Serial.print("Initializing SD card...");

// see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
     Serial.println("CARD SKIPPED");
    //while (1);
  }

  Serial.println("card initialized.");
// --------- End of Initialization of SD card ------------
  

//init_pump(1);

 // Pin assignmentts to Input/Output
  pinMode(MPX_PlusVout_pin, INPUT_PULLUP);
  pinMode(MPX_MinusVout_pin, INPUT_PULLUP);
}
//%%%%%%%% SETUP END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//%%%%%%%% LOOP START %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {
  // put your main code here, to run repeatedly:

//************** DATA LOG *********************************************
  // the timer will go back to zero after 50 days
  //Arduino_time =   millis()/1000;
  // Takes data every Take_data_interval
if ((millis() % Take_data_interval) == 0 && !logged)
{
  Press_Reading = Func_PressRead(MPX_PlusVout_pin, MPX_MinusVout_pin);
  //flow_reading = FlowRead(); 
  dataString += String(Press_Reading);
  dataString += ",";
  dataString += String(flow_reading);
  //dataString += ",";
  // add other take data functions in this if statement
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

Serial.println("DEBUGGING");
Serial.println("Press_reading = " + String(Press_Reading));

  
  // The following lines with the now function are currently commented out becasue of compiling error
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
 //dataFile.print("DATE ");
   // dataFile.print(now.day(), DEC); //*ERROR - CHECK DURING COMPILE*
   // dataFile.print('/');
    //dataFile.print(now.month(), DEC);
   // dataFile.print('/');
   // dataFile.print(now.year(), DEC);
   // dataFile.print("| TIME ");
   // dataFile.print(now.hour(), DEC);
   // dataFile.print(':');
   // dataFile.print(now.minute(), DEC);
   // dataFile.print(':');
   // dataFile.print(now.second(), DEC);
   // dataFile.print(",");
// delay(Take_data_interval);
//     dataFile.close();
//     dataString = "";
//     // print to the serial port too:
//     Serial.println(dataString);
//     logged = true;
// 
}
else
{
  logged = false;
}
 
  }

  //Motor and Encoder Loop Code:
  currentTime = millis();

  if(millis() % wipe_interval==0){
    Time_to_wipe = true;
    total_wipes++ ;
  }

  // Calculate the encoder pulse rate and motor speed for initial wipe
  if(Time_to_wipe == true && digitalRead(PIN_NRL_LIMIT_SWITCH1)==LOW ){ //If it's time to wipe and the limit switch is NOT being pressed
    if (currentTime - prevTime >= timeInterval) {
        currentEncoderCount = myEncoder.read();
        encoderPulseRate = abs(currentEncoderCount - prevEncoderCount) * (1000 / timeInterval);
        prevEncoderCount = currentEncoderCount;
        prevTime = currentTime;
    
        // Calculate the error and adjust motor speed
        long error = targetPulseRate - encoderPulseRate;
        motorSpeed += Kp * error;
        motorSpeed = constrain(motorSpeed, 0, 255);
    
        // Set motor speed and direction!!!! - GET THIS DOUBLE CHECKED
        digitalWrite(forward1,HIGH);  // rotate forward
        digitalWrite(forward2,HIGH); // rotate forward
         digitalWrite(reverse1,LOW);  // rotate forward
        digitalWrite(reverse2,LOW); // rotate forward
        analogWrite(motorPWMPin, motorSpeed);

    // Print information to the serial monitor
//    Serial.print("Target Pulse Rate: ");
//    Serial.print(targetPulseRate);
//    Serial.print(" | Current Pulse Rate: ");
//    Serial.print(encoderPulseRate);
//    Serial.print(" | Motor Speed: ");
//    Serial.println(motorSpeed);
    }
  }else if(Time_to_wipe == true && digitalRead(PIN_NRL_LIMIT_SWITCH1)==HIGH ){ //Limit switch was pressed
  Time_to_wipe = false;
  Time_to_reverse = true;
  motorSpeed = 0;
      analogWrite(motorPWMPin, motorSpeed);
  delay(5000); //stop wiper for 5 seconds (not sure if this will actually stop the motor??)
  }

if(Time_to_reverse == true && digitalRead(PIN_NRL_LIMIT_SWITCH2)==LOW){ //If it is time to reverse the wipe and the second limit switch is not pressed

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
       digitalWrite(forward1,LOW);  // rotate backwards
        digitalWrite(forward2,LOW); // rotate backwards
         digitalWrite(reverse1,HIGH);  // rotate backwards
        digitalWrite(reverse2,HIGH); // rotate backwards
      analogWrite(motorPWMPin, motorSpeed);
/**  
      // Print information to the serial monitor
      Serial.print("Target Pulse Rate: ");
      Serial.print(targetPulseRate);
      Serial.print(" | Current Pulse Rate: ");
      Serial.print(encoderPulseRate);
//      Serial.print(" | Motor Speed: ");
//      Serial.println(motorSpeed);
*/
   }

}

    else if(Time_to_reverse == true && digitalRead(PIN_NRL_LIMIT_SWITCH2)==HIGH ){
      Time_to_reverse = false;
      motorSpeed = 0;
      analogWrite(motorPWMPin, motorSpeed);
    }
}
//%%%%%%%% LOOP END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
