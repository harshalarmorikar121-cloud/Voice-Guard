# Bill of Materials (BOM) & Project Cost Analysis
## Smart Women's Safety & Emergency Response Device (ESP32 + GPS + GSM + IoT)

---

## 💵 1. Itemized Component Cost Breakdown (Prototype Scale)

| # | Component Name | Model / Specification | Quantity | Approx. Cost (INR ₹) | Approx. Cost (USD $) | Sourcing / Buying Note |
|---|---|---|---|---|---|---|
| **1** | **ESP32 Microcontroller** | ESP32-WROOM-32 (30-pin Dev Module) | 1 | ₹380 | $4.60 | Central controller with built-in Wi-Fi & Bluetooth |
| **2** | **GPS Module** | NEO-6M GPS Module with ceramic antenna | 1 | ₹350 | $4.20 | Fetches satellite coordinates via UART2 |
| **3** | **GSM / GPRS Module** | SIM800C or SIM800L Quad-band module | 1 | ₹320 | $3.90 | Handles emergency SMS dispatch & voice calls |
| **4** | **SOS Panic Switch** | Tactile Push Button / Wearable Switch | 1 | ₹15 | $0.20 | Momentary active-LOW trigger |
| **5** | **Rechargeable Battery** | 3.7V 1200mAh 18650 Li-ion / Li-Po Cell | 1 | ₹150 | $1.80 | High-capacity power source |
| **6** | **Battery Charging Board**| TP4056 Type-C Charger with protection | 1 | ₹30 | $0.35 | Safe USB charging & overcharge protection |
| **7** | **Voltage Regulator** | MT3608 Boost / AMS1117 3.3V/5V module | 1 | ₹40 | $0.50 | Maintains stable 4.0V for GSM 2A current spikes |
| **8** | **Alert Buzzer & LED** | 5V Active Buzzer + 5mm LED + Resistors | 1 | ₹20 | $0.25 | Haptic audio-visual feedback |
| **9** | **Prototyping Accessories**| Half Breadboard + Jumper Wires + Box | 1 | ₹100 | $1.20 | Circuit wiring & housing enclosure |
| **10**| **GSM SIM Card** | Prepaid 2G/3G/4G SIM Card | 1 | ₹80 | $0.95 | For SMS & voice call balance |
| **TOTAL** | **Complete Prototype Cost** | — | — | **₹1,485** | **~$18.00** | *(Range: ₹1,300 to ₹1,600)* |

---

## 🏬 2. Best Sourcing Platforms for Indian Hackathon Students

To get components at wholesale student prices:
- **Online Vendors:** [Robu.in](https://robu.in), [Robocraze](https://robocraze.com), [Evelta](https://evelta.com), [Quartz Components](https://quartzcomponents.com).
- **Offline Electronics Hubs (Pune/Mumbai):**
  - **Pune:** Appa Balwant Chowk (ABC) / Budhwar Peth electronics markets near PCCOE campus.
  - **Mumbai:** Lamington Road electronics wholesale market.

---

## 📊 3. Prototype vs Commercial Product Cost Comparison

| Feature / Metric | Commercial Safety Wearable (e.g. Smart Rings/Jewelry) | Our SafeShield IoT Prototype |
|---|---|---|
| **Purchase Price** | ₹4,000 – ₹8,000 ($50 – $100) | **₹1,485 (~$18)** |
| **Monthly Subscription Fee**| ₹200 – ₹500 / month | **₹0 (Zero recurring fees)** |
| **Smartphone Dependency** | 100% (App must run in background) | **0% (100% Autonomous GSM/GPS)** |
| **Dual Alert Network** | Bluetooth App only | **Cellular SMS/Call + Cloud IoT** |

---

## 🏭 4. Mass Production Unit Cost Estimation (1,000+ Units Scale)

When transitioning from breadboard prototype to factory SMD manufacturing:

- **Custom 2-Layer PCB:** ₹45 / unit
- **Surface Mount (SMD) ESP32-PICO-D4 Chip:** ₹160 / unit
- **SMD SIM800C & NEO-6M Modules:** ₹380 / unit
- **Custom Molded Plastic/Silicone Enclosure:** ₹60 / unit
- **Estimated Mass Production Cost:** **₹645 ($7.80) per unit** *(Over 55% cost reduction at scale!)*
