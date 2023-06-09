/**********************************************************************

TextCommand.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "Arduino.h"

// See TextCommand::parse() below for defined text commands.
#include "CommInterface.h"
#include "TextCommand.h"
#include "Config.h"
#ifdef USE_TEXTCOMMAND


#ifdef VISUALSTUDIO
#include "string.h"
#include "iostream"
#else
extern unsigned int __heap_start;
extern void *__brkval;
#endif

String serialInUse[3] = {"no", "no", "no"}; // Para CommInterface en comando 's'
String info_vers;	// Para CommInterface en comando 's'
String info_serial;	// Para CommInterface en comando 's'
int nElemento[4] = {0, 0, 0}; 	// Para CommInterface en comando 'E'
int v = 0;						// Para CommInterface en comando 'F'
int memoria = 0;				// Para CommInterface en comando 'F'

///////////////////////////////////////////////////////////////////////////////

char TextCommand::commandString[MAX_COMMAND_LENGTH+1];

///////////////////////////////////////////////////////////////////////////////

void TextCommand::init(volatile RegisterList *_mRegs, volatile RegisterList *_pRegs, CurrentMonitor *_mMonitor){
  commandString[0] = 0;
} // TextCommand:TextCommand

///////////////////////////////////////////////////////////////////////////////

void TextCommand::process(){
	char c;
	#if defined(USE_ETHERNET)
		EthernetClient client= DCCPP_INTERFACE.available();
	if (client) {

		if (DCCppConfig::Protocol == EthernetProtocol::HTTP) {
			DCCPP_INTERFACE.println("HTTP/1.1 200 OK");
			DCCPP_INTERFACE.println("Content-Type: text/html");
			DCCPP_INTERFACE.println("Access-Control-Allow-Origin: *");
			DCCPP_INTERFACE.println("Connection: close");
			DCCPP_INTERFACE.println("");
		}

		while (client.connected() && client.available()) {        // while there is data on the network
			c = client.read();
			if (c == '<')                    // start of new command
				commandString[0] = 0;
			else if (c == '>'){               // end of new command
				if (parse(commandString) == false){
					#if defined(DCCPP_DEBUG_MODE)
						Serial.println("invalid command !");
					#endif
				}
			}
			else if (strlen(commandString) < MAX_COMMAND_LENGTH)    // if comandString still has space, append character just read from network
				sprintf(commandString, "%s%c", commandString, c);     // otherwise, character is ignored (but continue to look for '<' or '>')
		} // while

		if (DCCppConfig::Protocol == EthernetProtocol::HTTP)
			client.stop();
	} // if (client)
	
	#else  // SERIAL case

		while (DCCPP_INTERFACE.available()>0) {    // while there is data on the serial line
			c = DCCPP_INTERFACE.read();
			if (c == '<')                    // start of new command
				commandString[0] = 0;
			else if (c == '>')               // end of new command
				parse(commandString);
			else if (strlen(commandString) < MAX_COMMAND_LENGTH)	// if commandString still has space, append character just read from serial line
				sprintf(commandString, "%s%c", commandString, c);	// otherwise, character is ignored (but continue to look for '<' or '>')
		} // while
		
	#endif
} // TextCommand:process
   
///////////////////////////////////////////////////////////////////////////////

bool TextCommand::parse(char *com){

#ifdef DCCPP_DEBUG_MODE
	Serial.print(com[0]);
	Serial.println(F(" command"));
#ifdef VISUALSTUDIO
	std::cout << com <<  " command received" << std::endl; 
#endif
#endif

#ifdef USE_OLED
		// Serial.println(F(" command"));
	if (com[0] != 'c') Oled::Monitor(com);    // OLED para que no muestre el consumo con cada consulta
#endif

switch(com[0]){

#ifdef USE_S88      // decode XBPC command
    case 'Q':       // <Q>
    com[1] = ' ';
    com[2] = '6';
    com[3] = '4';
    com[4] = ' ';
    com[5] = '3';
    com[6] = '\0';

    //return true; // NUEVA LINEA 

    case 'Y':       // <Y Nb_S88_Modules DataFormat> para inicialización o <Y> para feedback de ocupación
/*
 *   <Y Nb_S88_Bytes DataFormat>:            sets Nb_S88_Modules read with output DataFormat
 *   <Y>                                     proporciona feedback de ocupación 
 *
 *   Nb_S88_Bytes: the byte number (0-64) to read
 *   DataFormat: 0 (Binary) 1 (Hexadecimal)
 *
 *   returns: <o status>, then <y S88_Bytes> or <q/Q n>
 *
 *   *** CONSULTE S88.CPP PARA OBTENER INFORMACIÓN COMPLETA SOBRE LAS DIFERENTES VARIACIONES DEL COMANDO "Y"
 *   used to define S88 retrosignalisation definitions
 */
      // DCCPP_INTERFACE.print("\n<y 00000000>"); or DCCPP_INTERFACE.print("\n<y 00>");
      S88::parse(com +1); // NUEVA LINEA 
      return true;

