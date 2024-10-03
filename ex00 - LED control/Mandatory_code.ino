// Arduino pins used for the LEDs and the button
const int greenLED = 8;
const int yellowLED = 9;
const int redLED = 10;
const int buttonPin = 2;

// Boolean variables to track blinking state and button state
bool isBlinking = false;  
bool buttonPressed = false;  

// Variables for timing the LED blinking
unsigned long previousMillis = 0; // Stores the last time the LEDs were toggled
const long interval = 1000; // Interval between LED toggles in milliseconds (1 second)

void setup() {
  // Set the LED pins as output pins
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // Set the button pin as input with an internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Initially turn on the green LED and turn off the yellow and red LEDs
  digitalWrite(greenLED, HIGH);
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
}

void loop() {
  // Check if the button is pressed (LOW because INPUT_PULLUP is used)
  if (digitalRead(buttonPin) == LOW) {  
    if (!buttonPressed) {  
      buttonPressed = true; // Mark button as pressed
      isBlinking = !isBlinking; // Toggle the blinking state
      
      // If now blinking, turn off the green LED
      if (isBlinking) {
        digitalWrite(greenLED, LOW);
      } else {
        // If blinking stopped, reset LEDs: turn on green, turn off yellow and red
        digitalWrite(greenLED, HIGH);
        digitalWrite(yellowLED, LOW);
        digitalWrite(redLED, LOW);
      }
    }
  } else {
    buttonPressed = false;  // Reset button state when the button is released
  }

  // If in blinking mode, toggle the yellow and red LEDs at the set interval
  if (isBlinking) {
    unsigned long currentMillis = millis(); // Get the current time in milliseconds
    if (currentMillis - previousMillis >= interval) { // Check if enough time has passed (1 second)
      previousMillis = currentMillis; // Update the previous time to current time

      // Toggle the state of yellow and red LEDs
      digitalWrite(yellowLED, !digitalRead(yellowLED)); // Turn yellow LED on/off
      digitalWrite(redLED, !digitalRead(redLED)); // Turn red LED on/off
    }
  }
}
