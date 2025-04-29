# Raspberry-Pi-Pico-CLI

**Pico CLI (Command Line Interpreter)**  
A lightweight and powerful command-line tool for the Raspberry Pi Pico, designed to simplify testing and interaction with electronic components.

---

## ⚡ Quick Start

1. Download and extract the ZIP archive.
2. Configure GPIO protection settings and the [ESP-AT UART connection](#-wifi-module-with-esp-at).
3. Install the Raspberry Pi Pico board support package  
   👉 [Install Pico board in Arduino IDE](https://docs.arduino.cc/hardware/raspberry-pi-pico/)
4. Flash the Pico with the default firmware settings.

---

## 📡 WiFi Module with ESP-AT

Flash the ESP-AT firmware to your module using the official guide:  
👉 [ESP-AT Firmware Documentation](https://docs.espressif.com/projects/esp-at/en/latest/esp32c3/)

Supported modules include:
- ESP8266  
- ESP32  
- ESP32-C2, ESP8684  
- ESP32-C3  
- ESP32-C6  
- ESP32-S2

Once the firmware is flashed, connect the module to the configured UART pins on the Pico.

To send AT commands via Pico's serial interface, use the `@` symbol as a prefix. Note that the ESP-AT module will not respond unless a host microcontroller is sending commands properly.

### 📘 ESP-AT Command Format

```bash
@ <your AT command>
@ AT          # Check connection/status
@ AT+GMR      # Check firmware version
@ AT+CWMODE?  # Query current WiFi mode