#endif





	case 't':       
		/**	\addtogroup commandsGroup
		ESTABLECE LOS ACELERADORES UTILIZANDO EL CONTROL DE VELOCIDAD DE 128 PASOS
		-------------------------------------------------

		<b>
		\verbatim
		<t REGISTER CAB SPEED DIRECTION>
		\endverbatim
		</b>
	
	   sets the throttle for a given register/cab combination 
	   
	   - <b>REGISTE%R</b>: an internal register number, from 1 through MAX_MAIN_REGISTERS (inclusive), to store the DCC packet used to control this throttle setting
	   - <b>CAB</b>:  the short (1-127) or long (128-10293) address of the engine decoder
	   - <b>SPEED</b>: throttle speed from 0-126, or -1 for emergency stop (resets SPEED to 0)
	   - <b>DIRECTION</b>: 1=forward, 0=reverse.  Setting direction when speed=0 or speed=-1 only effects directionality of cab lighting for a stopped train
	   
	   returns: <b>\<T REGISTE%R SPEED DIRECTION\></b>
	   */

	 	DCCpp::mainRegs.setThrottle(com+1);
		return true;

	case 'f':       
		/**	\addtogroup commandsGroup
		FUNCIONES DEL DECODIFICADOR DE OPERACIÓN DE LA MAQUINA F0-F28
		---------------------------------------
		
		<b>
		\verbatim
		<f CAB BYTE1 [BYTE2]>
		\endverbatim
		\verbatim
		<f -1 REGISTER CAB BYTE1 [BYTE2]>
		\endverbatim
		</b>

		turns on and off engine decoder functions F0-F28 (F0 is sometimes called FL)  
		NOTE: setting requests transmitted directly to mobile engine decoder --- current state of engine functions is not stored by this program
   
		- <b>REGISTE%R</b>: an internal register number, from 1 through MAX_MAIN_REGISTERS (inclusive), to store the DCC packet used to control these functions. REGISTER is only used if the first argument is -1.
		- <b>CAB</b>:  the short (1-127) or long (128-10293) address of the engine decoder
   
		To set functions F0-F4 on (=1) or off (=0):
     
		- <b>BYTE1</b>:  128 + F1*1 + F2*2 + F3*4 + F4*8 + F0*16
		- <b>BYTE2</b>:  omitted
  
		To set functions F5-F8 on (=1) or off (=0):
  
		- <b>BYTE1</b>:  176 + F5*1 + F6*2 + F7*4 + F8*8
		- <b>BYTE2</b>:  omitted
  
		To set functions F9-F12 on (=1) or off (=0):
  
		- <b>BYTE1</b>:  160 + F9*1 +F10*2 + F11*4 + F12*8
		- <b>BYTE2</b>:  omitted
  
		To set functions F13-F20 on (=1) or off (=0):
  
		- <b>BYTE1</b>: 222 
		- <b>BYTE2</b>: F13*1 + F14*2 + F15*4 + F16*8 + F17*16 + F18*32 + F19*64 + F20*128
  
		To set functions F21-F28 on (=1) of off (=0):
  
		- <b>BYTE1</b>: 223
		- <b>BYTE2</b>: F21*1 + F22*2 + F23*4 + F24*8 + F25*16 + F26*32 + F27*64 + F28*128
  
		returns: NONE
		*/

		DCCpp::mainRegs.setFunction(com+1);
	  	return true;

    case '#': // NUMBER OF LOCOSLOTS <#>
    	CommManager::printf("<# %d>\n", MAX_MAIN_REGISTERS);
        return true;

	case 'a':       
		/**	\addtogroup commandsGroup
		OPERAR DECODER DE ACCESORIOS ESTACIONARIO
		-------------------------------------
		
		<b>
		\verbatim
		<a ADDRESS SUBADDRESS ACTIVATE>
		\endverbatim
		</b>

		turns an accessory (stationary) decoder on or off
   
		- <b>ADDRESS</b>:  the primary address of the decoder (0-511)
		- <b>SUBADDRESS</b>: the sub-address of the decoder (0-3)
		- <b>ACTIVATE</b>: 1=on (set), 0=off (clear)
   
		Note that many decoders and controllers combine the ADDRESS and SUBADDRESS into a single number, N,
		from  1 through a max of 2044, where
   
		N = (ADDRESS - 1) * 4 + SUBADDRESS + 1, for all ADDRESS>0
   
		OR
   
		- <b>ADDRESS</b> = INT((N - 1) / 4) + 1
		- <b>SUBADDRESS</b> = (N - 1) % 4
   
		Sin embargo, este comando general simplemente envía el paquete de instrucciones DCC apropiado a la pista 
		principales para operar los accesorios conectados. No almacena ni retiene ninguna información relativa al
		estado de ese accesorio. Para que este croquis almacene y retenga la dirección de los desvíos conectados 
		a DCC, así como invocar automáticamente los<b>\<a\></b> comando según sea necesario, primero definir/editar/eliminar desvíos
		usando las siguientes variaciones del comando "T".

		returns: Sin respuesta
		*/

	 	DCCpp::mainRegs.setAccessory(com+1);
		return true;


