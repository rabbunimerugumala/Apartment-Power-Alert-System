# ⚡ Apartment Power Alert System

> A Smart System to Detect Power Availability and Alert Users to Turn Off or Turn On The Backup Generators — Save Fuel, Stay Notified.



## 🧠 Project Motivation

In many apartments, **diesel generators** are used as backup during power cuts. But often, there are **two key problems**:

1. The **watchman or residents don't realize when the power goes off**, leading to **delays in turning on the generator**, causing inconvenience.
2. Once the **main supply returns**, they often **fail to notice it**, so the **generator keeps running unnecessarily** — wasting **fuel and money**.

This system provides a **visual, audible, and real-time alert** for both situations:

* **When power goes OFF**, it alerts users to **start the generator.**
* **When power is restored**, it prompts users to **turn off the generator immediately.**

This ensures **fuel conservation**, reduces generator wear-and-tear, and improves overall **power management** in the apartment.


## 🚀 Features

* 🔌 Detects **main voltage** using ZMPT101B
* ⚡ Detects **current load** using ZMCT103C
* 🔊 **Buzzer alert** on power restoration (1-minute duration)
* 🟢 **Green LED**: Power is available
* 🟡 **Yellow LED**: Power is lost
* 🖥️ Real-time **voltage & current display** on 16x2 LCD
* 🔘 Manual **buzzer stop button** for quick silencing
* 🧠 Debounce logic prevents false alarms due to voltage spikes



## 🧰 Components Required

| Component             | Specs / Type  | Quantity |
| --------------------- | ------------- | -------- |
| Arduino Uno           | ATmega328P    | 1        |
| Voltage Sensor Module | ZMPT101B      | 1        |
| Current Sensor Module | ZMCT103C      | 1        |
| LCD Display (16x2)    | I2C (0x27)    | 1        |
| Buzzer                | 5V Piezo      | 1        |
| Green LED             | 5mm           | 1        |
| Yellow LED            | 5mm           | 1        |
| Push Button           | Normally Open | 1        |



## 🔧 Circuit Overview

> **Main Power IN ➜ Voltage Sensor ➜ Arduino**
>
> **Current Sensor ➜ Arduino**
>
> **Arduino ➜ LCD + LEDs + Buzzer + Button**

* Voltage sensed via **analog pin A0**
* Current sensed via **analog pin A1**
* Buzzer on **pin 7**
* LEDs on **pin 8 (YELLOW), pin 9 (GREEN)**
* Button on **pin 6 (INPUT\_PULLUP)**


## 📐 Working Logic

| State              | Voltage | Action                                                                 |
| -------------------| ------- | -----------------------------------------------------------------------|
| 🔌 Power ON       | > 90V   | Green LED ON, LCD shows voltage & current, buzzer beeps for 1 minute    |
| ❌ Power OFF      | < 90V   | Yellow LED ON, LCD says "Power OFF", buzzer remains OFF                 |
| 🔘 Buzzer Stop    | Pressed | Stops any buzzer sound instantly                                        |

> ℹ️ Uses 5-second debounce time to confirm consistent power loss before triggering alert.

### 🔊 Buzzer Sound Pattern

* When **main power is restored**, the buzzer gives a **short intermittent beep** for 1 minute to draw attention.
* When **main power goes OFF**, the buzzer gives a **continuous sound** to urgently alert the user to switch on the generator.
* The buzzer can be **manually silenced** anytime by pressing the stop button.


## 🖥️ LCD Display Format

V: 230.4V
I:  0.21A
