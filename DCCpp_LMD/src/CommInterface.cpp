/*
 * CommInterface.cpp
 *
 *  Created on: Mar 3, 2017
 *      Author: mdunston
 *  
 *  If using a Mega (or an UNO using software serial):
 *  This unit creates a common communication interface to allow bidirectional communication 
 *  though all serial and networked devices connect to the base station. Any commands and responses
 *  sent to one interface will be echoed to all the other. EX: Command entered and responses recevied
 *  over the serial connection will appear on an esp8266 WiFi connection and visa versa.
 *  
 *  CommInterface adapters for serial/ethernet/ESP8266 translate the text stream to the target interface
 *  and back. By having the code reference "CommInterfaceX", where X is the name of the interface
 *  (ESP, Ethernet, Serial) to transmit / receive text it makes the rest of the DCC++EX code generic 
 *  in that it doesn't know or care what it is really talking to.
 *  
 *  SEE: CommInterfaceESP.cpp, CommInterfaceEthernet.cpp and CommInterfaceSerial.cpp
 */

#include <Arduino.h>
#include "CommInterface.h"
#include "DCCpp.h"



// This routine sends DCCpp output and responses to all the devices connected via comm interfaces
void CommManager::printf(const char *fmt, ...) {
	char buf[128] = {0};
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	
	#ifdef USE_TEXTCOMMAND
		DCCPP_INTERFACE.print(buf);	// Serial (PC)
		#if !defined(USE_ETHERNET)
			DCCPP_INTERFACE.println("");
		#endif
	#endif
	#ifdef USE_SERIALWIFI		// Serial1
		WIFI.print(buf);
	#endif
	#ifdef USE_SERIALBLUETOOTH	// Serial2
		BLUETOOTH.print(buf);	
	#endif
	#ifdef USE_SERIALAUX		// Serial3
		SERIALAUX.print(buf);	
	#endif	
	/*#if !defined(USE_ETHERNET)
		DCCPP_INTERFACE.println("");	
	#endif*/	

	/*for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->send(buf);
		}
	}*/
}

// CommInterface *CommManager::interfaces[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
// int CommManager::nextInterface = 0;

// CommInterface::~CommInterface() {
// }

/*void CommManager::update() {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->process();
		}
	}
}*/
 
/*void CommManager::registerInterface(CommInterface *interface) {
	if(nextInterface < 10) {
		interfaces[nextInterface++] = interface;
	}
}*/

/*void CommManager::showConfiguration() {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->showConfiguration();
		}
	}
}*/

/* void CommManager::showInitInfo() {
	for(int i = 0; i < nextInterface; i++) {
		if(interfaces[i] != NULL) {
			interfaces[i]->showInitInfo();
		}
	}
}*/

