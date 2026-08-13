/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - GSM IMPLEMENTATION
 *
 *  FIXES APPLIED:
 *  FIX #4 – WDT-safe response parser:
 *            Same as GPS – added delay(1) inside the
 *            waitForResponse() helper so FreeRTOS IDLE
 *            task runs and prevents WDT reboots.
 *
 *  FIX #5 – SMS confirmation check:
 *            Original code waited for "OK" after Ctrl-Z.
 *            The SIM800L actually sends "+CMGS: <n>\r\nOK"
 *            so we now check for "+CMGS" which appears
 *            before "OK" and is the correct success marker.
 *
 *  FIX #6 – initGSM() returns false path corrected:
 *            If "AT" does not get an "OK" within 2 s the
 *            module is likely powered off or wired wrong.
 *            We now print a helpful diagnostic message.
 *****************************************************/

#include "gsm.h"
#include "config.h"

// Use UART2 (Hardware Serial port 2 of the ESP32)
HardwareSerial GSMSerial(2);

static const uint32_t GSM_BAUD = 9600;

// ─────────────────────────────────────────────────────
// waitForResponse()
//   Reads from GSMSerial until 'expected' substring is
//   found or timeoutMs elapses.
//   FIX #4 : delay(1) added so the WDT is fed.
// ─────────────────────────────────────────────────────
static bool waitForResponse(const String& expected, uint32_t timeoutMs)
{
    String response = "";
    unsigned long start = millis();

    while (millis() - start < timeoutMs)
    {
        while (GSMSerial.available())
        {
            char c = (char)GSMSerial.read();
            response += c;

            if (response.indexOf(expected) != -1)
            {
                Serial.print("[GSM RX] ");
                Serial.println(response);
                return true;
            }
        }
        delay(1);  // FIX #4 – yield to RTOS / feed WDT
    }

    Serial.print("[GSM] Timeout waiting for: ");
    Serial.println(expected);
    return false;
}

// ─────────────────────────────────────────────────────
bool initGSM()
{
    GSMSerial.begin(GSM_BAUD, SERIAL_8N1, GSM_RX, GSM_TX);
    delay(1000);   // Give the SIM800L time to power up

    Serial.println("[GSM] Sending AT...");
    GSMSerial.println("AT");
    if (!waitForResponse("OK", 3000))   // FIX #6 : extended to 3 s
    {
        Serial.println("[GSM] ERROR – No response. Check power & wiring.");
        return false;
    }

    GSMSerial.println("ATE0");          // Disable command echo
    waitForResponse("OK", 2000);

    GSMSerial.println("AT+CMGF=1");     // Set SMS to text mode
    waitForResponse("OK", 2000);

    GSMSerial.println("AT+CREG?");      // Check network registration
    waitForResponse("OK", 3000);

    Serial.println("[GSM] Initialisation complete.");
    return true;
}

// ─────────────────────────────────────────────────────
bool sendSMS(const String& phoneNumber, const String& message)
{
    Serial.print("[GSM] Sending SMS to ");
    Serial.println(phoneNumber);

    GSMSerial.print("AT+CMGS=\"");
    GSMSerial.print(phoneNumber);
    GSMSerial.println("\"");

    if (!waitForResponse(">", 5000))
    {
        Serial.println("[GSM] SMS prompt '>' not received.");
        return false;
    }

    GSMSerial.print(message);
    GSMSerial.write(26);   // Ctrl+Z  (ASCII 26) terminates the SMS body

    // FIX #5 : Check for "+CMGS:" (sent by SIM800L after the message is
    //           queued in the network) rather than bare "OK".
    if (waitForResponse("+CMGS:", 15000))
    {
        Serial.println("[GSM] SMS sent successfully.");
        return true;
    }

    Serial.println("[GSM] SMS send FAILED.");
    return false;
}

// ─────────────────────────────────────────────────────
bool makeCall(const String& phoneNumber)
{
    Serial.print("[GSM] Dialling ");
    Serial.println(phoneNumber);

    GSMSerial.print("ATD");
    GSMSerial.print(phoneNumber);
    GSMSerial.println(";");   // Trailing semicolon = voice call (not data)

    return waitForResponse("OK", 5000);
}

// ─────────────────────────────────────────────────────
void hangUp()
{
    Serial.println("[GSM] Hanging up.");
    GSMSerial.println("ATH");
    waitForResponse("OK", 3000);
}
