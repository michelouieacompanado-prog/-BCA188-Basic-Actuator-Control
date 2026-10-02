# Laboratory Activity 6: Basic Actuator Control

## 1. Hardware Specifications

| Component | Rated Voltage / Limits | Stall Current / Peak Limit | Datasheet Link |
| :--- | :--- | :--- | :--- |
| **DC Motor** | `[Insert Volts e.g., 3V - 6V]` | `[Insert Amps e.g., 1.5A]` | `[Insert URL]` |
| **Driver Breakout** | `[Insert Driver Volts e.g., 2.7V - 10.8V]` | `[Insert Driver Amps e.g., 1.5A RMS]`| `[Insert URL]` |

## 2. Pin Explanations

* **AIN1 & AIN2:** Logic pins that control the H-bridge channel state. The high/low combination across both pins determines the motor's direction (Forward, Reverse) or idle state (Coast, Brake).
* **nSLEEP:** An active-low pin. Pulling it HIGH wakes up the driver; pulling it LOW puts it into a low-power sleep state and clears faults.
* **VM (Motor Voltage):** The external power supply input dedicated strictly to driving the motor load, kept isolated from logic voltage.
* **Common Ground:** Connects the logic ground and external motor power ground to ensure a shared 0V reference for stable H-bridge switching.

## 3. Example 7: Button-Controlled Run/Coast

**Observed Behavior:** 
`[Record your observation here. e.g., The motor begins rotating immediately while the button on GPIO 23 is held. Upon releasing the button, AIN1 drops LOW matching AIN2, causing the motor to coast smoothly to a stop rather than braking aggressively.]`

### Base Source Code
```cpp
#include <Arduino.h>
const uint8_t BUTTON_PIN = 23;
const uint8_t MOTOR_IN1 = 21;
const uint8_t MOTOR_IN2 = 22;
const uint8_t DRIVER_SLEEP_PIN = 27;

void setup() {
  pinMode(DRIVER_SLEEP_PIN, OUTPUT);
  digitalWrite(DRIVER_SLEEP_PIN, LOW);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(DRIVER_SLEEP_PIN, HIGH);
  delay(2); // One-time driver wake-up interval.
}

void loop() {
  const bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN1, buttonPressed ? HIGH : LOW);
}
