#include <SPI.h>
#include <SD.h>

File dataFile;
int fileNumber = 0;

void setup() {
  // Open serial communication for debugging
  Serial.begin(9600);

  // Initialize SD card
  if (!SD.begin(10)) {
    Serial.println("SD card initialization failed");
    return;
  }

  // Find the next available file number
  while (SD.exists(String(fileNumber) + ".txt")) {
    fileNumber++;
  }

  // Create new data file with unique name
  dataFile = SD.open(String(fileNumber) + ".txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error creating new file");
    return;
  }

  // Write some test data to the file
  dataFile.println("This is a new file");
  dataFile.close();

  // Print debugging information
  Serial.print("Created new file ");
  Serial.print(fileNumber);
  Serial.println(".txt");
}

void loop() {
  // do nothing
}
