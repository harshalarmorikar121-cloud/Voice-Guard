/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - CONFIG
 *      Author  : Your Name
 *      Version : 2.0 (Debugged)
 *      Board   : ESP32 Dev Module
 *****************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ─────────────────────── WiFi ────────────────────────
// FIX #1 : Variables declared as 'extern' here.
//           Actual definitions live in config.cpp.
//           Defining non-const variables directly in a
//           header causes "multiple definition" linker
//           errors when the header is included in more
//           than one .cpp translation unit.
extern const char* WIFI_NAME;
extern const char* WIFI_PASSWORD;

// ──────────────────── ThingSpeak ─────────────────────
#define CHANNEL_NUMBER   1234567
#define WRITE_API_KEY    "YOUR_WRITE_API_KEY"   // <-- Replace with real key

// ──────────────────── Emergency Contacts ─────────────
// FIX #2 : Same extern pattern for String objects.
extern String emergency1;
// extern String emergency2;   // uncomment to add a second contact

// ──────────────────── Google Maps ────────────────────
// Built at runtime; kept extern so all modules can use it.
extern String googleLink;

// ─────────────────────── Pins ────────────────────────
#define GPS_RX      16    // ESP32 UART1 RX  <- GPS TX
#define GPS_TX      17    // ESP32 UART1 TX  -> GPS RX

#define GSM_RX      27    // ESP32 UART2 RX  <- GSM TX
#define GSM_TX      26    // ESP32 UART2 TX  -> GSM RX

#define SOS_BUTTON  14    // Active-LOW push button (INPUT_PULLUP)
#define BUZZER      15    // Active-HIGH buzzer / transistor base
#define LED_PIN      2    // Onboard LED (active-HIGH)

// ──────────────────── Timing (ms) ────────────────────
#define BUTTON_DEBOUNCE   50       // Debounce window
#define GPS_TIMEOUT      30000     // Max wait for valid GPS fix
#define CALL_TIME        20000     // Duration to hold the call open
#define BUZZER_TIME       3000     // Buzzer ON duration on SOS

#endif // CONFIG_H
