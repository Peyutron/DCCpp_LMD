//SerialBluetooth.h
#ifndef SerialBluetooth_h
#define SerialBluetooth_h

#include "Arduino.h"
#include "DCCpp.h"


#define BLUETOOTH Serial2
#define BT_MAX_COMMAND_LENGTH         30


class Bluetooth {
public:
	static char btCommandString[BT_MAX_COMMAND_LENGTH+1];
 	static void btprocess();

};

#endif