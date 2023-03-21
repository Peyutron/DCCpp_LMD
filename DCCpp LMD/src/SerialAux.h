// SerialAux.h
#ifndef SerialAux_h
#define SerialAux_h

#include "Arduino.h"

#include "DCCpp.h"

// #include "PacketRegister.h"
// #include "CurrentMonitor.h"

#define SERIALAUX Serial3
#define AUX_MAX_COMMAND_LENGTH         30


class SerialAux {
public:
	static char auxCommandString[AUX_MAX_COMMAND_LENGTH+1];
 	static void auxprocess();
	//static void printf(const char *fmt, ...);

};

#endif