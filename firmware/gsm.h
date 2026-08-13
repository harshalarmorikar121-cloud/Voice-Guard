/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - GSM HEADER
 *****************************************************/

#ifndef GSM_H
#define GSM_H

#include <Arduino.h>

// Initialise the GSM module and verify it responds to AT commands.
// Returns true on success, false if the module does not respond.
bool initGSM();

// Send an SMS text message.
// phoneNumber : international format e.g. "+917709481015"
// message     : plain text body (keep under 160 chars for single PDU)
// Returns true when the modem replies with "+CMGS:" confirmation.
bool sendSMS(const String& phoneNumber, const String& message);

// Dial a voice call. Returns true if the modem accepted the ATD command.
bool makeCall(const String& phoneNumber);

// Send ATH to hang up any active call.
void hangUp();

#endif // GSM_H
