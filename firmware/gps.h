/*****************************************************
 *      SMART WOMEN SAFETY DEVICE - GPS HEADER
 *****************************************************/

#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>   // Library: TinyGPS++ by Mikal Hart

// Shared GPS object – defined in gps.cpp, used in main sketch
extern TinyGPSPlus gps;

// Initialise UART1 for GPS communication
void initGPS();

// Block until a valid location fix arrives (or GPS_TIMEOUT elapses).
// Returns true  -> fix acquired, getLatitude() / getLongitude() are valid.
// Returns false -> timeout, no fix.
bool updateGPS();

// Non-blocking validity check
bool isGPSValid();

double  getLatitude();
double  getLongitude();

// Returns "https://www.google.com/maps?q=lat,lng"
String  getGoogleMapsLink();

#endif // GPS_H
