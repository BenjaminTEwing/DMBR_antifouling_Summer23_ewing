#include <SD.h>


/* Pin Layout
% 5V --> 5V on arduino
GND --> GND on arduino
CLK --> PIN 52 
DO --> PIN 50
DI --> PIN 51
CS --> PIN 53

THIS CODE WILL DO A BASIC DEMONSTRATION OF WRITITNG TO A FILE
*/

 
// SET UP VARIABLES USING THE SD utitlity library function:

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// aDAFRUIT SD shields and modules: pin 10
// SparkFun SD shield: pin 8

 
void setup(){
Serial.begin(9600);
 Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(53, OUTPUT);
 
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
File myFile = SD.open("test.txt", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
 
void loop()
{
	// nothing happens after setup
}
