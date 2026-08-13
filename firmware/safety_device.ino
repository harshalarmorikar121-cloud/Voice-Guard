/*
  ==================================================================================
  PROJECT: Smart Women Safety & Emergency Response Device (GSM GPRS ONLY)
  MICROCONTROLLER: ESP32
  MODULES: NEO-6M GPS, SIM800C/SIM900A GSM, SOS Panic Button, ThingSpeak Cloud IoT
  USB PROVISIONING: Uses Preferences NVS Flash for Web Serial USB Configuration
  ==================================================================================
*/

#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Preferences.h>
#include <ArduinoJson.h>

// Preferences (NVS Flash Storage)
Preferences preferences;

// Emergency Contact Numbers stored in NVS Flash
String primaryContact   = "+919876543210";
String secondaryContact = "+919876543211";

// ThingSpeak Cloud Configuration
const char* THINGSPEAK_API_KEY = "YOUR_THINGSPEAK_WRITE_API_KEY";
const char* THINGSPEAK_SERVER  = "api.thingspeak.com";

// Hardware Pin Definitions
#define PIN_SOS_BUTTON 4
#define PIN_STATUS_LED 2
#define PIN_BUZZER     15

#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

#define GSM_RX_PIN 26
#define GSM_TX_PIN 27

// Hardware Serial Interfaces
HardwareSerial SerialGPS(2);  // UART2 for GPS
HardwareSerial SerialGSM(1);  // UART1 for GSM

// TinyGPS++ Object
TinyGPSPlus gps;

// System Variables & Debounce
unsigned long lastButtonPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 500; // ms
bool isEmergencyActive = false;
int simulatedHeartRate = 82; // BPM

// Timing for continuous IoT cloud updates
unsigned long lastCloudLogTime = 0;
const unsigned long CLOUD_LOG_INTERVAL = 30000; // 30 seconds

// ================================================================================
// FUNCTION DECLARATIONS
// ================================================================================
void loadNVSConfiguration();
void handleUSBSerialCommands();
void initGSM();
void sendATCommand(String command, int timeout, bool debug = false);
String sendATCommandWithResponse(String command, int timeout);
void triggerEmergency();
void sendSMS(String phoneNumber, String message);
void makePhoneCall(String phoneNumber);
void logToThingSpeak(float lat, float lng, int heartRate, String statusStr);
void soundBuzzerPattern();

// ================================================================================
// SETUP FUNCTION
// ================================================================================
void setup() {
  // Initialize USB Serial at 115200 for Web Serial Configurator
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("\n=================================================="));
  Serial.println(F("  SAFESHIELD WOMEN SAFETY DEVICE - INITIALIZING   "));
  Serial.println(F("             (GSM / GPRS ONLY MODE)               "));
  Serial.println(F("=================================================="));

  // Initialize GPIO Pins
  pinMode(PIN_SOS_BUTTON, INPUT_PULLUP);
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_STATUS_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  // Load Saved Configuration from ESP32 NVS Flash Memory
  loadNVSConfiguration();

  // Initialize GPS Serial
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println(F("[INFO] GPS Module Serial Initialized (9600 Baud)"));

  // Initialize GSM Serial
  SerialGSM.begin(9600, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
  Serial.println(F("[INFO] GSM Module Serial Initialized (9600 Baud)"));
  
  // Configure GSM Module for SMS and GPRS
  initGSM();

  Serial.println(F("[READY] Device System Fully Armed. Awaiting SOS Trigger or USB Config..."));
}

// ================================================================================
// MAIN LOOP
// ================================================================================
void loop() {
  // 1. Listen for USB Serial Configuration commands from Web App
  handleUSBSerialCommands();

  // 2. Continuously feed incoming GPS stream to TinyGPS++
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }

  // 3. Read SOS Switch with Software Debounce
  if (digitalRead(PIN_SOS_BUTTON) == LOW) {
    if (millis() - lastButtonPressTime > DEBOUNCE_DELAY) {
      lastButtonPressTime = millis();
      Serial.println(F("\n[!!! ALERT !!!] SOS PANIC BUTTON DETECTED!"));
      triggerEmergency();
    }
  }

  // 4. Periodic Background Telemetry to Cloud (ThingSpeak) via GPRS
  if (millis() - lastCloudLogTime >= CLOUD_LOG_INTERVAL) {
    lastCloudLogTime = millis();
    float lat = gps.location.isValid() ? gps.location.lat() : 0.0;
    float lng = gps.location.isValid() ? gps.location.lng() : 0.0;
    logToThingSpeak(lat, lng, simulatedHeartRate, isEmergencyActive ? "EMERGENCY" : "NORMAL");
  }
}

