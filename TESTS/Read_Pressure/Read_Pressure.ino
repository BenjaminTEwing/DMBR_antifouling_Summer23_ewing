//Read_Pressure.ino
/*#include <SPI.h>
#include <SD.h>

*/

//pins
int MPX_PlusVout_pin = A0;// output pin for pressure transducer
int MPX_MinusVout_pin = A1; // output pin for pressure transducer
int Amplified_signal_pin = A2;
int send_voltage_A = 5;
int send_voltage_B = 3;

//variables
double Pressure_reading = 0.0; // pressure reading that is used for data
int Take_data_interval = 500;
float valuesensor1 = 0.0000;
float valuesensor2 = 0.0000;
double amplified_signal = 0.0000;
double volt_sensor1 = 0.0;
double volt_sensor2 = 0.0;
double Func_PressRead (int Vp, int Vn) // Pressure take data function declaration 
{
	valuesensor1 = analogRead(Vp); //read analong and convert to voltage
	valuesensor2 = analogRead(Vn); //read analong and convert to voltage
 	valuesensor1 = valuesensor1/1024 * 5 * 1000;
  valuesensor2 = valuesensor2/1024 * 5 * 1000;
  //Serial.println("(analog Val sense 1 = " + String(analogRead(Vp)) + ") (analog Val sense 2 = " + String(analogRead(Vn)) + ") ");
  //Serial.println("(Voltage Val sense 1 = " + String(valuesensor1) + ") (Voltage Val sense 2 = " + String(valuesensor2) + ") ");
	double Voff = 0; // [mV] max value is 35 per table specs
	double Press_Sensitivity = 1.2; // delta V / delta P [mV/kPa]
	double milliVoltage = valuesensor1 - valuesensor2;
	double Actual_PressRead = (milliVoltage - Voff) / Press_Sensitivity; // [kPa]
	// Vout = Voff + (Sensitivity x Pressure)
return Actual_PressRead;
}

void setup(){
Serial.begin(9600);
Serial.println();
Serial.println("Begining reading pressure 2 ways");

pinMode(MPX_PlusVout_pin, INPUT);
pinMode(MPX_MinusVout_pin, INPUT);
pinMode(Amplified_signal_pin, INPUT);
analogWrite(send_voltage_A, 51); //1V
analogWrite(send_voltage_B, 102); //2V
}
// END SETUP%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void loop(){
//if ((millis() % Take_data_interval) == 0)
	{
  delay(500);
	//Pressure_reading = Func_PressRead(MPX_PlusVout_pin, MPX_MinusVout_pin);
	//valuesensor1 = analogRead(MPX_PlusVout_pin); //read analong and convert to voltage
  //valuesensor2 = analogRead(MPX_MinusVout_pin); //read analong and convert to voltage
  //volt_sensor1 = valuesensor1/1024 * 5 * 1000;
  //volt_sensor2 = valuesensor2/1024 * 5 * 1000;
	amplified_signal = analogRead(Amplified_signal_pin);
	//Serial.print("Sensor Voltage plus = " + String(valuesensor1) + " | Sensor Val minus = " + String(valuesensor2));
  //Serial.print("Voltage 1 (mV) = " + String(volt_sensor1) + "| Voltage 2 m(V) = " + String(volt_sensor2));
	//Serial.print("| Voltage Difference (mV) =  " + String((volt_sensor1-volt_sensor2)));
	Serial.print(" | Probe signal = " + String(amplified_signal) + " | Probe Voltage (mV) = " + String(amplified_signal/1024*5*1000));
  Serial.println();
	}
}
