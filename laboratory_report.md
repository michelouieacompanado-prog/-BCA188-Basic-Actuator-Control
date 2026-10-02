# LABORATORY REPORT 6: BASIC ACTUATOR CONTROL

**Course Code:** BCA188 — Internet of Things / Embedded Systems  
**Activity Title:** Laboratory Activity 6: Basic Actuator Control  
**Student Name:** Miche Louie B. Acompañado 
**Degree Program:** BS Computer Applications (IoT Track)  
**Institution:** Mindanao State University – Iligan Institute of Technology  
**Date of Execution:** October 2, 2026  

---

## I. OBJECTIVES

1. Construct and verify a DC motor driver circuit using an ESP32 microcontroller and a Texas Instruments DRV8833 dual H-bridge breakout board.
2. Demonstrate level-controlled run and coast motor behavior using digital button logic.
3. Evaluate the functional roles of control signals (`AIN1`, `AIN2`), driver wake-up logic (`nSLEEP`), motor power isolation (`VM`), and common ground reference.
4. Measure and document the minimum Pulse Width Modulation (PWM) duty cycle required to overcome static mechanical friction (onset of rotation) on a 3V DC motor.

---

## II. HARDWARE SPECIFICATIONS & DATASHEETS

### 1. DC Motor
* **Nominal Rated Voltage:** 3.0V DC (Operating Range: 1.5V – 4.5V)
* **Stall Current:** ~0.80 A @ 3.0V
* **No-Load Current:** ~0.12 A
* **Datasheet Reference:** [Adafruit 3V DC Motor Specifications (ID: 711)](https://www.adafruit.com/product/711)

### 2. Motor Driver Breakout Board
* **Model:** Texas Instruments DRV8833 Dual H-Bridge Motor Driver
* **Motor Supply Voltage ($V_M$):** 2.7V to 10.8V
* **Continuous Output Current:** 1.5A RMS per channel (2.0A Peak)
* **Logic Compatibility:** 1.8V to 5.0V logic systems
* **Protection Features:** Overcurrent Protection (OCP), Short-Circuit Protection, Thermal Shutdown (TSD), and Undervoltage Lockout (UVLO).
* **Datasheet Reference:** [Texas Instruments DRV8833 Datasheet (SLRS096E)](https://www.ti.com/lit/ds/symlink/drv8833.pdf)

---

## III. CONTROL SIGNALS & SYSTEM PINOUT MAPPING

### 1. Driver Signal Roles
* **`AIN1` & `AIN2` (H-Bridge Control Inputs):** Logic-level switching pins that determine the direction and state of the internal H-bridge MOSFETs. Setting `AIN1 = HIGH` and `AIN2 = LOW` completes the forward power path. Setting `AIN1 = LOW` and `AIN2 = LOW` places the driver outputs in high-impedance Coast mode.
* **`nSLEEP` (Active-LOW Wake/Disable):** Disables internal charge pumps and logic circuits when pulled LOW ($V < 0.5\text{V}$), dropping current draw below $1.6\,\mu\text{A}$. Pulling `nSLEEP` HIGH ($V > 2.0\text{V}$) wakes up the IC. A compulsory 2 ms delay is required post-wake before issuing drive commands.
* **`VM` (Motor Power Rail):** Dedicated external power rail for motor loads. Isolates high-power inductive switching currents from the low-power 3.3V logic rail, preventing brownouts and voltage spike disruptions on the microcontroller.
* **`Common Ground` (Shared 0V Reference):** Interconnects the logic ground of the ESP32 with the power ground of the motor power supply. Establishing a uniform 0V baseline is required for digital signal logic thresholds to register accurately.

### 2. Microcontroller Pin Mapping Table

| Source Signal | ESP32 GPIO Pin | Configured Mode | Function Description |
| :--- | :--- | :--- | :--- |
| **Push Button** | `GPIO 23` | `INPUT_PULLUP` | Reads `LOW` when pressed, `HIGH` when unpressed |
| **DRV8833 AIN1** | `GPIO 21` | `OUTPUT` | Direction / PWM command signal pin |
| **DRV8833 AIN2** | `GPIO 22` | `OUTPUT` | Direction reference pin (Held `LOW`) |
| **DRV8833 nSLEEP** | `GPIO 27` | `OUTPUT` | Active-LOW driver wake-up pin |
| **Potentiometer** | `GPIO 34` | `INPUT` (ADC1_CH6) | 12-bit analog input for PWM duty scaling |

---

## IV. DRV8833 LOGIC TRUTH TABLE

| `nSLEEP` | `AIN1` | `AIN2` | H-Bridge State | Resulting Motor Behavior |
| :---: | :---: | :---: | :--- | :--- |
| **LOW** | X | X | High-Z Outputs | **Sleep / Fault Mode:** Ultra-low power state |
| **HIGH** | HIGH | LOW | $AOUT1 = V_M, AOUT2 = GND$ | **Forward Drive:** Motors turns forward |
| **HIGH** | LOW | HIGH | $AOUT1 = GND, AOUT2 = V_M$ | **Reverse Drive:** Motor turns in reverse |
| **HIGH** | LOW | LOW | High-Z Outputs (Fast Decay) | **Coast Mode:** Shaft spins down freely |
| **HIGH** | HIGH | HIGH | Low-Side MOSFETs Active | **Brake Mode:** Terminals shorted; locks shaft |

---

## V. SOURCE CODE IMPLEMENTATIONS

### 1. Main Task: Level-Controlled Drive (`Example7.ino`)

```cpp
/*
  BCA188 - Laboratory Activity 6: Basic Actuator Control
  Task 1: Level-Controlled Run/Coast Motor Drive
*/

#include <Arduino.h>

const uint8_t BUTTON_PIN = 23;
const uint8_t MOTOR_IN1 = 21;
const uint8_t MOTOR_IN2 = 22;
const uint8_t DRIVER_SLEEP_PIN = 27;

void setup() {
  pinMode(DRIVER_SLEEP_PIN, OUTPUT);
  digitalWrite(DRIVER_SLEEP_PIN, LOW); // Hold driver in low-power sleep
  
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Active-LOW button setup

  digitalWrite(DRIVER_SLEEP_PIN, HIGH); // Wake up DRV8833
  delay(2);                            // Required wake-up interval
}

void loop() {
  const bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN1, buttonPressed ? HIGH : LOW);
}
```

---

### 2. Optional Exercise: Potentiometer PWM Control (`PotentiometerPWM.ino`)

```cpp
/*
  BCA188 - Laboratory Activity 6: Basic Actuator Control
  Task 2: Potentiometer PWM Speed Control & Onset Duty Cycle Measurement
*/

#include <Arduino.h>

const uint8_t MOTOR_IN1 = 21;         // PWM drive signal
const uint8_t MOTOR_IN2 = 22;         // Held LOW
const uint8_t DRIVER_SLEEP_PIN = 27;  // Active-LOW driver wake pin
const uint8_t POT_PIN = 34;           // Analog input from Potentiometer

void setup() {
  Serial.begin(115200);

  pinMode(DRIVER_SLEEP_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(DRIVER_SLEEP_PIN, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  digitalWrite(DRIVER_SLEEP_PIN, HIGH);
  delay(2);
  
  Serial.println("--- DRV8833 Potentiometer PWM Test Initialized ---");
}

void loop() {
  uint16_t potRaw = analogRead(POT_PIN);              // Read 12-bit ADC (0 - 4095)
  uint8_t pwmValue = map(potRaw, 0, 4095, 0, 255);    // Map to 8-bit PWM (0 - 255)

  analogWrite(MOTOR_IN1, pwmValue);                   // Output PWM to AIN1

  float dutyPercentage = (pwmValue / 255.0) * 100.0;
  
  Serial.print("ADC Raw: ");
  Serial.print(potRaw);
  Serial.print(" | PWM Duty: ");
  Serial.print(pwmValue);
  Serial.print(" / 255 (");
  Serial.print(dutyPercentage, 1);
  Serial.println("%)");

  delay(100);
}
```

---

## VI. EXPERIMENTAL RESULTS & ANALYSIS

### 1. Verification of Run and Coast Behavior
* **Button Held (`GPIO 23 = LOW`):** `MOTOR_IN1` transitions to `HIGH`, applying full $V_M$ across output channel `AOUT1` relative to `AOUT2`. Current flows through the motor winding, resulting in steady rotation.
* **Button Released (`GPIO 23 = HIGH`):** `MOTOR_IN1` drops to `LOW`. With both `AIN1` and `AIN2` at `LOW`, the DRV8833 enters **Coast Mode** (Fast Decay). The driver outputs go high-impedance, letting rotor momentum dissipate gradually over ~0.5s due to mechanical friction, rather than abruptly locking.

### 2. Onset of Rotation Measurement
Due to static mechanical friction ($T_{stiction}$) in the brushes and motor bearings, applying low duty cycles does not immediately yield rotation. The onset threshold was empirically recorded via the Serial Monitor at the exact point initial movement occurred:

| Measured Parameter | Recorded Experimental Value |
| :--- | :--- |
| **Raw ADC Input (GPIO 34)** | `1044` / 4095 |
| **8-bit PWM Register Value** | `65` / 255 |
| **Onset Duty Cycle Percentage** | **`25.5%`** |
| **Effective Motor Voltage ($V_{eff}$)** | $V_{eff} = 3.0\text{V} \times 0.255 = \mathbf{0.765\text{V}}$ |

#### Analytical Explanation:
* **Below 25.5% Duty Cycle (PWM < 65):** Average terminal voltage ($V_{eff} < 0.765\text{V}$) creates electromagnetic torque insufficient to overcome static friction:
  $$T_{electro} = k_t \cdot I_{avg} < T_{stiction}$$
  An audible coil hum was noted between 10% and 24% duty cycle without shaft motion.
* **At $\mathbf{\ge 25.5\%}$ Duty Cycle (PWM $\ge$ 65):** Electromagnetic torque satisfies $T_{electro} \ge T_{stiction}$, establishing continuous rotation.

---

## VII. TECHNICAL DISCUSSION

1. **Why is a Common Ground mandatory between the ESP32 and DRV8833?**  
   Without a common ground, signal lines (`AIN1`, `AIN2`, `nSLEEP`) lack a shared 0V reference. Floating reference levels cause unpredictable logic state evaluation, false switching, or complete driver failure.
2. **Why separate $V_M$ from the 3.3V logic power rail?**  
   DC motors induce inductive voltage spikes (back-EMF) and high current surges during operation. Isolating $V_M$ on a dedicated power supply prevents microcontroller power drops (brownouts) and protects GPIO pins from voltage spikes.
3. **What is the difference between Coast Mode and Active Brake Mode?**  
   * **Coast Mode (`AIN1=0, AIN2=0`):** Output transistors go high-impedance, letting the shaft spin down freely under inertia.  
   * **Brake Mode (`AIN1=1, AIN2=1`):** Short-circuits the motor terminals through the low-side MOSFETs, inducing opposing electromagnetic force that rapidly decelerates the rotor.

---

## VIII. CONCLUSION

Laboratory Activity 6 successfully demonstrated basic actuator control using an ESP32 microcontroller and the DRV8833 H-Bridge breakout IC. The level-control experiment validated run and coast switching modes. Extension to potentiometer-based PWM speed control highlighted the non-linear relationship between duty cycle and mechanical shaft rotation, establishing the motor's onset of rotation threshold at a minimum duty cycle of **25.5%** ($V_{eff} \approx 0.765\text{V}$).