// ================================================================================
// NVS FLASH STORAGE ROUTINES
// ================================================================================
void loadNVSConfiguration() {
  preferences.begin("safeshield", true); // Read-only mode
  primaryContact   = preferences.getString("num1", "+919876543210");
  secondaryContact = preferences.getString("num2", "+919876543211");
  preferences.end();

  Serial.println(F("[NVS Flash] Configuration Loaded Successfully:"));
  Serial.print(F("  - Primary Emergency Number  : ")); Serial.println(primaryContact);
  Serial.print(F("  - Secondary Emergency Number: ")); Serial.println(secondaryContact);
}

void handleUSBSerialCommands() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "GET_CONFIG") {
      String jsonResp = "CONFIG_DATA:{\"num1\":\"" + primaryContact + "\",\"num2\":\"" + secondaryContact + "\"}";
      Serial.println(jsonResp);
    } 
    else if (input.startsWith("SET_CONFIG:")) {
      String payload = input.substring(11);
      
      // Simple JSON Field Parsing
      int num1Idx = payload.indexOf("\"num1\":\"");
      int num2Idx = payload.indexOf("\"num2\":\"");

      if (num1Idx != -1) {
        int endIdx = payload.indexOf("\"", num1Idx + 8);
        primaryContact = payload.substring(num1Idx + 8, endIdx);
      }
      if (num2Idx != -1) {
        int endIdx = payload.indexOf("\"", num2Idx + 8);
        secondaryContact = payload.substring(num2Idx + 8, endIdx);
      }

      // Save to ESP32 Preferences NVS Flash
      preferences.begin("safeshield", false); // Read-write mode
      preferences.putString("num1", primaryContact);
      preferences.putString("num2", secondaryContact);
      preferences.end();

      // Send final confirmation SMS
      if (primaryContact.length() > 0) {
        Serial.println(F("[INFO] Sending registration confirmation SMS..."));
        sendSMS(primaryContact, "Your mobile numbers is succesfully registered on voice-guard device");
      }

      Serial.println(F("SUCCESS:CONFIG_SAVED"));
      soundBuzzerPattern();
    }
    else if (input.startsWith("SEND_OTP_SMS|")) {
      int firstPipe = input.indexOf('|');
      int secondPipe = input.indexOf('|', firstPipe + 1);
      
      if (firstPipe != -1 && secondPipe != -1) {
        String phone = input.substring(firstPipe + 1, secondPipe);
        String otp = input.substring(secondPipe + 1);
        String otpMsg = "Your Voice-Guard OTP is: " + otp;
        
        Serial.print(F("[INFO] Sending OTP SMS to: "));
        Serial.println(phone);
        
        sendSMS(phone, otpMsg);
        Serial.println(F("SUCCESS:OTP_SENT"));
      }
    }
  }
}

// ================================================================================
// GSM INITIALIZATION ROUTINE
// ================================================================================
void initGSM() {
  Serial.println(F("[GSM] Initializing SIM800C/SIM900A..."));
  sendATCommand("AT", 1000, true);                  // Handshake test
  sendATCommand("AT+CPIN?", 1000, true);            // Check SIM card ready
  sendATCommand("AT+CREG?", 1000, true);            // Check Network Registration
  sendATCommand("AT+CMGF=1", 1000, true);           // Set SMS mode to Text
  sendATCommand("AT+CNMI=2,2,0,0,0", 1000, true);   // Set SMS notification parameters
  
  // GPRS Setup
  Serial.println(F("[GSM] Configuring GPRS Bearer..."));
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", 1000, true); // Set connection type to GPRS
  // NOTE: Depending on your SIM carrier, you might need to set the APN here, e.g.:
  // sendATCommand("AT+SAPBR=3,1,\"APN\",\"internet\"", 1000, true); 
  
  Serial.println(F("[GSM] Module Ready for SMS, Calls & GPRS Internet."));
}

