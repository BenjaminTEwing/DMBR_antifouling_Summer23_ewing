//Scale serial communication test
// Ohaus Series 3000 T31P Scale Communication

#include <SoftwareSerial.h>

SoftwareSerial scaleSerial(10, 11);  // RX, TX pins

//const int MAX_VALUES = 10;  // Maximum number of values to store
//int values[MAX_VALUES];     // Array to store the values
//int numValues = 0;          // Number of values currently stored
string Serial_read = "";


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
void setup() 
{
	Serial.begin(9600);      // Serial monitor baud rate
	scaleSerial.begin(9600); // Scale baud rate
	scaleSerial.setTimeout(1000);  // Set timeout for serial communication

	Serial.println("Begin Test of Serial Connectivity, end setup");
}

Void loop()
{
Serial_read = Read_Weight()

} 
