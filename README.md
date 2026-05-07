# Smart Mining Helmet for Real Time Hazard Detection
**Developed by:** Ch Koushik | MGIT ECE

---

## 1. Aim
To design and develop a decentralized, infrastructure-less safety system for underground mining that utilizes **Machine-to-Machine (M2M)** communication to provide real-time environmental monitoring and instant hazard alerts without relying on Wi-Fi or cellular networks.

## 2. Abstract
Underground mining environments present a unique challenge for safety communication due to signal attenuation through rock strata and the high cost of network infrastructure. This project proposes a **Smart Mining Helmet** that functions as an autonomous node. By leveraging the **ESP-NOW** protocol, the helmet (Transmitter) communicates directly with a base station (Receiver) via hardware MAC addresses. 

The system integrates a multi-sensor array—including Methane (MQ-4), smoke (MQ-2), air quality (MQ-135), and environmental sensors (DHT11, BMP180)—to monitor for life-threatening conditions. Hardware design was executed in **KiCad** for optimized signal integrity, while the software architecture employs a non-blocking state machine with digital filtering to ensure data reliability. Results demonstrate ultra-low latency (<10ms) and robust connectivity, proving the system's efficacy for real-time industrial safety applications.

---

## 3. Components Required

### A. Smart Helmet (Transmitter Module)
* **ESP32 DevKit V1:** Core microcontroller utilizing the ESP-NOW protocol for data broadcast.
* **MQ-4 Sensor:** Dedicated for Methane (CH4) gas detection.
* **MQ-2 Sensor:** Detects Smoke, LPG, and Carbon Monoxide.
* **MQ-135 Sensor:** Monitors general Air Quality (NH3, Benzene, etc.).
* **DHT11:** Provides Temperature and Humidity data.
* **BMP180:** High-precision sensor for Barometric Pressure and Altitude monitoring.
* **Status LEDs:** Local indicators for power and transmission verification.
* **Custom Transmitter PCB:** Designed in KiCad with a compact form factor for helmet mounting.

### B. Base Station / Node (Receiver Module)
* **ESP32 DevKit V1:** Configured to listen for specific MAC addresses and process incoming data packets.
* **16x2 LCD with I2C Module:** Displays real-time sensor telemetry from the helmet.
* **Active Buzzer (5V):** Sounds a high-decibel alarm when sensor thresholds are breached.
* **Visual Alarm LED:** High-intensity LED for immediate hazard recognition.
* **Custom Receiver PCB:** Designed in KiCad with an integrated I2C header and centralized power distribution.

---

## 4. Procedure

### A. Hardware Analysis
The hardware design focuses on **Signal Integrity** and **Power Management**. 
* **Sensing Layer:** The gas sensors (MQ series) are analog; their output is fed into the ESP32’s 12-bit ADC. I implemented a common ground plane in KiCad to reduce noise during high-frequency Wi-Fi bursts.
* **PCB Strategy:** The Transmitter PCB is designed for a compact footprint to fit the helmet. The Receiver PCB utilizes the **I2C protocol** via the PCF8574 expander, which allowed me to reduce the LCD wiring from 10+ pins down to just 2 (SDA/SCL), simplifying the routing significantly.
* **Power Logic:** Because the MQ sensors have internal heaters that pull high current (~800mA peak), I ensured the power traces on the PCB were wide enough to prevent voltage drops that could crash the ESP32.

### B. Software Analysis
The software is the "intelligence" of the system, acting as a non-blocking state machine.
* **M2M Protocol (ESP-NOW):** Unlike standard Wi-Fi, the firmware bypasses the router. I hardcoded the Receiver's hardware MAC address into the Transmitter’s peer list. This allows for near-instant "Power-On-to-Data" transmission.
* **Data Structuring:** I defined a custom `struct` to package sensor values. This ensures that the Receiver interprets the incoming raw bytes correctly every time.
* **Signal Processing:** To handle the inherent "jitter" of analog gas sensors, I implemented a **Moving Average Filter** in the C++ code. This smooths out the readings before they are sent over the air.
* **Callback & ACK:** The software uses the `OnDataSent` callback. If the Receiver is out of range, the code detects the lack of an Acknowledgement (ACK) and triggers a "Link Fail" state without stopping the sensor polling loop.

---

## 5. Implementation
1. **PCB Fabrication:** The KiCad layouts were exported to Gerbers for fabrication.
2. **Firmware Upload:** The TX module was flashed with the sensor-gathering and ESP-NOW broadcast code. The RX module was flashed with the display and alert logic.
3. **M2M Pairing:** MAC addresses were verified and hardcoded for a dedicated point-to-point link.

---

## 6. Observations
* **Latency:** The "Sensor-to-Alert" delay was measured at approximately **10ms**, significantly faster than any Cloud-based IoT setup.
* **I2C Efficiency:** The I2C bus remained stable at 100kHz, providing clean data updates to the LCD every 500ms.
* **Range:** The ESP-NOW link remained robust even through moderate industrial interference, reaching up to 50 meters in non-line-of-sight conditions.

---

## 7. Results
* **Successful M2M Link:** Established a permanent MAC-to-MAC connection that requires zero external network infrastructure.
* **Hazard Response:** The system successfully triggered the buzzer within milliseconds of exposing the MQ-4 sensor to a methane-simulated environment.
* **Data Accuracy:** The moving average filter effectively removed ADC noise, providing stable, reliable PPM readings on the LCD.

---

## 8. Conclusion
The project proves that for high-risk environments like mines, **Machine-to-Machine (M2M)** communication is far superior to standard IoT. By combining custom KiCad hardware with a lean, non-blocking software stack, I created a safety system that is faster, more reliable, and completely independent of the internet.

---
*Department of Electronics and Communication Engineering | MGIT, Hyderabad*
