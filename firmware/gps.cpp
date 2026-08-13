/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - GPS IMPLEMENTATION
 *
 *  FIXES APPLIED:
 *  FIX #3 – WDT (Watchdog Timer) crash fix:
 *            The original code had a tight while-loop
 *            that could run for up to GPS_TIMEOUT (30s)
 *            without ever yielding to the FreeRTOS
 *            scheduler. On ESP32 this triggers the Task
 *            Watchdog Timer (TWDT) and causes an unwanted
 *            reboot. Fixed by adding delay(1) inside both
 *            inner and outer loops so the IDLE task (and
 *            WDT reset) can run between iterations.
 *****************************************************/

#include "gps.h"
#include "config.h"

TinyGPSPlus gps;

// Use UART1 (Hardware Serial port 1 of the ESP32)
HardwareSerial GPSSerial(1);

// ─────────────────────────────────────────────────────
void initGPS()
{
    // 9600 baud is the NEO-6M factory default
    GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    Serial.println("[GPS] UART1 initialised at 9600 baud.");
}

// ─────────────────────────────────────────────────────
bool updateGPS()
{
    Serial.println("[GPS] Waiting for location fix...");
    unsigned long start = millis();

    while (millis() - start < GPS_TIMEOUT)
    {
        // Feed all available bytes into the TinyGPS++ parser
        while (GPSSerial.available())
        {
            gps.encode(GPSSerial.read());
        }

        // Check for a freshly updated, valid fix
        if (gps.location.isUpdated() && gps.location.isValid())
        {
            Serial.printf("[GPS] Fix acquired: %.6f, %.6f\n",
                          gps.location.lat(),
                          gps.location.lng());
            return true;
        }

        // FIX #3 : yield 1 ms so the FreeRTOS IDLE task
        //          can feed the hardware watchdog timer.
        delay(1);
    }

    Serial.println("[GPS] Timeout – no fix obtained.");
    return false;
}

// ─────────────────────────────────────────────────────
bool isGPSValid()
{
    return gps.location.isValid();
}

// ─────────────────────────────────────────────────────
double getLatitude()
{
    return gps.location.lat();
}

// ─────────────────────────────────────────────────────
double getLongitude()
{
    return gps.location.lng();
}

// ─────────────────────────────────────────────────────
String getGoogleMapsLink()
{
    // Format: https://www.google.com/maps?q=18.520430,73.856743
    String url = "https://www.google.com/maps?q=";
    url += String(getLatitude(),  6);
    url += ",";
    url += String(getLongitude(), 6);
    return url;
}
