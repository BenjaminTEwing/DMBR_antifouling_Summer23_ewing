//Scale serial communication test
// Ohaus Series 3000 T31P Scale Communication

#include <SoftwareSerial.h>

SoftwareSerial scaleSerial(10, 11);  // RX, TX pins

void setup() {
  Serial.begin(9600);      // Serial monitor baud rate
  scaleSerial.begin(9600); // Scale baud rate
  scaleSerial.setTimeout(1000);  // Set timeout for serial communication
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
