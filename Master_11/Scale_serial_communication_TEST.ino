//Scale serial communication test
// Ohaus Series 3000 T31P Scale Communication

#include <SoftwareSerial.h>

SoftwareSerial scaleSerial(10, 11);  // RX, TX pins

void setup() {
  Serial.begin(9600);      // Serial monitor baud rate
  scaleSerial.begin(9600); // Scale baud rate
  scaleSerial.setTimeout(1000);  // Set timeout for serial communication

const int MAX_VALUES = 10;  // Maximum number of values to store
int values[MAX_VALUES];     // Array to store the values
int numValues = 0;          // Number of values currently stored

void readSerialData() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // Read the input data

    // Process the data
    int start = 0;
    int end = data.indexOf(' ');
    int index = 0;

    while (end >= 0 && index < MAX_VALUES) {
      String valueStr = data.substring(start, end);  // Extract value string
      values[index] = valueStr.toInt();              // Convert to integer and store in array
      index++;

      start = end + 1;
      end = data.indexOf(' ', start);  // Find the next space character
    }

    // Store the last value (after the last space) if there's still space in the array
    if (index < MAX_VALUES) {
      String lastValueStr = data.substring(start);  // Extract last value string
      values[index] = lastValueStr.toInt();         // Convert to integer and store in array
      index++;
    }

    numValues = index;  // Update the number of values stored

    // Print the values to verify
    for (int i = 0; i < numValues; i++) {
      Serial.print("Value ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(values[i]);
    }
  }
}
#define MAX_VALUES 10  // Maximum number of values to store

void parseSerialData(int data[], int size) {
  // Clear the array
  for (int i = 0; i < size; i++) {
    data[i] = 0;
  }

  // Read input from Serial monitor
  String input = Serial.readStringUntil('\n');

  // Split the input into individual values
  int count = 0;  // Number of values found
  int startIndex = 0;  // Start index of each value
  int endIndex = 0;  // End index of each value

  while (endIndex >= 0 && count < size) {
    endIndex = input.indexOf(' ', startIndex);  // Find the index of the space separator

    if (endIndex >= 0) {
      // Extract the value between startIndex and endIndex
      String valueString = input.substring(startIndex, endIndex);

      // Convert the value string to an integer and store it in the array
      data[count] = valueString.toInt();

      // Update the start index for the next value
      startIndex = endIndex + 1;
      count++;
    }
  }

  // Print the values to Serial monitor
  for (int i = 0; i < count; i++) {
    Serial.print("Value ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(data[i]);
  }
}

}

void loop() {
  // Send command to request weight data
  scaleSerial.write("IP\r\n");

  // Wait for response
  delay(100);  // Delay to allow scale to respond
  if (scaleSerial.available()) {
    String response = scaleSerial.readStringUntil('\n');

    // Process response
    if (response.startsWith("P")) {
      // Extract weight value from the response
      String weightString = response.substring(1);  // Remove the 'P' character
      float weight = weightString.toFloat();

      // Print weight to Serial monitor
      Serial.print("Weight: ");
      Serial.println(weight);
    }
  }

  delay(1000);  // Wait for 1 second before requesting weight again
}
