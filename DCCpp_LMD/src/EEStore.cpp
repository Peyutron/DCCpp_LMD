/**********************************************************************

EEStore.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "Arduino.h"

#include "EEStore.h"

#ifdef USE_EEPROM
#ifdef VISUALSTUDIO
#include "string.h"
#endif
#include "DCCpp_Uno.h"
#include "Turnout.h"
#ifdef USE_OLED
#include "Oled.h"
#endif
#include "Sensor.h"
#include "Outputs.h"
#include "EEPROM.h"
#include "S88.h"

///////////////////////////////////////////////////////////////////////////////

void EEStore::init(){

  
  //eeStore=(EEStore *)calloc(1,sizeof(EEStore));

  //EEPROM.get(0,eeStore->data);                                       // obtiene los datos de eeStore 
#ifdef VISUALSTUDIO
  EEPROM.get(0, (void *)&data, sizeof(EEStoreData));
#else
  EEPROM.get(0, data);
#endif
  
  if(strncmp(data.id,EESTORE_ID,sizeof(EESTORE_ID))!=0){    // verifica que eeStore contenga una ID de DCC++ válida
    sprintf(data.id,EESTORE_ID);                           // si no, crea una estructura eeStore en blanco (sin desvíos, sin sensores) y la guarda nuevamente en EEPROM
#ifdef USE_TURNOUT
	data.nTurnouts=0;
#endif
#ifdef USE_SENSOR
	data.nSensors=0;
#endif
#ifdef USE_OUTPUT
	data.nOutputs=0;
#endif
#ifdef USE_S88
  data.nS88=0;
#endif	
#ifdef VISUALSTUDIO
	EEPROM.put(0, (void *)&data, sizeof(EEStoreData));
#else
	EEPROM.put(0, data);
#endif
  }
  
  reset();            // establece el puntero de memoria en el primer espacio libre de EEPROM
#ifdef USE_TURNOUT
  Turnout::load();    // carga las definiciones de los desvíos
#endif
#ifdef USE_SENSOR
  Sensor::load();     // carga las definiciones de los sensores
#endif
#ifdef USE_OUTPUT
  Output::load();     // carga las definiciones de las salidas 
#endif
#ifdef USE_S88
  S88::load();        // carga las definiciones de S88 
#endif
}

///////////////////////////////////////////////////////////////////////////////

void EEStore::clear(){
  sprintf(data.id,EESTORE_ID);  // crea una estructura eeStore en blanco (sin desvíos, sin sensores) y la guarda nuevamente en EEPROM
#ifdef USE_TURNOUT
  data.nTurnouts=0;
#endif
#ifdef USE_SENSOR
  data.nSensors=0;
#endif
#ifdef USE_OUTPUT
  data.nOutputs=0;
#endif
#ifdef USE_S88
  data.nS88=0;
#endif
#ifdef VISUALSTUDIO
  EEPROM.put(0, (void *)&data, sizeof(EEStoreData));
#else
  EEPROM.put(0, data);
#endif
#ifdef USE_OLED
  Oled::printDeleteMemory();
#endif
#ifdef USE_SOUND
  Sound::ActionOK();
#endif

}

///////////////////////////////////////////////////////////////////////////////

void EEStore::store() {
	reset();
#ifdef USE_TURNOUT
	Turnout::store();
#endif
#ifdef USE_SENSOR
	Sensor::store();
#endif
#ifdef USE_OUTPUT
	Output::store();
#endif
#ifdef USE_S88
  S88::store();
#endif
#ifdef VISUALSTUDIO
	EEPROM.put(0, (void *)&data, sizeof(EEStoreData));
#else
	EEPROM.put(0, data);
#endif
}

///////////////////////////////////////////////////////////////////////////////

bool EEStore::needsRefreshing() {
#ifdef USE_TURNOUT
	if (data.nTurnouts != Turnout::count())
		return true;
#endif
#ifdef USE_SENSOR
	if (data.nSensors != Sensor::count())
		return true;
#endif
#ifdef USE_OUTPUT
	if (data.nOutputs != Output::count())
		return true;
#endif
#ifdef USE_S88
  if (data.nS88 != S88::count())
    return true;
#endif
	return false;
}

///////////////////////////////////////////////////////////////////////////////

void EEStore::advance(int n){
  eeAddress+=n;
}

///////////////////////////////////////////////////////////////////////////////

void EEStore::reset(){
  eeAddress=sizeof(EEStoreData);
}
///////////////////////////////////////////////////////////////////////////////

int EEStore::pointer(){
  return(eeAddress);
}
///////////////////////////////////////////////////////////////////////////////

//EEStore *EEStore::eeStore=NULL;
EEStoreData EEStore::data;

int EEStore::eeAddress=0;

#endif
