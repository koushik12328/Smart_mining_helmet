# Smart Mining Helmet for Real-Time Hazard Detection
# 1. Aim of the Project
To design and implement a Smart Helmet safety monitoring system that integrates multiple environmental sensors with wireless communication. The project enables real-time data acquisition and hazard detection for toxic gases, temperature, and humidity conditions in industrial mining environments.  

# 2. Abstract
Mining is a hazardous occupation due to exposure to toxic gases and extreme conditions. This project presents a Smart Mining Helmet equipped with MQ-2, MQ-4, and MQ-135 sensors for gas detection and BME/BMP180 sensors for temperature and humidity. Data is processed by an ESP32 and transmitted wirelessly using the ESP-NOW protocol to generate immediate alerts, transforming passive equipment into an active safety system.  

# 3. Technical Methodology
# 3.1 Transmitter Unit (Smart Helmet)
* Microcontroller: ESP32.  

* Sensors: MQ-2, MQ-4, MQ-135 (Gases), DHT11 (Temperature & Humidity) and BME/BMP180 (Environment).
  
* 2 status LEDs and an active buzzer      

# 3.2 Receiver Unit (Base Station)
* Microcontroller: ESP32 configured as a receiver.  

* Display: I2C LCD for real-time monitoring.

* 2 status LEDs and an active buzzer  

# 3.3 Communication Protocol
Protocol: ESP-NOW (Enables low-latency peer-to-peer wireless communication without dependency on internet or routers, ideal for underground mines).  

# 4. Experimental Results and Thresholds
* Gas (MQ2): Safe at < 600 PPM; Danger triggered at > 600 PPM.
  * [following formula:
  \(D_{out}=\frac{V_{in}}{V_{max}}\times (2^{n}-1)\)
  is used to convert the 600ppm to the desired value with the help of ADCs and given to the code]

* Similarily for other gas sensors like MQ-4 and MQ-135 same method (as of 1st point) is used

* Temperature: Safe at < 45.0°C; Danger triggered at > 45.0°C.  

* Pressure: Danger triggered if pressure falls below 950 hPa.  

* System Response: Red LED ON, Buzzer 1kHz, and "DANGER" displayed on LCD.  

* Latency: End-to-end delay from sensor trigger to LCD update is less than 250ms.  

# 5. Implementation Status
* Hardware: Circuit implemented on breadboard and verified in Wokwi simulator initially and circuit was finally designed on the pcb using kicad.  

* Software: Embedded C code for sensor acquisition, threshold comparison, and ESP-NOW communication is completed and tested.
  * Tranmitter: Analog values are compared with predefined safety thresholds; alerts are triggered locally and transmitted wirelessly.
  * Receiver: Receives sensor data via ESP-NOW and triggers a buzzer or flashing "DANGER" message on LCD display if thresholds are breached.  

* Status: Prototype demonstrates reliable communication within a 100m range and is ready for pilot deployment.  

# 6. References
* S. Kumar et al., “IoT-Based Smart Helmet for Air Quality Monitoring,” IEEE I-SMAC, 2021.  

* M. Abdullah et al., “Development of Smart Safety Helmet for Construction Workers,” IEEE Access, 2020.  

* R. Patel and K. Sharma, “ESP-NOW Communication Protocol for Real-Time Industrial Applications,” IEEE ICCCNT, 2019.  

* NodeMCU ESP8266/ESP32 Documentation, Espressif Systems, 2023.
