<img width="1280" height="640" alt="git (1)" src="https://github.com/user-attachments/assets/8920b256-2ba8-4988-b824-5351134eb4bd" />

# HateBot 🤖😤

> *The utterly useless judgmental robot that exists purely to make you regret touching it.*

---

## Basic Details
### Team Name: Hateu_robo

### Project Description
HateBot sits on your desk, detects when you've disturbed it, and responds with maximum contempt — a disgusted face on screen, a dismissive arm flick, an annoyed buzz, and a mood LED that escalates from "tolerating you" to "furious." The joke is the point: massive engineering effort, zero useful output.

### The Problem (that doesn't exist)
Nobody asked for a robot that hates them. Desktop robots are usually friendly and helpful. This is wrong.

### The Solution (that nobody asked for)
A robot with a full emotional state machine calibrated entirely around contempt. The more you bother it, the angrier it gets, until it eventually gives up on you entirely and sulks in silence.

---

## Mood State Machine

```
        [idle sigh]
            |
        (IMU: small tilt) ──> ANNOYED ──(more disturbance)──> FURIOUS
            |                     |                               |
      (no input, 12s)       (no input, 10s)               (no input, 30s)
            |                     |                               |
            v                     v                               v
          CALM  <──────────────────────────────────────────── SULKING
                                                          (after 3 FURIOUS hits)
```

| Mood | LED | Face | Arms | Sound |
|---|---|---|---|---|
| **CALM** | 🟢 slow pulse | Bored slits, flat mouth | Occasional slow twitch | Rare low "hmph" |
| **ANNOYED** | 🟡 solid | Side-eye, angled brows | Quick synchronized flick | Two-tone descending "eh-eh" |
| **FURIOUS** | 🔴 blinking | Hard squint, V-brows, bared teeth | Rapid alternating flap x4 | Rising buzz sequence |
| **SULKING** | 🔴 slow blink | X eyes, resigned mouth | Raise & hold ("stop" pose) | One long low tone, then silence |

---

## Bill of Materials

| Component | Notes |
|---|---|
| ESP32 Dev Board | GPIO/PWM/SPI+I2C |
| TFT Display (ILI9341, 2.4"/2.8" SPI) | Face + insult text |
| IMU (MPU6050 I2C) | Detects tilt, shake, pickup, tap |
| Passive Buzzer | Emotional tones via PWM |
| 2× Micro Servo (SG90) | Arms — dismissive gestures |
| 3× LED + 220–330Ω resistors | Mood ring: green / yellow / red |
| 5V 2A+ external supply | Servos + display need more than USB |
| Breadboard / perfboard, wires | Build |
| 3D Printed Enclosure | `hateu_pod.scad` — see `/hateu_pod.scad` |

> ⚠️ **Power Note:** Servos need their own 5V rail. Share only common ground with the ESP32. Running servos off the ESP32 3.3V will brown-out the display.

---

## Pin Mapping

| Signal | GPIO |
|---|---|
| TFT SCK | 18 |
| TFT MOSI | 23 |
| TFT MISO | 19 |
| TFT CS | 5 |
| TFT DC | 2 |
| TFT RST | 4 |
| TFT Backlight | 15 |
| IMU SDA | 21 |
| IMU SCL | 22 |
| Servo Left Arm | 13 |
| Servo Right Arm | 12 |
| Buzzer | 27 |
| LED Calm (green) | 14 |
| LED Annoyed (yellow) | 16 |
| LED Furious (red) | 17 |

---

## IMU Interaction Detection

Reads at ~50 Hz, classifies events:

| Event | Detection | Mood Weight |
|---|---|---|
| **Tap / Knock** | High-pass acceleration spike | +1 |
| **Tilt** | Static angle > 15° from resting baseline | +1 |
| **Shake** | Rolling 8-sample variance spike | +2 |
| **Pickup** | Z-accel departs >0.3g for >500ms | +3 |

Picking it up is the fastest way to make it furious.

---

## Technical Details

### Software
- **Language:** C++ (Arduino framework for ESP32)
- **Libraries:**
  - `Adafruit_ILI9341` — TFT display
  - `Adafruit_GFX` — graphics primitives
  - `ESP32Servo` — servo PWM
  - `MPU6050` by Electronic Cats — IMU
  - `Wire`, `SPI` — built-in

### Hardware
- ESP32 DevKit (38-pin or similar)
- 2.4"/2.8" ILI9341 SPI TFT display
- MPU6050 IMU breakout
- 2× SG90 micro servo
- Passive buzzer module
- 3× 5mm LEDs (green, yellow, red) + resistors
- 5V 2A USB power supply with split rail

---

## Installation

```bash
# 1. Install Arduino IDE 2.x
# 2. Add ESP32 board package:
#    Boards Manager → search "esp32" → install by Espressif

# 3. Install libraries via Library Manager:
#    - Adafruit ILI9341
#    - Adafruit GFX Library
#    - ESP32Servo
#    - MPU6050 by Electronic Cats

# 4. Open firmware/hatebot/hatebot.ino
# 5. Select board: "ESP32 Dev Module"
# 6. Upload
```

---

## Calibration & Tuning

1. Open Serial Monitor at 115200 baud — it prints the IMU baseline on boot.
2. Let it sit still for the 1-second calibration window before touching it.
3. Tune thresholds in the `#define` block at the top of `hatebot.ino`:
   - `SHAKE_VAR_THRESH` — shake sensitivity
   - `TILT_DEG_THRESH` — tilt angle
   - `PICKUP_G_THRESH` — pickup sensitivity
   - `TIMEOUT_ANNOYED_CALM`, `TIMEOUT_FURIOUS_ANNOYED`, `TIMEOUT_SULK_CALM` — de-escalation speed
4. Confirm servo motion doesn't flicker the TFT (power rail separation check).

---

## Project Documentation

### Diagrams
![Workflow](Add your workflow/architecture diagram here)
*Mood state machine flow*

### Schematic & Circuit
![Circuit](Add your circuit diagram here)
*Pin connections to ESP32*

### Build Photos
![Components](Add photo of your components here)
*All components laid out*

![Build](Add photos of build process here)
*Assembly steps*

![Final](Add photo of final product here)
*Final assembled HateBot*

### Project Demo
[Add your demo video link here]

---

## Roadmap of Uselessness (optional extensions)

- [ ] Light sensor — complains about room lighting
- [ ] WiFi insult fetch — pull daily insults from a remote text file
- [ ] Microphone — react to being yelled at
- [ ] "Forgiveness" gesture — hold level for 10s → rare, begrudging compliment

---

## Team Contributions
- [Name 1]: [Specific contributions]
- [Name 2]: [Specific contributions]
- [Name 3]: [Specific contributions]

---
Made with ❤️ at TinkerHub Useless Projects

![Static Badge](https://img.shields.io/badge/TinkerHub-24?color=%23000000&link=https%3A%2F%2Fwww.tinkerhub.org%2F)
![Static Badge](https://img.shields.io/badge/UselessProjects--26-26?link=https%3A%2F%2Ftinkerhub.org%2Fevents%2F1M8ORET9A1%2Fuseless-projects-3.0)
