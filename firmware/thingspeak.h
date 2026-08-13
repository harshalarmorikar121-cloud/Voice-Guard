/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - THINGSPEAK HEADER
 *****************************************************/

#ifndef THINGSPEAK_H
#define THINGSPEAK_H

// FIX #7 : The original header included only <WiFi.h>.
//           The ThingSpeak library also needs WiFiClient,
//           which is already part of <WiFi.h> on ESP32,
//           but the ThingSpeak header itself must be
//           included in the header so the compiler knows
//           the return type of ThingSpeak calls when
//           other modules include this header.
#include <WiFi.h>
#include <ThingSpeak.h>   // Library: ThingSpeak by MathWorks

// Connect / reconnect to the configured Wi-Fi network.
// Returns true once WL_CONNECTED, false on timeout (15 s).
bool connectWiFi();

// Upload a data point to ThingSpeak.
//   field1 = latitude
//   field2 = longitude
//   field3 = sosStatus  (1 = SOS active, 0 = normal)
// Returns true on HTTP 200 response.
bool uploadThingSpeak(double latitude,
                      double longitude,
                      int    sosStatus);

#endif // THINGSPEAK_H