// ================================================================================
// EMERGENCY TRIGGER WORKFLOW
// ================================================================================
void triggerEmergency() {
  isEmergencyActive = true;
  digitalWrite(PIN_STATUS_LED, HIGH);

  // Sound audible alert pattern
  soundBuzzerPattern();

  // Attempt to acquire fresh GPS coordinates
  Serial.println(F("[GPS] Fetching satellite location fix..."));
  
  unsigned long startGpsWait = millis();
  while ((millis() - startGpsWait < 5000) && !gps.location.isValid()) {
    while (SerialGPS.available() > 0) {
      gps.encode(SerialGPS.read());
    }
    delay(100);
  }

  double latitude = 0.0;
  double longitude = 0.0;
  String mapUrl = "";

  if (gps.location.isValid()) {
    latitude  = gps.location.lat();
    longitude = gps.location.lng();
    mapUrl = "http://maps.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
    Serial.println(F("[GPS FIX ACQUIRED] Location:"));
    Serial.println(mapUrl);
  } else {
    Serial.println(F("[WARNING] GPS Lock pending indoors. Sending fallback alert."));
    mapUrl = "Location unavailable (GPS satellite search active). Emergency alert triggered!";
  }

  // Construct Emergency Message
  String sosMessage = "EMERGENCY ALERT!\nI am in danger and need immediate help.\n\nLocation:\n" + mapUrl;
  if (gps.speed.isValid()) {
    sosMessage += "\nSpeed: " + String(gps.speed.kmph()) + " km/h";
  }

  // Step 1: Send SMS to Primary Contact
  if (primaryContact.length() > 0) {
    Serial.print(F("[SMS] Sending alert to Primary: "));
    Serial.println(primaryContact);
    sendSMS(primaryContact, sosMessage);
    delay(2000);
  }

  // Step 2: Send SMS to Secondary Contact if available
  if (secondaryContact.length() > 0 && secondaryContact != primaryContact) {
    Serial.print(F("[SMS] Sending alert to Secondary: "));
    Serial.println(secondaryContact);
    sendSMS(secondaryContact, sosMessage);
    delay(2000);
  }

  // Step 3: Cloud Telemetry Upload via GPRS
  logToThingSpeak(latitude, longitude, simulatedHeartRate, "SOS_ACTIVE");

  // Step 4: Place Direct Emergency Phone Call to primary contact
  if (primaryContact.length() > 0) {
    Serial.print(F("[CALL] Placing direct voice call to primary contact: "));
    Serial.println(primaryContact);
    makePhoneCall(primaryContact);
  }

  Serial.println(F("[SUCCESS] All emergency notifications dispatched."));
  digitalWrite(PIN_STATUS_LED, LOW);
}

// ================================================================================
// GSM UTILITIES
// ================================================================================
void sendSMS(String phoneNumber, String message) {
  SerialGSM.print("AT+CMGS=\"");
  SerialGSM.print(phoneNumber);
  SerialGSM.println("\"");
  delay(1000);
  
  SerialGSM.print(message);
  delay(500);
  
  SerialGSM.write(26); // CTRL+Z
  delay(5000);

  while (SerialGSM.available()) {
    String resp = SerialGSM.readString();
    Serial.println("[GSM Resp] " + resp);
  }
}

void makePhoneCall(String phoneNumber) {
  SerialGSM.print("ATD");
  SerialGSM.print(phoneNumber);
  SerialGSM.println(";");
  delay(15000);
  sendATCommand("ATH", 1000, true); // Hang up
}

void sendATCommand(String command, int timeout, bool debug) {
  sendATCommandWithResponse(command, timeout);
}

String sendATCommandWithResponse(String command, int timeout) {
  SerialGSM.println(command);
  unsigned long startTime = millis();
  String response = "";
  
  while (millis() - startTime < timeout) {
    while (SerialGSM.available()) {
      char c = SerialGSM.read();
      response += c;
    }
  }
  return response;
}

void logToThingSpeak(float lat, float lng, int heartRate, String statusStr) {
  Serial.println(F("[IoT Cloud] Uploading data via GSM GPRS..."));
  
  // Construct the GET URL
  String url = "http://" + String(THINGSPEAK_SERVER) + "/update?api_key=" + String(THINGSPEAK_API_KEY) +
               "&field1=" + String(lat, 6) +
               "&field2=" + String(lng, 6) +
               "&field3=" + String(heartRate) +
               "&field4=" + statusStr;

  // Open GPRS Context
  sendATCommand("AT+SAPBR=1,1", 2000, false); 
  delay(1000);

  // Initialize HTTP service
  sendATCommand("AT+HTTPINIT", 1000, false);
  delay(1000);

  // Set HTTP Parameters (CID & URL)
  sendATCommand("AT+HTTPPARA=\"CID\",1", 1000, false);
  
  SerialGSM.print("AT+HTTPPARA=\"URL\",\"");
  SerialGSM.print(url);
  SerialGSM.println("\"");
  delay(1000);

  // Execute HTTP GET Action
  Serial.println(F("[GSM] Executing HTTP GET..."));
  sendATCommand("AT+HTTPACTION=0", 5000, false); // Wait up to 5s for network response

  // Terminate HTTP service
  sendATCommand("AT+HTTPTERM", 1000, false);

  Serial.println(F("[IoT Cloud] GSM GPRS Data Upload Sequence Completed."));
}

void soundBuzzerPattern() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(150);
    digitalWrite(PIN_BUZZER, LOW);
    delay(100);
  }
}
