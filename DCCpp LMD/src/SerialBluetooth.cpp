// SerialBluetooth.cpp

#include "Arduino.h"
#include "DCCpp.h"
#include "SerialBluetooth.h"
#ifdef USE_SERIALBLUETOOTH
#include "TextCommand.h"


////////////////////////

char Bluetooth::btCommandString[BT_MAX_COMMAND_LENGTH+1];

////////////////////////
void Bluetooth::btprocess(){

  char b;

		while (BLUETOOTH.available()>0) {    // while there is data on the serial line
			b = BLUETOOTH.read();
			if (b == '<')                    // start of new command
				btCommandString[0] = 0;
			else if (b == '>')               // end of new command
				TextCommand::parse(btCommandString);
			else if (strlen(btCommandString) < BT_MAX_COMMAND_LENGTH)	// if btCommandString still has space, append character just read from serial line
				sprintf(btCommandString, "%s%c", btCommandString, b);	// otherwise, character is ignored (but continue to look for '<' or '>')

		} // while
		
} // TextCommand:process
#endif




/*void Bluetooth::printf(const char *fmt, ...) {
	char buf[128] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	#ifdef USE_SERIALBLUETOOTH
		BLUETOOTH.println(buf);
	#endif
	}*/