#ifdef USE_TURNOUT
	case 'T':
/*
* *** VER TURNOUT.CPP PARA INFORMACIÓN COMPLETA SOBRE LAS DIFERENTES VARIACIONES DEL COMANDO "T"
* UTILIZADO PARA CREAR/EDITAR/ELIMINAR/MOSTRAR DEFINICIONES DE DESVIOS
*/

		return Turnout::parse(com+1);
#endif

#ifdef USE_OUTPUT

	case 'Z':
/**** SEE OUTPUT.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "Z" COMMAND
*   USED TO CREATE / EDIT / REMOVE / SHOW OUTPUT DEFINITIONS
*/

	  return Output::parse(com+1);
#endif

#ifdef USE_SENSOR
	  
	case 'S': 
/*   
 *   *** SEE SENSOR.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "S" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW SENSOR DEFINITIONS
 */
	return Sensor::parse(com+1);	  

#ifdef DCCPP_PRINT_DCCPP
#ifndef USE_S88
	case 'Q':
		/**	\addtogroup commandsGroup
		SHOW STATUS OF ALL SENSORS
		--------------------------

		<b>
		\verbatim
		<Q>
		\endverbatim
		</b>

		returns: the status of each sensor ID in the form <b>\<Q ID\></b> (active) or <b>\<q ID\></b> (not active)
		*/

	 	Sensor::status();
		return true;

