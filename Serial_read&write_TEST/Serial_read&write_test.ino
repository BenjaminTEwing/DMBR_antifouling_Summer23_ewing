//Serial_read&write_test.ino

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

Void loop()
{
scaleSerial.print("IP\r");
delay(50);
if scaleSerial.available()
{
	data = scaleSerial.readStringUntil("\n");
}

Serial.println(data);
Serial.println("pause 0.5 sec to avoid flooding the serial monitor");
delay(500);

} 