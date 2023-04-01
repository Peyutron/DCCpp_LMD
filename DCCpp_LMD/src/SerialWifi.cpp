// SerialWifi.cpp

#include "Arduino.h"
#include "DCCpp.h"
#include "SerialWifi.h"
#ifdef USE_SERIALWIFI
#include "TextCommand.h"


////////////////////////

char Wifi::wfCommandString[WF_MAX_COMMAND_LENGTH+1];

////////////////////////
void Wifi::wifiprocess(){

  char w;

	
	  // SERIAL case

	while (WIFI.available()>0) {    // while there is data on the serial line c= 
		w = WIFI.read(); 
		if (w == '<')                // start of new command
			wfCommandString[0] = 0;
		else if (w == '>')               // end of new command 
			TextCommand::parse(wfCommandString); 
		else if (strlen(wfCommandString) < WF_MAX_COMMAND_LENGTH)	// if btCommandString still has space, append character just read from serial line
			sprintf(wfCommandString, "%s%c", wfCommandString, w);	
		// otherwise, character is ignored (but continue to look for '<' or '>')

		} // while
		
} // TextCommand:process
#endif



/*void Wifi::printf(const char *fmt, ...) {
	char buf[128] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	#ifdef USE_SERIALWIFI
		WIFI.println(buf);
	#endif
	}*/