#endif	// USE_S88
#endif	// DCCPP_PRINT_DCCPP
#endif	// USE_SENSOR


	case 'w':      
		
		/**	\addtogroup commandsGroup
		WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON MAIN OPERATIONS TRACK
		----------------------------------------------------------------------------

		<b>
		\verbatim
		<w CAB CV VALUE>
		\endverbatim
		</b>

		- Escribe, sin ninguna verificación, una variable de configuración en el decodificador de una locomotora 
			en la pista de operaciones principal    
		- <b>CAB</b>:  Dirección corta (1-127) o larga (128-10293) del decoder de la locomotora 
		- <b>CV</b>: número de la variable de configuracion  (1-1024)
		- <b>VALUE</b>: Valor que se escribira en la variable de configuración (0-255)
    
		returns: NONE
		*/    

	  DCCpp::mainRegs.writeCVByteMain(com+1);
		return true;


	case 'b':      
		/**	\addtogroup commandsGroup
		WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON MAIN OPERATIONS TRACK
		---------------------------------------------------------------------------

		<b>
		\verbatim
		<b CAB CV BIT VALUE>
		\endverbatim
		</b>

		writes, without any verification, a single bit within a Configuration Variable to the decoder of an engine on the main operations track
    
		- <b>CAB</b>:  the short (1-127) or long (128-10293) address of the engine decoder 
		- <b>CV</b>: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
		- <b>BIT</b>: the bit number of the Configuration Variable register to write (0-7)
		- <b>VALUE</b>: the value of the bit to be written (0-1)
    
		returns: NONE
		*/        

	  DCCpp::mainRegs.writeCVBitMain(com+1);
		return true;

	case 'W':      
		/**	\addtogroup commandsGroup
		WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON PROGRAMMING TRACK
		------------------------------------------------------------------------

		<b>
		\verbatim
		<W CV VALUE CALLBACKNUM CALLBACKSUB>
		\endverbatim
		</b>

		writes, and then verifies, a Configuration Variable to the decoder of an engine on the programming track
    
		- <b>CV</b>: número de la variable de configuracion  (1-1024)
		- <b>VALUE</b>: Valor que se escribira en la variable de configuración (0-255)
		
		- <b>CALLBACKNUM</b>: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
		
		- <b>CALLBACKSUB</b>: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function
    
		returns: <b>\<r CALLBACKNUM|CALLBACKSUB|CV Value\></b>
		where VALUE is a number from 0-255 as read from the requested CV, or -1 if verification read fails
		*/    

	  DCCpp::progRegs.writeCVByte(com+1);
		return true;

	case 'B':      
		/**	\addtogroup commandsGroup
		WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON PROGRAMMING TRACK
		-----------------------------------------------------------------------

		<b>
		\verbatim
		<B CV BIT VALUE CALLBACKNUM CALLBACKSUB>
		\endverbatim
		</b>

		writes, and then verifies, a single bit within a Configuration Variable to the decoder of an engine on the programming track
		
		- <b>CV</b>: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
		- <b>BIT</b>: the bit number of the Configuration Variable memory location to write (0-7)
		- <b>VALUE</b>: the value of the bit to be written (0-1)
		- <b>CALLBACKNUM</b>: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
		- <b>CALLBACKSUB</b>: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function
		
		returns: <b>\<r CALLBACKNUM|CALLBACKSUB|CV BIT VALUE\></b>
		where VALUE is a number from 0-1 as read from the requested CV bit, or -1 if verification read fails
		*/

	  DCCpp::progRegs.writeCVBit(com+1);
		return true;

	case 'R':     
		/**	\addtogroup commandsGroup
		READ CONFIGURATION VARIABLE BYTE FROM ENGINE DECODER ON PROGRAMMING TRACK
		-------------------------------------------------------------------------

		<b>
		\verbatim
		<R CV CALLBACKNUM CALLBACKSUB>
		\endverbatim
		</b>

		reads a Configuration Variable from the decoder of an engine on the programming track
		
		- <b>CV</b>: the number of the Configuration Variable memory location in the decoder to read from (1-1024)
		- <b>CALLBACKNUM</b>: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
		- <b>CALLBACKSUB</b>: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function
		
		returns: <b>\<r CALLBACKNUM|CALLBACKSUB|CV VALUE\></b>
		where VALUE is a number from 0-255 as read from the requested CV, or -1 if read could not be verified
		*/

	  DCCpp::progRegs.readCV(com+1);
		return true;

	case 'r':
		/**	\addtogroup commandsGroup
		READ CONFIGURATION VARIABLE BYTE FROM ENGINE DECODER ON MAIN TRACK
		-------------------------------------------------------------------------

		<b>
		\verbatim
		<r CV CALLBACKNUM CALLBACKSUB>
		\endverbatim
		</b>

		reads a Configuration Variable from the decoder of an engine on the main track

		- <b>CV</b>: the number of the Configuration Variable memory location in the decoder to read from (1-1024)
		- <b>CALLBACKNUM</b>: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
		- <b>CALLBACKSUB</b>: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function

		returns: <b>\<r CALLBACKNUM|CALLBACKSUB|CV VALUE\></b>
		where VALUE is a number from 0-255 as read from the requested CV, or -1 if read could not be verified
		*/

		DCCpp::mainRegs.readCV(com + 1);
		break;

	case '1':      
		/**	\addtogroup commandsGroup
		TURN ON POWER FROM MOTOR SHIELD TO TRACKS
		-----------------------------------------

		<b>
		\verbatim
		<1>
		\endverbatim
		</b>

		enables power from the motor shield to the main operations and programming tracks
    
		returns: <b>\<p1\></b>
		*/    

	  DCCpp::powerOn();
		return true;
		  
	case '0':     
		/**	\addtogroup commandsGroup
		TURN OFF POWER FROM MOTOR SHIELD TO TRACKS
		------------------------------------------

		<b>
		\verbatim
		<0>
		\endverbatim
		</b>

		disables power from the motor shield to the main operations and programming tracks
    
		returns: <b>\<p0\></b>
		*/

		DCCpp::powerOff();

		return true;

	case 'c':     
		/**	\addtogroup commandsGroup
		LEE LA CORRIENTE DE LA PISTA DE OPERACIONES ESPECIAL
		----------------------------------

		<b>
		\verbatim
		<c>
		\endverbatim
		</b>

		reads current being drawn on main operations track
    
		returns: <b>\<a CURRENT\> </b>
		where CURRENT = 0-1024, based on exponentially-smoothed weighting scheme
		*/
		CommManager::printf("<a%d>", int(DCCpp::getCurrentMain()));
		return true;

	case 's':

	if (digitalRead(DCCppConfig::SignalEnablePinProg) == LOW) // could check either PROG or MAIN
      CommManager::printf("<p0>");
    else
      CommManager::printf("<p1>");
	


	 for(int i=1;i<=MAX_MAIN_REGISTERS;i++){
		if(DCCpp::mainRegs.speedTable[i]==0)
			continue;
		if(DCCpp::mainRegs.speedTable[i]>0){
			CommManager::printf("<T%d %d 1>", i, DCCpp::mainRegs.speedTable[i]);

		} else {
			CommManager::printf("<T%d %d 0>", i, - DCCpp::mainRegs.speedTable[i]);

		}          
	}

	#ifdef USE_SERIALWIFI
		serialInUse[0] = (String)WIFI;
	#endif
	#ifdef USE_SERIALBLUETOOTH
		serialInUse[1] = (String)BLUETOOTH;
	#endif
	#ifdef USE_SERIALAUX
		serialInUse[2] = (String)SERIALAUX;
	#endif
		
	
	CommManager::printf("<iDCCpp-LMD LIBRARY BASE STATION FOR ARDUINO V-%s", DCCPP_LIBRARY_VERSION );
	CommManager::printf("<N Wifi: %s Bluetooth: %s Aux: %s>", serialInUse[0], serialInUse[1], serialInUse[2]);
	
	
	#ifdef USE_SERIALWIFI
		WIFI.println(info_vers);
		WIFI.println(info_serial);
	#endif
	#ifdef USE_SERIALBLUETOOTH
		BLUETOOTH.println(info_vers);
		BLUETOOTH.println(info_serial);
	#endif
	#ifdef USE_SERIALAUX
		SERIALAUX.println(info_vers);
		SERIALAUX.println(info_serial);
	#endif


