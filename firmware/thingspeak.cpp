/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - THINGSPEAK
 *
 *  FIXES APPLIED:
 *  FIX #8 – connectWiFi() is now idempotent:
 *            Added WiFi.disconnect() + WiFi.mode()
 *            before calling begin() so a stale
 *            connection state from a previous boot
 *            doesn't prevent reconnection.
 *
 *  FIX #9 – ThingSpeak.begin() called only once:
 *            Moved ThingSpeak.begin(client) into
 *            connectWiFi() but guarded so it is only
 *            called the first time a connection
 *            succeeds; subsequent calls to
 *            uploadThingSpeak() skip re-init.
 *****************************************************/

#include "thingspeak.h"
#include "config.h"

WiFiClient client;
static bool tsInitialised = false;   // FIX #9

// ─────────────────────────────────────────────────────
bool connectWiFi()
{
    if (WiFi.status() == WL_CONNECTED) return true;

    // FIX #8 : reset any stale state before connecting
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    Serial.print("[WiFi] Connecting");
    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - start < 15000)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[WiFi] Connection FAILED.");
        return false;
    }

    Serial.print("[WiFi] Connected. IP: ");
    Serial.println(WiFi.localIP());

    // FIX #9 : initialise ThingSpeak client once
    if (!tsInitialised)
    {
        ThingSpeak.begin(client);
        tsInitialised = true;
    }

    return true;
}

// ─────────────────────────────────────────────────────
bool uploadThingSpeak(double latitude,
                      double longitude,
                      int    sosStatus)
{
    if (!connectWiFi()) return false;

    ThingSpeak.setField(1, (float)latitude);
    ThingSpeak.setField(2, (float)longitude);
    ThingSpeak.setField(3, sosStatus);

    int response = ThingSpeak.writeFields(CHANNEL_NUMBER, WRITE_API_KEY);

    if (response == 200)
    {
        Serial.println("[ThingSpeak] Upload success.");
        return true;
    }

    Serial.print("[ThingSpeak] Upload failed, code: ");
    Serial.println(response);
    return false;
}
