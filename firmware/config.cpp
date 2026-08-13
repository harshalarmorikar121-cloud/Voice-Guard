/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - CONFIG DEFINITIONS
 *      FIX #1 : All global String / char* variables are
 *               defined here (in ONE translation unit)
 *               and declared extern in config.h.
 *****************************************************/

#include "config.h"

// ─────────────────────── WiFi ────────────────────────
const char* WIFI_NAME     = "YourWiFiSSID";     // <-- Replace
const char* WIFI_PASSWORD = "YourWiFiPass";     // <-- Replace

// ──────────────────── Emergency Contacts ─────────────
String emergency1 = "+91XXXXXXXXXX";            // <-- Set your primary guardian number here
// String emergency2 = "+91XXXXXXXXXX";          // <-- Optional secondary number

// ──────────────────── Google Maps Link ───────────────
// Will be assembled at runtime after GPS fix.
String googleLink = "";