#ifdef DCCPP_PRINT_DCCPP
#ifdef USE_TURNOUT
	  Turnout::show();
#endif
#ifdef USE_OUTPUT
	  Output::show();
#endif
#ifdef USE_SENSOR
	  Sensor::show();
#endif
#endif
	return true;


#ifdef USE_EEPROM
	case 'E':     
		/**	\addtogroup commandsGroup
		ALMACENA CONFIGURACION EL LA MEMORIA EEPROM
		------------------------

		<b>
		\verbatim
		<E>
		\endverbatim
		</b>

		Stores settings for turnouts and sensors EEPROM
    
		returns: <b>\<e nTurnouts nSensors\></b>
		*/
	 
		EEStore::store();

		#ifdef USE_TURNOUT
			nElemento[0] = EEStore::data.nTurnouts;
		#endif
		#ifdef USE_SENSOR
			nElemento[1] = EEStore::data.nSensors;
		#endif
		#ifdef USE_OUTPUT
			nElemento[2] = EEStore::data.nOutputs;
		#endif

		CommManager::printf("<e%d %d %d>", nElemento[0], nElemento[1], nElemento[2]);

		return true;

	case 'e':     
		/**	\addtogroup commandsGroup
		CLEAR SETTINGS IN EEPROM
		------------------------

		<b>
		\verbatim
		<e>
		\endverbatim
		</b>
		
		Limpia la memoria EEPROM
		
    
		returns: <b>\<O></b>
		*/
		EEStore::clear();
		CommManager::printf("<O>");

		return true;
