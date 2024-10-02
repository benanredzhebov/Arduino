#include <LiquidCrystal.h>

// Define constants
const int ledPin = 9; // Pin for LED control
const int maxSensorValue = 1023; // Maximum potentiomer value
const int minSensorValue = 0; // Minimum potentiomer value
const float percentConversionFactor = 10.23; // Conversion factor for percentage input

long prevSensorValue = -1; // Previous sensor value
long prevIncomingVal = -1; // Previous incoming value

// Initializes the LiquidCrystal object with the pin numbers connected to the LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // LCD pins

// Setup function to initialize the LCD and Serial communication
void setup() {
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
}

// Function to display the message on the LCD
void displayMessage(const String& line1, const String& line2) {
  lcd.clear(); // Clear the LCD screen
  lcd.setCursor(0, 0); // Set the cursor to the first column of the first row
  lcd.print(line1); // Print the message on the first line
  lcd.setCursor(0, 1); // Set the cursor to the first column of the second row
  lcd.print(line2); // Print the message on the second line
}

// Function to update the LCD display based on value
void updateLcd(long val, bool fromSerial) {
  int brightnessPercent = val / percentConversionFactor; // Calculate brightness percentage
  String line1 = fromSerial ? "Serial val: " : "Sensor val: "; // Check if value is from serial or sensor
  line1 += val; // Append the value to the message
  line1 += "          "; // Ensure the line is cleared

  String line2 = "Brightness: "; // Display brightness percentage
  line2 += brightnessPercent; // Append the brightness percentage to the message
  line2 += "%            ";  // Ensure the line is cleared

  displayMessage(line1, line2); // Display the message on the LCD
}

// Function to control the LED and update the LCD
void controlLed(long val, bool fromSerial) {
  if (val > maxSensorValue) { // Check if value exceeds maximum sensor value
    displayMessage("Incoming: " + String(val), "Must be <= " + String(maxSensorValue)); // Display error message
  } 
  else if (val < minSensorValue) { // Check if value is below minimum sensor value
    displayMessage("Incoming: " + String(val), "Must be >= " + String(minSensorValue)); // Display error message
  } 
  else { // If value is within sensor range
    updateLcd(val, fromSerial); // Update the LCD display
    analogWrite(ledPin, val / 4); // Control the LED brightness
  }
}

// Function to handle serial input and convert it if needed
void handleSerialInput() { // Function to handle serial input
  if (Serial.available() > 0) {
    long incomingVal = Serial.parseInt();  // Read integer value from serial
    char proc = Serial.read();  // Read next character to check for '%'
    
    // Convert percentage input to sensor value range
    if (proc == '%') { // Check if the incoming value is a percentage
      incomingVal = incomingVal * percentConversionFactor; // Convert percentage to sensor value range
    }

    // Process new value only if it's different from the previous one
    if (incomingVal != prevIncomingVal) { // Check if the incoming value is different from the previous value
      prevIncomingVal = incomingVal; // Update the previous value
      controlLed(incomingVal, true); // Control the LED and update the LCD
    }

    // Clear remaining serial data
    while (Serial.available()) { // Check if there is any remaining data in the serial buffer
      Serial.read(); // Read and discard the remaining data
    }
  }
}

void loop() {
  // Read sensor value from A0
  long sensorValue = analogRead(A0);

  // Update LED and LCD only if the sensor value has changed significantly
  if (abs(sensorValue - prevSensorValue) > 1) { // Check if the sensor value has changed significantly
    prevSensorValue = sensorValue; // Update the previous sensor value
    controlLed(sensorValue, false); // Control the LED and update the LCD
  }

  // Handle any serial input
  handleSerialInput();

  delay(200);  // Delay for stability
}
