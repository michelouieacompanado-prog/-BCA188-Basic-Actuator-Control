/*
  BCA188 - Laboratory Activity 6: Basic Actuator Control
  Example 7: Hold Button to Run Motor (Level-Controlled)
*/

#include <Arduino.h>

// Pin Definitions
const uint8_t BUTTON_PIN = 23;
const uint8_t MOTOR_IN1 = 21;
const uint8_t MOTOR_IN2 = 22;
const uint8_t DRIVER_SLEEP_PIN = 27;

void setup() {
  // Configure motor driver control pins as outputs
  pinMode(DRIVER_SLEEP_PIN, OUTPUT);
  digitalWrite(DRIVER_SLEEP_PIN, LOW); // Start in low-power sleep mode
  
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  // Configure push button with internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Wake up driver breakout (active HIGH)
  digitalWrite(DRIVER_SLEEP_PIN, HIGH);
  delay(2); // Required wake-up stabilization delay
}

void loop() {
  // Button reads LOW when pressed due to INPUT_PULLUP circuit
  const bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  // Keep IN2 LOW for single-direction drive
  digitalWrite(MOTOR_IN2, LOW);

  // Drive IN1 HIGH while button is held, otherwise drop LOW (coast mode)
  digitalWrite(MOTOR_IN1, buttonPressed ? HIGH : LOW);
}