#endif

	case ' ':
		/**	\addtogroup commandsGroup
		PRINT CARRIAGE RETURN IN SERIAL MONITOR WINDOW
		----------------------------------------------

		<b>
		\verbatim
		< >
		\endverbatim
		</b>

		simply prints a carriage return - useful when interacting with ArduIno through serial monitor window
    
		returns: a carriage return
		*/

	  DCCPP_INTERFACE.println("");
		return true;

///          
/// THE FOLLOWING COMMANDS ARE NOT NEEDED FOR NORMAL OPERATIONS AND ARE ONLY USED FOR TESTING AND DEBUGGING PURPOSES
/// PLEASE SEE SPECIFIC WARNINGS IN EACH COMMAND BELOW
///
case 'D':       
		/**	\addtogroup commandsGroup
		ENTER DIAGNOSTIC MODE
		---------------------

		<b>
		\verbatim
		<D>
		\endverbatim
		</b>

		changes the clock speed of the chip and the pre-scaler for the timers so that you can visually see the DCC signals flickering with an LED
		SERIAL COMMUNICATION WILL BE INTERUPTED ONCE THIS COMMAND IS ISSUED - MUST RESET BOARD OR RE-OPEN SERIAL WINDOW TO RE-ESTABLISH COMMS
		*/
		CommManager::printf("Entering Diagnostic Mode...");
		// Serial.println("\nEntering Diagnostic Mode...");
		delay(1000);

		DCCpp::setDebugDccMode();

		return true;

	case 'M':       
		/**	\addtogroup commandsGroup
		WRITE A DCC PACKE%T TO ONE OF THE REGISTERS DRIVING THE MAIN OPERATIONS TRACK
		-----------------------------------------------------------------------------

		<b>
		\verbatim
		<M REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
		\endverbatim
		</b>

		writes a DCC packet of two, three, four, or five hexadecimal bytes to a register driving the main operations track
		fOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
   
		- <b>REGISTE%R</b>: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTE%R=0) or write and store (if REGISTE%R>0) the packet 
		- <b>BYTE1</b>:  first hexadecimal byte in the packet
		- <b>BYTE2</b>:  second hexadecimal byte in the packet
		- <b>BYTE3</b>:  optional third hexadecimal byte in the packet
		- <b>BYTE4</b>:  optional fourth hexadecimal byte in the packet
		- <b>BYTE5</b>:  optional fifth hexadecimal byte in the packet
  
		returns: NONE   
		*/

	  DCCpp::mainRegs.writeTextPacket(com+1);
		return true;

	case 'P':       
		/**	\addtogroup commandsGroup
		WRITE A DCC PACKE%T TO ONE OF THE REGISTERS DRIVING THE MAIN OPERATIONS TRACK
		-----------------------------------------------------------------------------

		<b>
		\verbatim
		<P REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
		\endverbatim
		</b>

		writes a DCC packet of two, three, four, or five hexadecimal bytes to a register driving the programming track
		FOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
   
		- <b>REGISTE%R</b>: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTE%R=0) or write and store (if REGISTE%R>0) the packet 
		- <b>BYTE1</b>:  first hexadecimal byte in the packet
		- <b>BYTE2</b>:  second hexadecimal byte in the packet
		- <b>BYTE3</b>:  optional third hexadecimal byte in the packet
		- <b>BYTE4</b>:  optional fourth hexadecimal byte in the packet
		- <b>BYTE5</b>:  optional fifth hexadecimal byte in the packet
   
		returns: NONE   
		*/

	  DCCpp::progRegs.writeTextPacket(com+1);
		return true;
			
	

	case 'L':

		/**	\addtogroup commandsGroup
		LISTS BIT CONTENTS OF ALL INTERNAL DCC PACKE%T REGISTERS
		-------------------------------------------------------

		<b>
		\verbatim
		<L>
		\endverbatim
		</b>

		lists the packet contents of the main operations track registers and the programming track registers
		FOR DIAGNOSTIC AND TESTING USE ONLY
		*/
	  DCCPP_INTERFACE.println("");
	  for(Register *p = DCCpp::mainRegs.reg; p <= DCCpp::mainRegs.maxLoadedReg;p++){
		DCCPP_INTERFACE.print("M"); DCCPP_INTERFACE.print((int)(p - DCCpp::mainRegs.reg)); DCCPP_INTERFACE.print(":\t");
		DCCPP_INTERFACE.print((int)p); DCCPP_INTERFACE.print("\t");
		DCCPP_INTERFACE.print((int)(p->activePacket)); DCCPP_INTERFACE.print("\t");
		DCCPP_INTERFACE.print(p->activePacket->nBits); DCCPP_INTERFACE.print("\t");
		for(int i=0;i<10;i++){
		  DCCPP_INTERFACE.print(p->activePacket->buf[i],HEX); DCCPP_INTERFACE.print("\t");
		}
		DCCPP_INTERFACE.println("");
	  }
	  for(Register *p = DCCpp::progRegs.reg; p <= DCCpp::progRegs.maxLoadedReg;p++){
		DCCPP_INTERFACE.print("P"); DCCPP_INTERFACE.print((int)(p - DCCpp::progRegs.reg)); DCCPP_INTERFACE.print(":\t");
		DCCPP_INTERFACE.print((int)p); DCCPP_INTERFACE.print("\t");
		DCCPP_INTERFACE.print((int)p->activePacket); DCCPP_INTERFACE.print("\t");
		DCCPP_INTERFACE.print(p->activePacket->nBits); DCCPP_INTERFACE.print("\t");
		for(int i=0;i<10;i++){
		  DCCPP_INTERFACE.print(p->activePacket->buf[i],HEX); DCCPP_INTERFACE.print("\t");
		}
		DCCPP_INTERFACE.println("");
	 }
	  DCCPP_INTERFACE.println("");
		return true;
	#ifdef USE_SERIALWIFI	
		case 'I':
		#ifdef USE_OLED
			Oled::printWifiIp(com + 1);
		#endif
			return true;
	#endif

	
	case 'F':     
				

	  	#ifdef ARDUINO_ARCH_AVR
	  			memoria = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
 				CommManager::printf("<f%d>", memoria);
 			#endif
 			#ifdef USE_OLED
			Oled::printSram(memoria);
			#endif 

		//v++;			// not used. This line is just here to avoid a warning by the compiler !
		return true;

	}	// switch

	return false;
}; // SerialCommand::parse


///////////////////////////////////////////////////////////////////////////////

#endif
