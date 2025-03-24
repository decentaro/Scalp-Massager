# Scalp Massager (WIP)

A DIY scalp massager project using ESP32, designed to be remotely controlled via a Wi-Fi server. The project utilizes various components to control a 12V linear actuator, allowing smooth and adjustable massaging movements.

---

## 🛠️ Components Used

- **ESP32** - Microcontroller with built-in Wi-Fi support.
- **Bidirectional 5V to 3V Converter** - Voltage level shifter.
- **12V to 5V High/Low Trigger Relay Module** - Controls the linear actuator.
- **12V Linear Actuator** - Provides linear massaging movement.
- **Breadboard** - For prototyping and wiring.
- **12V Lithium Battery Rechargeable Pack** - Power source for the actuator.
- **Arduino IDE** - Used for programming the ESP32.

*Wiring diagrams and 3D models used will be provided soon.*

---

## 🌐 Features

- **Wi-Fi Server**: Allows remote control and configuration.
- **Relay Control**: Manages the actuator movement.
- **Rechargeable Battery**: Portable and convenient power supply.
- **Vibration (in progress)**: Adding vibration functionality for enhanced massage experience.

---

## ⚙️ Installation

1. Clone the repository:

git clone https://github.com/yourusername/scalp-massager.git cd scalp-massager

2. Open the project in Arduino IDE.
3. Select **ESP32** as the board.
4. Connect your ESP32 via USB.
5. Upload the code to the ESP32.
6. Monitor the Serial Console for the IP address and connection status.

---

## 🚀 Usage

1. Connect the device to the power source.
2. Access the Wi-Fi server by entering the IP address shown in the Serial Console.
3. To update SSID and password, connect the ESP32 to a PC and edit the sketch or `.ino` file.
4. Control the massager via the provided interface.

---

## 📝 Future Improvements

- Make the battery smaller and more efficient.
- Add a mobile app for easier control.
- Implement additional massage patterns.
- Finalize and integrate vibration functionality.

---
