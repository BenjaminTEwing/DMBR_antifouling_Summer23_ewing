//Notes

/*Make voltage driver function
	input volts
	no output

Make Serial read function	
*/	
#include <SoftwareSerial.h>

SoftwareSerial scaleSerial(10, 11);  // RX, TX pins

const int MAX_VALUES = 10;  // Maximum number of values to store
int values[MAX_VALUES];     // Array to store the values
int numValues = 0;          // Number of values currently stored
string data = "";

void setup() 
{
	Serial.begin(9600);      // Serial monitor baud rate
	scaleSerial.begin(9600); // Scale baud rate
	scaleSerial.setTimeout(1000);  // Set timeout for serial communication

	Serial.println("Begin Test of Serial Connectivity, end setup");
}

void loop()
{
	scaleSerial.print("IP\r");
	scaleSErial.print("This is a test at time = " + string(milli()/1000) + " seconds\n");

	delay(1000);
}