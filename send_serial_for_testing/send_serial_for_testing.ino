//Notes

/*Make voltage driver function
	input volts
	no output

Make Serial read function	
*/	
#include <SoftwareSerial.h>
//
//SoftwareSerial scaleSerial(10, 11);  // RX, TX pins

// const int MAX_VALUES = 10;  // Maximum number of values to store
// int values[MAX_VALUES];     // Array to store the values
// int numValues = 0;          // Number of values currently stored
//String data = "";

void setup() 
{
	Serial.begin(9600);      // Serial monitor baud rate
	Serial.println("Begin Test of Serial Connectivity, end setup");
}

void loop()
{
	//Serial.print("IP\r ");
  //delay(500);
	Serial.print("This is a test at time = " + String(millis()/1000) + " seconds \r \n");
	delay(1000);
}