//Read_Pressure.ino
//Pressure Transducer - SEPERATELY CHECKED AND RAN - BTE

/*#include <SPI.h>
#include <SD.h>
#include <Wire.h>
*/

//pins
int MPX_PlusVout_pin = A0;// output pin for pressure transducer
int MPX_MinusVout_pin = A1; // output pin for pressure transducer

//variables
double Pressure_reading = 0.0; // pressure reading that is used for data
int Take_data_interval = 1000;
String Data_output_string;
float print_time = 0;
//%%%%%%%%%%%%% BEGIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup(){
Serial.begin(9600);
Serial.println();
Serial.println("Begining reading pressure");

pinMode(MPX_PlusVout_pin, INPUT_PULLUP);
pinMode(MPX_MinusVout_pin, INPUT_PULLUP);
}
// END SETUP%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void loop(){
if ((millis() % Take_data_interval) == 0)
	{
	Pressure_reading = Func_PressRead(MPX_PlusVout_pin, MPX_MinusVout_pin);
	Data_output_string = " ";
	Data_output_string += String(Pressure_reading);
  Data_output_string += String(" KPA &  ");
	Data_output_string += String("Current Time = ");
  print_time = millis()/1000;
	Data_output_string += String(print_time); 
	Serial.println(Data_output_string);
  Serial.println();
	}
}
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