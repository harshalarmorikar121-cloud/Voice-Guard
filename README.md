# 🛡️ Voice-Guard — Standalone IoT Women Safety & Guardian Response Ecosystem

[![Hackathon](https://img.shields.io/badge/Hackathon-AI4SDG%20Global%20Hackathon%202026-blueviolet.svg)](https://github.com)
[![Theme](https://img.shields.io/badge/Theme-AI%20for%20Sustainable%20Development%20Goals-blue.svg)](https://github.com)
[![Problem Statement](https://img.shields.io/badge/Problem%20Statement-PS--I03-orange.svg)](https://github.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32%20%7C%20SIM800L%20%7C%20GPS-red.svg)](https://github.com)
[![Mobile](https://img.shields.io/badge/Mobile-React%20Native%20%7C%20Expo-cyan.svg)](https://github.com)

> **Voice-Guard** is an offline-first IoT women's safety ecosystem. It pairs an independent hardware panic device (GSM/GPS cellular alerts) with a WebUSB flash configurator and an Android Guardian App that overrides silent mode to display live emergency telemetry and dispatch authorities.

---

## 📌 Problem Statement (PS-I03)
In acute panic scenarios (stalking, assault, transit diversion), women frequently cannot unlock a smartphone, navigate touchscreen safety apps, or maintain a reliable Wi-Fi/4G data connection. **Voice-Guard** solves this by separating the trigger from the smartphone—providing a tactile, one-touch physical panic device that communicates directly through basic cellular networks (2G/4G/5G SMS).

---

## 🏗️ System Architecture

```text
 ┌─────────────────────────────────────────────────────────┐
 │               VOICE-GUARD HARDWARE UNIT                 │
 │  ESP32 Microcontroller + SIM800L GSM + Neo-6M GPS       │
 └────────────────────────────┬────────────────────────────┘
                              │ (Direct Cellular SMS)
                              ▼
 ┌─────────────────────────────────────────────────────────┐
 │               GUARDIAN ANDROID APPLICATION              │
 │   • Intercepts [VOICE-GUARD SOS] formatted SMS          │
 │   • Overrides phone Silent / DND mode with Siren        │
 │   • Plots victim's live coordinates on Google Map       │
 │   • 1-Tap Police Dispatch Trigger                       │
 └─────────────────────────────────────────────────────────┘
                              ▲
                              │ (USB Serial / WebUSB API)
 ┌────────────────────────────┴────────────────────────────┐
 │               WEBUSB CONFIGURATOR PORTAL                │
 │   • Plug-and-play browser serial connection             │
 │   • OTP-verified guardian number provisioning           │
 │   • Permanent NVS Flash memory storage                  │
 └─────────────────────────────────────────────────────────┘
```

---

## 🌟 Key Features

1. **100% Wi-Fi & Smartphone Independent:** Hardware communicates directly with cellular towers without requiring a smartphone, Bluetooth, or local Wi-Fi.
2. **One-Touch Tactile Trigger:** A discrete physical button sends high-priority distress SMS messages in under 3 seconds.
3. **WebUSB Flash Provisioning:** Users can configure emergency guardian contacts from any Chromium browser (Chrome/Edge) with zero driver installation.
4. **OTP Verification Security:** Generates and validates SMS OTPs prior to burning numbers into ESP32 non-volatile memory.
5. **Guardian Companion App:** React Native / Expo application that sounds an emergency alarm and loads interactive map tracking upon SMS reception.
6. **Ultra Low-Power Deep Sleep:** Optimized power management ensuring up to multiple weeks of standby battery life on a single charge.

---

## 🛠️ Technology Stack

| Domain | Technologies |
|---|---|
| **Firmware & Microcontroller** | ESP32 Dev Board, C++ / Arduino Core, FreeRTOS |
| **Cellular & Telemetry** | SIM800L GSM/GPRS, AT Commands, Neo-6M GPS (NMEA) |
| **Web Provisioning Portal** | HTML5, CSS3 Glassmorphism, JavaScript, Web Serial API |
| **Mobile App (Guardian)** | React Native, Expo, TypeScript, React Native Maps, Expo-AV |
| **Power Management** | TP4056 USB Charger, 3.7V 800mAh Li-Po Battery |

---

## 📂 Repository Structure

```text
voice-guard/
├── firmware/                   # ESP32 C++ / Arduino firmware source code
│   ├── safety_device.ino       # Main firmware loop and state machine
│   ├── config.h / config.cpp   # Pin definitions & contact configuration
│   ├── gps.h / gps.cpp         # GPS NMEA parsing & coordinates
│   ├── gsm.h / gsm.cpp         # SIM800L AT Command driver & SMS dispatch
│   └── thingspeak.h / .cpp     # Optional cloud telemetry logging
├── web-configurator/           # Web Serial USB Provisioning Portal
│   ├── index.html              # Modern glassmorphism UI dashboard
│   ├── style.css               # Dynamic animations & theme styling
│   ├── configurator.js         # Web Serial API & OTP handling logic
│   └── manifest.json           # PWA installation manifest
├── mobile-app/                 # React Native / Expo Guardian Companion App
│   ├── package.json            # Dependencies & scripts
│   ├── app.json                # Expo configuration
│   └── src/app/index.tsx       # Live map view, siren trigger & SMS handler
└── docs/                       # Hackathon Pitch & Engineering Docs
    ├── circuit_diagram.md      # Pin-to-pin wiring schematics
    ├── cost_analysis_bom.md    # Component pricing & production breakdown
    ├── step_by_step_wiring_guide.md # Assembly guide
    ├── Voice-Guard_Abstract.docx    # Formal project abstract
    └── Voice-Guard_Hackathon_Final.pptx # 6-Slide official pitch deck
```

---

## ⚡ Quick Start & Installation

### 1. ESP32 Firmware Setup
1. Open Arduino IDE or VS Code with PlatformIO.
2. Select Board: **ESP32 Dev Module**.
3. Install required libraries:
   * `TinyGPSPlus`
4. Connect ESP32 via USB and flash `firmware/safety_device.ino`.

### 2. WebUSB Configurator Portal
1. Navigate to the `web-configurator/` directory.
2. Open `index.html` in **Google Chrome** or **Microsoft Edge**.
3. Connect the Voice-Guard hardware via USB and click **"Pair & Connect USB Device"**.
4. Enter emergency contacts, verify via OTP, and flash the configuration.

### 3. Mobile Guardian App (Expo)
```bash
cd mobile-app
npm install
npm start
```
*Open the **Expo Go** app on your Android device and scan the generated QR code.*

---

## 💰 Bill of Materials (BOM) & Costing

| Component | Quantity | Single Prototype Cost | Bulk Production Cost (100+ units) |
|---|---|---|---|
| ESP32 Dev Board | 1 | ₹330 - ₹500 | ~₹200 ($2.50) |
| SIM800L GSM Module | 1 | ₹290 - ₹415 | ~₹165 ($2.00) |
| 800mAh Li-Po Battery | 1 | ₹250 - ₹415 | ~₹125 ($1.50) |
| TP4056 Charging Module | 1 | ₹80 | ~₹20 ($0.25) |
| Tactile Push Button | 1 | ₹15 | ~₹2 ($0.02) |
| Active Buzzer | 1 | ₹40 | ~₹8 ($0.10) |
| Custom 3D Printed Case & PCB | 1 | ₹245 | ~₹105 ($1.30) |
| **Total Hardware Cost** | | **~₹1,250 - ₹1,890** | **~₹625 ($7.50 USD)** |

---

## 🎯 Sustainable Development Goals (SDGs)
* **SDG 5:** Gender Equality (Ending violence and harassment against women)
* **SDG 11:** Sustainable Cities and Communities (Safe urban and transit mobility)
* **SDG 16:** Peace, Justice, and Strong Institutions (Rapid emergency intervention)

---

## 👥 Contributors & Hackathon Team
* **Team Leader:** [Your Name] — Firmware, IoT & Hardware Design
* **Team Member 2:** [Name] — Mobile App Development (React Native)
* **Team Member 3:** [Name] — Web Frontend & USB Serial Protocols
* **Team Member 4:** [Name] — Circuit Design & Testing

---

## 📜 License
This project is open source and available under the [MIT License](LICENSE).
