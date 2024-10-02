#include <Servo.h>  // Include the Servo library

int ldr1Pin = A0;  // LDR 1 connected to analog pin A0
int ldr2Pin = A1;  // LDR 2 connected to analog pin A1
int servoPin = 9;  // Servo connected to digital pin 9
int sensorValue1 = 0;
int sensorValue2 = 0;
int servoGrad = 90;  // Start at the middle position (90 degrees)
int tolerance = 100; // Tolerance to avoid small fluctuations

Servo myservo;

void setup() {
  pinMode(ldr1Pin, INPUT);  // Set LDR 1 pin as input
  pinMode(ldr2Pin, INPUT);  // Set LDR 2 pin as input
  myservo.attach(servoPin); // Attach the servo
  myservo.write(servoGrad); // Start with the servo in the middle
}

void loop() {
  sensorValue1 = analogRead(ldr1Pin);  // Read value from LDR 1
  sensorValue2 = analogRead(ldr2Pin);  // Read value from LDR 2

  // If LDR 1 detects more light (rotate right)
  if (sensorValue1 > sensorValue2 + tolerance) {
    if (servoGrad < 180) {
      servoGrad++;  // Increase the angle to rotate right
    }
  }

  // If LDR 2 detects more light (rotate left)
  if (sensorValue2 > sensorValue1 + tolerance) {
    if (servoGrad > 0) {
      servoGrad--;  // Decrease the angle to rotate left
    }
  }

  myservo.write(servoGrad);  // Move the servo to the new position

  delay(100);  // Short delay to stabilize the sensor readings
}