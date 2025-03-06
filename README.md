# Smart Electric Fence Energizer

## Overview
The **Smart Electric Fence Energizer** is an **advanced IoT-based security solution** designed to enhance perimeter protection. Built using an **ESP32**, it integrates **GSM (SIM7600) and ESP-NOW communication** to provide **real-time monitoring and remote control** capabilities. This system ensures **continuous operation**, logs critical events, and offers seamless user control via **MQTT and a mobile interface**. It is ideal for securing farms, industrial areas, and private properties.

This project involves **firmware development using C++** for ESP32 and SIM7600, leveraging **FreeRTOS for multitasking** and **optimized power management techniques**. The backend communication is handled using **MQTT protocol**, ensuring secure and efficient data transfer between the hardware and the user interface.

## Key Features

| Feature | Description |
|---------|------------|
| **Remote Monitoring** | Monitor voltage, relay status, and GPS location from anywhere. |
| **GSM Connectivity** | Ensures reliable communication over cellular networks. |
| **ESP-NOW Communication** | Enables low-power wireless control between modules. |
| **SD Card Logging** | Stores event history for later analysis. |
| **MQTT Integration** | Secure remote control and data transmission. |
| **Dual Network Support** | Automatic fallback between WiFi and GSM for reliability. |
| **Signal Strength Monitoring** | Optimizes connectivity for seamless operation. |
| **Firmware Updates (OTA)** | Allows remote firmware updates without physical access. |

## Hardware Requirements

| Component | Quantity | Description |
|------------|---------|-------------|
| ESP32 Development Board | 1 | The core microcontroller for processing and communication. |
| SIM7600 GSM Module | 1 | Enables cellular data transmission and MQTT communication. |
| Relay Module | 1 | Controls the electric fence activation. |
| SD Card Module | 1 | Logs critical events for security tracking. |
| Power Supply (5V/12V) | 1 | Provides necessary power to the system. |
| Electric Fence System | 1 | The main security component. |
| Jumper Wires & PCB | - | For circuit connections and stability. |

## Wiring Diagram
*A detailed circuit diagram should be provided here for reference.*

## Installation & Setup
1. **Flash** the ESP32 with the provided firmware written in **C++ (Arduino Framework)**.
2. **Configure** WiFi and GSM settings in the firmware.
3. **Set up an MQTT broker** (e.g., Mosquitto, AWS IoT, etc.).
4. **Deploy hardware** at the electric fence site.
5. **Test connectivity** and ensure real-time updates are received.

## MQTT Configuration

| Parameter | Value |
|-----------|-------|
| **Broker** | `119.2.119.202` |
| **Port** | `1883` |
| **Publish Topic** | `Energizer/data` |
| **Subscribe Topic** | `Energizer/commands` |

## Coding & Software Development
- The firmware is written in **C++ using Arduino IDE**.
- Uses **FreeRTOS** for task scheduling and efficient power management.
- Implements **EEPROM storage** for saving essential configurations.
- Communicates over **MQTT** for cloud-based monitoring and control.
- Utilizes **GSM AT commands** to handle network connectivity and SMS alerts.
- The mobile interface is built with **Flutter**, providing a smooth user experience.

## Troubleshooting

| Issue | Solution |
|--------|----------|
| **Modem not restarting** | Check power supply and reset sequence. |
| **No network connection** | Ensure a valid SIM card with active data. |
| **SD Card not working** | Verify SD card is properly formatted and detected. |
| **No response from MQTT** | Check broker settings and internet connectivity. |

## Future Enhancements
- **Mobile App:** Develop a user-friendly application for real-time control.
- **Cloud Logging:** Store long-term data for enhanced analytics.
- **Automated Alerts:** Implement SMS/email notifications for security breaches.
- **AI-Based Anomaly Detection:** Use machine learning to detect unusual patterns.

## Author
Developed by **[Your Name]**

## License
MIT License

