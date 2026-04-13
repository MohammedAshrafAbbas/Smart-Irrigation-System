# Smart Irrigation System (esp32 & AI Integration) 🌿💧

This repository contains a comprehensive **Smart Irrigation System** designed to optimize water usage in agriculture. The system leverages the **esp32** microcontroller for real-time monitoring and uses a **Python-based AI model** to make intelligent irrigation decisions.

---

## 🛠 Features
* **Automated Monitoring:** Real-time data collection from soil moisture, temperature, and humidity sensors.
* **AI Decision Making:** A machine learning model (`lap2.py`) that analyzes environmental data to predict the precise amount of water needed.
* **Embedded Control:** Efficient C-based firmware for esp32 to manage hardware components (Relays, LCD, Sensors).
* **Power Efficiency:** Optimized for low power consumption, suitable for remote farming areas.

---

## 📂 Project Structure
* **`Arduino Code/`**: Contains the source code for the esp32 microcontroller.
* **`lap2.py`**: The Python script responsible for the AI/Machine Learning logic and data processing.
* **`Images/`**: Hardware circuit diagrams and project snapshots.
* **`LICENSE`**: MIT License.

---

## 🏗 Hardware Components
* **Microcontroller:** esp32
* **Sensors:** Soil Moisture Sensor, DHT11 (Temperature & Humidity)
* **Actuators:** DC Water Pump, 5V Relay Module
* **Display:** 16x2 LCD Display
* **Power Supply:** 12V Adapter or Battery pack

---

## 🚀 Getting Started

### 1. Hardware Setup
Connect the sensors to the ATmega32 pins as specified in the circuit diagram located in the `/Images` folder.

### 2. Microcontroller Firmware
1. Open the code in `Arduino Code/` using your preferred IDE (Microchip Studio or Arduino IDE with ATmega32 core).
2. Compile and burn the `.hex` file to the ATmega32.

### 3. AI Model Setup
Ensure you have Python installed, then install the necessary dependencies:
```bash
pip install numpy pandas scikit-learn
