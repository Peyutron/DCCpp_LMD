/**********************************************************************

Turnout.cpp, renamed from Accessories.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/
#include "Arduino.h"

#include "DCCpp.h"

#ifdef USE_TURNOUT

#ifdef VISUALSTUDIO
#include "string.h"
#endif

#include "Turnout.h"
#include "DCCpp_Uno.h"
#include "CommInterface.h"
//#include "Comm.h"

#ifdef USE_TEXTCOMMAND
#include "TextCommand.h"
#endif
#ifdef USE_OLED
#include "Oled.h"
#endif
#ifdef USE_SOUND
#include "Sound.h"
#endif
#ifdef USE_EEPROM
#include "EEStore.h"
#include "EEPROM.h"
#endif

///////////////////////////////////////////////////////////////////////////////

void Turnout::begin(int id, int add, int subAdd) {
#if defined(USE_EEPROM)	|| defined(USE_TEXTCOMMAND)
#if defined(USE_EEPROM)	&& defined(DCCPP_DEBUG_MODE)
	if (strncmp(EEStore::data.id, EESTORE_ID, sizeof(EESTORE_ID)) != 0) {    // check to see that eeStore contains valid DCC++ ID
		DCCPP_INTERFACE.println(F("Turnout::begin() must be called BEFORE DCCpp.begin() !"));
	}
#endif
	if (firstTurnout == NULL) {
		firstTurnout = this;
	}
	else if (get(id) == NULL) {
		Turnout *tt = firstTurnout;
		while (tt->nextTurnout != NULL)
			tt = tt->nextTurnout;
		tt->nextTurnout = this;
	}
#endif

	this->set(id, add, subAdd);
	CommManager::printf("<O>");
	#ifdef USE_SOUND
		Sound::ActionOK();
	#endif	 
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::set(int id, int add, int subAdd) {
	this->data.id = id;
	this->data.address = add;
	this->data.subAddress = subAdd;
	this->data.tStatus = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::activate(int s) {
	data.tStatus = (s>0);                                    // if s>0 set turnout=ON, else if zero or negative set turnout=OFF
	DCCpp::mainRegs.setAccessory(this->data.address, this->data.subAddress, this->data.tStatus);
#ifdef USE_EEPROM
	if (this->eepromPos>0)
#ifdef VISUALSTUDIO
		EEPROM.put(this->eepromPos, (void *) &(this->data.tStatus), sizeof(int));	// ArduiEmulator version...
#else
		EEPROM.put(this->eepromPos, this->data.tStatus);
#endif
#endif

		if (data.tStatus == 0){
			CommManager::printf("<H%d %d>", data.id, 0);
		} else {
			CommManager::printf("<H%d %d>", data.id, 1);	 
		}

#ifdef USE_OLED
	Oled::GetAccesories(this->data.address, this->data.subAddress, this->data.tStatus); // OLED
#endif		
}

#if defined(USE_EEPROM)	|| defined(USE_TEXTCOMMAND)
///////////////////////////////////////////////////////////////////////////////

Turnout* Turnout::get(int id) {
	Turnout *tt;
	for (tt = firstTurnout; tt != NULL && tt->data.id != id; tt = tt->nextTurnout)
		;
	return(tt);
}

///////////////////////////////////////////////////////////////////////////////
void Turnout::remove(int id) {
	Turnout *tt, *pp;

	for (tt = firstTurnout, pp = NULL; tt != NULL && tt->data.id != id; pp = tt, tt = tt->nextTurnout)
		;

	if (tt == NULL) {
		CommManager::printf("<X>");
		#ifdef USE_OLED
			Oled::printDelete(1, false, id);	// OLED = muestra en pantalla guadado fallido
		#endif  	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif

		return;
	}

	if (tt == firstTurnout)
		firstTurnout = tt->nextTurnout;
	else
		pp->nextTurnout = tt->nextTurnout;

	free(tt);
	CommManager::printf("<O>");
	#ifdef USE_OLED
		Oled::printDelete(1, true, id);	// OLED = muestra en pantalla guadado con exito
	#endif  	
	#ifdef USE_SOUND
		Sound::ActionOK();
	#endif

}

///////////////////////////////////////////////////////////////////////////////

int Turnout::count() {
	int count = 0;
	Turnout *tt;
	for (tt = firstTurnout; tt != NULL; tt = tt->nextTurnout)
		count++;
	return count;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef USE_EEPROM
void Turnout::load() {
	struct TurnoutData data;
	Turnout *tt;

	for (int i = 0; i<EEStore::data.nTurnouts; i++) {
#ifdef VISUALSTUDIO
		EEPROM.get(EEStore::pointer(), (void *)&data, sizeof(TurnoutData));
#else
		EEPROM.get(EEStore::pointer(), data);
#endif
#if defined(USE_TEXTCOMMAND)
		tt = create(data.id, data.address, data.subAddress);
#else
		tt = get(data.id);
#ifdef DCCPP_DEBUG_MODE
		if (tt == NULL)
			DCCPP_INTERFACE.println(F("Turnout::begin() must be called BEFORE Turnout::load() !"));
		else
#endif
			tt->set(data.id, data.address, data.subAddress);
#endif
		tt->data.tStatus = data.tStatus;
		tt->eepromPos = EEStore::pointer();
		EEStore::advance(sizeof(tt->data));
	}
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::store() {
	Turnout *tt;

	tt = firstTurnout;
	EEStore::data.nTurnouts = 0;

	while (tt != NULL) {
		tt->eepromPos = EEStore::pointer();
		#ifdef USE_OLED 
			Oled::printSaved(true);
		#endif
#ifdef VISUALSTUDIO
		EEPROM.put(EEStore::pointer(), (void *) &(tt->data), sizeof(TurnoutData));	// ArduiEmulator version...
#else
		EEPROM.put(EEStore::pointer(), tt->data);
#endif
		EEStore::advance(sizeof(tt->data));
		tt = tt->nextTurnout;
		EEStore::data.nTurnouts++;
	}
}
#endif

#endif

#if defined(USE_TEXTCOMMAND)
///////////////////////////////////////////////////////////////////////////////

bool Turnout::parse(char *c){
	int n,s,m; // n=id s
 	Turnout *t;
  
	switch(sscanf(c,"%d %d %d",&n,&s,&m)){
    	// Serial.println(n);


    case 2: // argument is string with id number of turnout followed by zero (not thrown) or one (thrown)
      t=get(n);    	
		if (t != NULL) {
			if (s < 0) {	// if second argument s is negative, just send the current state of the turnout.
				if (t->data.tStatus == 0){
					CommManager::printf("<H%d %d>", n, 0);
				} else {
					CommManager::printf("<H%d %d>", n, 1);
				}
			} else t->activate(s);
		} else {	
			CommManager::printf("<X>");
			#ifdef USE_SOUND
				Sound::ActionError();
			#endif
		}
		return true;

    case 3:                     // argument is string with id number of turnout followed by an address and subAddress
    	if(m >= 4) return true;
    	else {
    	create(n,s,m);
		#ifdef USE_OLED
			Oled::printDefined(1, n, s, m);
		#endif
		#ifdef USE_SOUND
			Sound::ActionOK();
		#endif
		return true;
		}
	case 1:                     // argument is a string with id number only
		remove(n);
		return true;

	#ifdef DCCPP_PRINT_DCCPP
	case -1:                    // no arguments
		show();
		return true;
	#endif
  }
	return false;
}

Turnout *Turnout::create(int id, int add, int subAdd) {
	Turnout *tt = new Turnout();

	if (tt == NULL) {       // problem allocating memory
		CommManager::printf("<X>");
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED 
			Oled::printSaved(false);
		#endif
	
		return(tt);
	}

	tt->begin(id, add, subAdd);
	return(tt);
}

	#endif //USE_TEXTCOMMAND
	#if defined(USE_EEPROM)	|| defined(USE_TEXTCOMMAND)
	#ifdef DCCPP_PRINT_DCCPP

///////////////////////////////////////////////////////////////////////////////

void Turnout::show() {
	int count = 0;	
	Turnout *tt;

	if (firstTurnout == NULL) {
		CommManager::printf("<X>");
		#ifdef USE_OLED
			Oled::printErrorList(1);     // OLED
		#endif
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		return;
	}

	for (tt = firstTurnout; tt != NULL; tt = tt->nextTurnout) {
		if (tt->data.tStatus == 0){

			CommManager::printf("<H%d %d %d %d>", tt->data.id, tt->data.address, tt-> data.subAddress, 0);	 
		} else {
			CommManager::printf("<H%d %d %d %d>", tt->data.id, tt->data.address, tt-> data.subAddress, 1);	 
		}

		count++;
	}
	#ifdef USE_OLED
		Oled::printList(1, count);
	#endif
}
#endif

///////////////////////////////////////////////////////////////////////////////

Turnout *Turnout::firstTurnout = NULL;
#endif

#endif //USE_TURNOUT
