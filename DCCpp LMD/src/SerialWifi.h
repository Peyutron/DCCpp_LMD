//Bluetooth.h
#ifndef SerialWifi_h
#define SerialWifi_h

#include "Arduino.h"

#include "DCCpp.h"

// #include "PacketRegister.h"
// #include "CurrentMonitor.h"

#define WIFI Serial1
#define WF_MAX_COMMAND_LENGTH         30


class Wifi {
public:
	static char wfCommandString[WF_MAX_COMMAND_LENGTH+1];
 	static void wifiprocess();
	// static void printf(const char *fmt, ...);

};

#endif