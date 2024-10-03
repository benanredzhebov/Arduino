// Arduino pins used for the LEDs and the button
const int greenLED = 8;
const int yellowLED = 9;
const int redLED = 10;
const int buttonPin = 2;

// Boolean variables to track blinking state and button state
bool isBlinking = false;  
bool buttonPressed = false;

// Variables for timing the blinking of yellow and red LEDs
unsigned long previousMillisYellow = 0;  
unsigned long previousMillisRed = 0;

// Intervals for yellow and red LED blinking
const long intervalYellow = 2000; // Yellow LED blink interval (2 seconds)
const long intervalRed = 500;     // Red LED blink interval (0.5 seconds) 

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
    buttonPressed = false; // Reset button state when the button is released 
  }
  // If in blinking mode, toggle the yellow and red LEDs at the set intervals
  if (isBlinking) {
    unsigned long currentMillis = millis(); // Get the current time in milliseconds

    // Handle yellow LED blinking based on its interval
    if (currentMillis - previousMillisYellow >= intervalYellow) {
      previousMillisYellow = currentMillis; // Update previous time for yellow LED
      digitalWrite(yellowLED, !digitalRead(yellowLED)); // Toggle yellow LED state
    }

    // Handle red LED blinking based on its interval
    if (currentMillis - previousMillisRed >= intervalRed) {
      previousMillisRed = currentMillis; // Update previous time for red LED
      digitalWrite(redLED, !digitalRead(redLED)); // Toggle red LED state
    }
  }
}
