# IoT-Based Smart Waste Management System Using ESP32

## 📌 Overview

The IoT-Based Smart Waste Management System is an ESP32-powered smart bin prototype designed to monitor the amount of waste inside a bin in real time.

An HC-SR04 ultrasonic sensor measures the distance between the top of the bin and the waste. The ESP32 converts this distance into a waste-fill percentage and displays the result on an OLED display.

When the waste level reaches a predefined threshold, the system activates a buzzer to indicate that the bin needs attention.

The ESP32 also hosts a lightweight web dashboard that allows the waste level and bin status to be monitored remotely over the local Wi-Fi network.

---

## 🎯 Objectives

- Monitor waste level automatically
- Calculate the approximate percentage of bin capacity used
- Display waste information on an OLED display
- Generate an alert when the bin is nearly full
- Provide remote monitoring using ESP32 Wi-Fi
- Demonstrate a practical IoT application

---

## ✨ Features

- Real-time waste level monitoring
- HC-SR04 ultrasonic distance measurement
- ESP32-based processing
- OLED status display
- Automatic full-bin alert
- Buzzer notification
- Wi-Fi connectivity
- Web-based monitoring dashboard
- Automatic dashboard refresh

---

## 🛠️ Components Required

| Component | Quantity | Purpose |
|---|---:|---|
| ESP32 Development Board | 1 | Main controller |
| HC-SR04 Ultrasonic Sensor | 1 | Waste level measurement |
| 0.96" OLED SSD1306 | 1 | Local display |
| Buzzer Module | 1 | Full-bin alert |
| Breadboard | 1 | Circuit prototyping |
| Jumper Wires | As required | Connections |
| USB Cable | 1 | Programming and power |
| Mini Bin / Container | 1 | Prototype body |

---

## 🔌 Pin Connections

### HC-SR04

| HC-SR04 Pin | ESP32 Pin |
|---|---|
| VCC | VIN / 5V |
| GND | GND |
| TRIG | GPIO 5 |
| ECHO | GPIO 18* |

### OLED SSD1306

| OLED Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Buzzer

| Buzzer Pin | ESP32 Pin |
|---|---|
| + | GPIO 27 |
| GND | GND |

> **Important:** The HC-SR04 ECHO output can be 5V. ESP32 GPIO pins are 3.3V logic. Use a suitable voltage divider or level shifter between ECHO and GPIO 18 when using the sensor with a physical ESP32.

---

## ⚙️ Working Principle

The system works in the following sequence:

1. The HC-SR04 ultrasonic sensor sends an ultrasonic pulse.
2. The sensor receives the reflected echo from the waste surface.
3. ESP32 calculates the distance using the echo time.
4. The distance is converted into an approximate waste-fill percentage.
5. The OLED displays the waste level and bin status.
6. If the waste level reaches 80% or more, the buzzer is activated.
7. ESP32 connects to Wi-Fi and hosts a web dashboard.
8. Users connected to the same Wi-Fi network can monitor the bin remotely.

---

## 📊 Waste Level Status

| Waste Level | Status |
|---:|---|
| 0–19% | EMPTY |
| 20–49% | FILLING |
| 50–79% | HALF FULL |
| 80–100% | FULL |

The full-bin threshold can be changed in the Arduino code.

---

## 🌐 Web Dashboard

After the ESP32 connects to Wi-Fi, the IP address is displayed in the Serial Monitor.

Example:

```text
Wi-Fi connected!
IP Address: 192.168.1.105
Web server started!
