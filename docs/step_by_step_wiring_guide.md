# Step-by-Step Hardware Assembly & Wiring Blueprint
## How to Connect & Run the Smart Women's Safety Device (ESP32 + GPS + GSM)

---

## 🛠️ Required Parts List Before Assembly

- [x] **ESP32 Dev Board** (30-pin version)
- [x] **NEO-6M GPS Module** (with ceramic antenna attached)
- [x] **SIM800C / SIM900A / SIM800L GSM Module** (with SIM card inserted)
- [x] **Tactile Push Button** (SOS Switch)
- [x] **5V Active Buzzer**
- [x] **5mm Blue/Red LED + 220Ω Resistor**
- [x] **Breadboard & Male-to-Male / Male-to-Female Jumper Wires**
- [x] **Power Source:** 3.7V 18650 Li-ion battery + TP4056 + Buck Converter tuned to **4.0V** (or 5V 2A USB power adapter with common ground)

---

## 🔌 Complete Wire-by-Wire Joining Instructions

### **STEP 1: Establish the Common Power & Ground Rails on Breadboard**

> [!IMPORTANT]
> **Rule #1 of Embedded Electronics:** All ground (`GND`) pins MUST be joined together. Without a shared common ground, communication between the ESP32, GPS, and GSM modules will fail!

1. Insert the **ESP32 Dev Module** onto the breadboard straddling the center divider.
2. Run a jumper wire from the **ESP32 GND pin** to the **Blue (-) Rail** of the breadboard.
3. Run a jumper wire from the **ESP32 5V (or VIN) pin** to the **Red (+) Rail** of the breadboard.

---

### **STEP 2: Connect the NEO-6M GPS Module**

The GPS module communicates with the ESP32 via **Hardware Serial UART2**.

| NEO-6M GPS Pin | Wire Color Recommendation | Connect To (ESP32 / Rail) | Notes |
|---|---|---|---|
| **VCC** | Red Wire | **Red (+) 5V / 3.3V Rail** | Powers the GPS module |
| **GND** | Black Wire | **Blue (-) GND Rail** | Ground connection |
| **TX** | Green Wire | **ESP32 GPIO 16 (RX2)** | GPS transmits data sentence to ESP32 |
| **RX** | Yellow Wire | **ESP32 GPIO 17 (TX2)** | ESP32 sends commands to GPS |

---

### **STEP 3: Connect the SIM800C / SIM900A GSM Module (Power Critical!)**

The GSM module communicates with the ESP32 via **Hardware Serial UART1**.

> [!CAUTION]
> **Power Spike Warning:** DO NOT power the GSM module directly from the ESP32 3.3V pin! Connect GSM `VCC` to your 3.7V–4.2V external battery rail or buck converter capable of **2 Amps output**.

| GSM Module Pin | Wire Color Recommendation | Connect To (Target) | Notes |
|---|---|---|---|
| **VCC** | Thick Red Wire | **Ext. Power 4.0V (+) Rail** | Needs up to 2A current spikes |
| **GND** | Thick Black Wire | **Blue (-) Common GND Rail** | **MUST be connected to ESP32 GND!** |
| **TX** | Blue Wire | **ESP32 GPIO 26 (RX1)** | GSM module sends AT responses to ESP32 |
| **RX** | White Wire | **ESP32 GPIO 27 (TX1)** | ESP32 sends AT commands to GSM module |

---

### **STEP 4: Connect the SOS Panic Push Switch**

The SOS switch uses the ESP32 internal pull-up resistor (`INPUT_PULLUP`).

1. Place the 2-pin tactile push button across the breadboard divider.
2. Connect **Leg 1** of the button to **ESP32 GPIO 4**.
3. Connect **Leg 2** of the button to the **Blue (-) Common GND Rail**.

---

### **STEP 5: Connect the Feedback LED & Buzzer**

#### **A. Active Buzzer Setup:**
1. Connect the **Positive (+) Long Pin** of the Buzzer to **ESP32 GPIO 15**.
2. Connect the **Negative (-) Short Pin** of the Buzzer to the **Blue (-) Common GND Rail**.

#### **B. Status LED Setup:**
1. Connect **ESP32 GPIO 2** to one leg of a **220Ω Resistor**.
2. Connect the other leg of the resistor to the **Long Leg (Anode +)** of the LED.
3. Connect the **Short Leg (Cathode -)** of the LED to the **Blue (-) Common GND Rail**.

---

## 🗺️ Master Wiring Schematic (ASCII Diagram)

```
                          +-------------------------+
                          |   ESP32 Dev Module      |
                          |                         |
                          |  [GPIO 16 (RX2)] <------+------ NEO-6M GPS TX
                          |  [GPIO 17 (TX2)] ------+------> NEO-6M GPS RX
                          |  [GPIO 26 (RX1)] <------+------ SIM800 GSM TX
                          |  [GPIO 27 (TX1)] ------+------> SIM800 GSM RX
                          |                         |
                          |  [GPIO 4]  ------------+------> SOS Switch Leg 1 (Leg 2 -> GND)
                          |  [GPIO 15] ------------+------> Buzzer (+)
                          |  [GPIO 2]  ------------+------> Resistor -> LED (+)
                          |                         |
                          |  [VIN / 5V] -----------> Breadboard (+) 5V Rail
                          |  [GND]     -----------> Breadboard (-) GND Rail
                          +------------+------------+
                                       |
                   +-------------------+-------------------+
                   |                                       |
                   v                                       v
         +-------------------+                   +-------------------+
         | NEO-6M GPS Module |                   | GSM Module        |
         | VCC -> 5V Rail    |                   | VCC -> 4.0V (2A)  |
         | GND -> GND Rail   |                   | GND -> GND Rail   |
         +-------------------+                   +-------------------+
```

---

## 🚀 Step-by-Step Instructions to Flash & Run

1. **Insert SIM Card:** Insert a 2G/3G/4G valid SIM card into the GSM slot (make sure SIM PIN is disabled and SIM has voice/SMS balance).
2. **Connect ESP32 to PC:** Plug the ESP32 into your computer via a Micro-USB or Type-C data cable.
3. **Open Arduino IDE:**
   - Open `safety_device.ino`.
   - Go to `Tools > Board > ESP32 Dev Module`.
   - Go to `Tools > Port` and select your ESP32 COM Port (e.g., `COM3` or `COM5`).
4. **Update Config Parameters:**
   - Edit your phone number: `const char* EMERGENCY_NUMBERS[] = {"+91XXXXXXXXXX"};`
   - Edit Wi-Fi credentials: `WIFI_SSID` & `WIFI_PASSWORD`.
5. **Upload Firmware:** Click the **Upload** arrow button in Arduino IDE.
6. **Open Serial Monitor:** Set baud rate to **115200 baud**.
7. **Test SOS Trigger:** Press the panic button. Watch the buzzer beep, the LED glow, the Serial Monitor log GPS satellite fixes, the emergency SMS arrive on your target phone, and the incoming call ring!
