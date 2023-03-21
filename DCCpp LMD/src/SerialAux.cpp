// SeriaAux.cpp

#include "Arduino.h"

#include "DCCpp.h"
#include "SerialAux.h"
#ifdef USE_SERIALAUX
#include "TextCommand.h"


////////////////////////

char SerialAux::auxCommandString[AUX_MAX_COMMAND_LENGTH+1];

////////////////////////
void SerialAux::auxprocess(){

  char a;

	
	  // SERIAL case

		while (SERIALAUX.available()>0) {    // while there is data on the serial line
			a = SERIALAUX.read();
			if (a == '<')                    // start of new command
				auxCommandString[0] = 0;
			else if (a == '>')               // end of new command
				TextCommand::parse(auxCommandString);
			else if (strlen(auxCommandString) < AUX_MAX_COMMAND_LENGTH)	// if AUXCommandString still has space, append character just read from serial line
				sprintf(auxCommandString, "%s%c", auxCommandString, a);	// otherwise, character is ignored (but continue to look for '<' or '>')

		} // while
		
} // TextCommand:process
#endif

/*void SerialAux::printf(const char *fmt, ...) {
	char buf[128] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	#ifdef USE_SERIALAUX
		SERIALAUX.println(buf);
		Serial.println(F("AUX: "));
		Serial.println(buf);
	#endif
	}*/
