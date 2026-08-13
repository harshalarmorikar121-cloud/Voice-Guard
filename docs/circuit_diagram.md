# Hardware Architecture & Wiring Diagram
## Smart Women's Safety Device (ESP32 + GPS + GSM + IoT)

---

## 1. System Architecture Overview

```
                      +-----------------------------+
                      |   Rechargeable Li-ion 3.7V  |
                      |   or 9V Battery Pack        |
                      +--------------+--------------+
                                     |
                         +-----------+-----------+
                         | Voltage Regulator /   |
                         | Buck Converter        |
                         +-----+-----------+-----+
                               |           |
             (3.3V/5V Stable)  |           | (4.0V @ 2A Peak)
                               v           v
                        +------------+   +------------+
                        |   ESP32    |   |  SIM800C/  |
                        | Dev Module |   |  SIM900A   |
                        +-----+------+   +-----+------+
                              |                |
         +--------------------+----------------+
         |                    |                |
         v                    v                v
  +--------------+    +---------------+  +------------+
  |  NEO-6M GPS  |    |  Panic Switch |  | Status LED |
  |  (UART2)     |    |  (GPIO 4)     |  | & Buzzer   |
  +--------------+    +---------------+  +------------+
```

---

## 2. Complete Pin Connection Table

| Component | Component Pin | ESP32 GPIO Pin | Description / Notes |
|---|---|---|---|
| **NEO-6M GPS** | VCC | 3.3V / 5V | Powers the GPS module |
| | GND | GND | Shared System Ground |
| | TX | **GPIO 16 (RX2)** | ESP32 receives NMEA data sentences |
| | RX | **GPIO 17 (TX2)** | ESP32 transmits commands (Optional) |
| **SIM800C / SIM900A GSM** | VCC | **Ext. 3.7V - 4.2V** | **DO NOT power from ESP32 pins!** Peak 2A required. |
| | GND | GND | **CRITICAL: Shared Ground with ESP32!** |
| | TX | **GPIO 26 (RX1)** | ESP32 receives GSM AT responses |
| | RX | **GPIO 27 (TX1)** | ESP32 sends AT commands (SMS & Calls) |
| **SOS Panic Button** | Terminal 1 | **GPIO 4** | Configured with `INPUT_PULLUP` |
| | Terminal 2 | GND | Triggers active-LOW signal when pressed |
| **Status Indicator LED**| Anode (+) | **GPIO 2** | Blue Onboard / External LED (via 220Ω resistor) |
| | Cathode (-) | GND | Ground |
| **Audio Buzzer** | Positive (+) | **GPIO 15** | Audio feedback when SOS pressed |
| | Negative (-) | GND | Ground |

---

## 3. Power Supply Engineering (Crucial Hackathon Tip)

> [!CAUTION]
> **Common Failure Point in IoT/GSM Projects:** 
> GSM modules (SIM800L / SIM800C / SIM900A) experience **sudden 2 Amp current spikes** when transmitting to cell towers. If you attempt to power the GSM module directly from the ESP32's 3.3V or 5V rail, the ESP32 will suffer from a **brown-out reset**, causing the microcontroller to crash during emergency transmission!

### Power Strategy Solution:
1. **Option A (3.7V Li-ion Battery + TP4056 + Buck Converter):**
   - Connect a 3.7V 18650 Li-ion battery to a **TP4056 charging module**.
   - Feed battery output to an **LM2596 or XL6009 Buck/Boost converter** tuned to **4.0V** for the GSM module.
   - Use a 5V step-up rail for the ESP32 VIN pin.
2. **Option B (9V Battery + Dual AMS1117 Regulators):**
   - Step down 9V to 5V (for ESP32) and 4.0V (for GSM module).
   - Add a 1000µF electrolytic capacitor across GSM `VCC` and `GND` pins to buffer peak transmission current spikes.

---

## 4. Hardware Verification & Debugging Checklist

- [x] **Common Ground:** Are GND pins of ESP32, GSM, GPS, and Power Modules linked together?
- [x] **Baud Rate Matching:** GPS default is `9600` baud. GSM default is `9600` baud.
- [x] **SIM Card Setup:** Insert a 2G/3G/4G compatible SIM card (with 2G fallback) that has SIM PIN disabled, valid SMS pack, and active voice call balance.
- [x] **GPS Clear Sky Access:** Antenna must have direct line of sight to outdoor sky for fast initial satellite lock (Cold start takes ~30 to 60 seconds).
