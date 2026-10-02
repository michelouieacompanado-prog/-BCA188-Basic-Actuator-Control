/*
  BCA188 - Laboratory Activity 6: Basic Actuator Control
  Optional Exercise: Potentiometer PWM Speed Control & Onset Duty Cycle Measurement
*/

#include <Arduino.h>

// Pin Definitions
const uint8_t MOTOR_IN1 = 21;         // PWM Drive Signal
const uint8_t MOTOR_IN2 = 22;         // Held LOW
const uint8_t DRIVER_SLEEP_PIN = 27;  // Wake-up pin
const uint8_t POT_PIN = 34;           // Analog input from Potentiometer

void setup() {
  Serial.begin(115200); // For logging duty cycle to measure motor onset

  // Pin Configurations
  pinMode(DRIVER_SLEEP_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(DRIVER_SLEEP_PIN, LOW);
  digitalWrite(MOTOR_IN2, LOW); // Keep IN2 LOW for forward drive

  // Wake up driver breakout
  digitalWrite(DRIVER_SLEEP_PIN, HIGH);
  delay(2);
  
  Serial.println("--- Potentiometer PWM Control Initialized ---");
}

void loop() {
  // Read analog value from potentiometer (0 - 4095)
  uint16_t potRaw = analogRead(POT_PIN);

  // Map 12-bit ADC reading to 8-bit PWM resolution (0 - 255)
  uint8_t pwmValue = map(potRaw, 0, 4095, 0, 255);

  // Output PWM signal to AIN1
  analogWrite(MOTOR_IN1, pwmValue);

  // Print raw ADC and mapped Duty Cycle to Serial Monitor
  Serial.print("Pot ADC: ");
  Serial.print(potRaw);
  Serial.print(" | Mapped Duty Cycle (0-255): ");
  Serial.println(pwmValue);

  delay(100); // Small delay to readable Serial output rate
}
