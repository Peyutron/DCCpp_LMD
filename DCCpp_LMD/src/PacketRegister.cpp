/**********************************************************************

PacketRegister.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman
Adapted by Thierry PARIS

Part of DCC++ BASE STATION for the Arduino
**********************************************************************/

#include "Arduino.h"

//static byte ackThreshold = 30;

#include "DCCpp.h"
#include "CommInterface.h"

//#include "DCCpp_Uno.h"
//#include "PacketRegister.h"
//#include "Comm.h"

#ifdef USE_ETHERNET
uint8_t DCCppConfig::EthernetIp[4];
uint8_t DCCppConfig::EthernetMac[6];
int DCCppConfig::EthernetPort = 0;

EthernetProtocol DCCppConfig::Protocol = EthernetProtocol::TCP;
#endif

byte DCCppConfig::SignalEnablePinMain = UNDEFINED_PIN;
byte DCCppConfig::CurrentMonitorMain = UNDEFINED_PIN;
					
byte DCCppConfig::SignalEnablePinProg = UNDEFINED_PIN;
byte DCCppConfig::CurrentMonitorProg = UNDEFINED_PIN;
					
#ifndef USE_ONLY1_INTERRUPT
byte DCCppConfig::DirectionMotorA = UNDEFINED_PIN;
byte DCCppConfig::DirectionMotorB = UNDEFINED_PIN;
#else
uint8_t	DCCppConfig::SignalPortMaskMain = 0;
uint8_t	DCCppConfig::SignalPortMaskProg = 0;

volatile uint8_t *DCCppConfig::SignalPortInMain = 0;
volatile uint8_t *DCCppConfig::SignalPortInProg = 0;
#endif

///////////////////////////////////////////////////////////////////////////////

void Register::initPackets(){
  activePacket=packet;
  updatePacket=packet+1;
} // Register::initPackets

///////////////////////////////////////////////////////////////////////////////
	
RegisterList::RegisterList(int maxNumRegs){
  this->maxNumRegs=maxNumRegs;
  reg=(Register *)calloc((maxNumRegs+1),sizeof(Register));
  for(int i=0;i<=maxNumRegs;i++)
	reg[i].initPackets();
  regMap=(Register **)calloc((maxNumRegs+1),sizeof(Register *));
  speedTable=(int *)calloc((maxNumRegs+1),sizeof(int *));
  currentReg=reg;
  regMap[0]=reg;
  maxLoadedReg=reg;
  nextReg=NULL;
  currentBit=0;
  nRepeat=0;
} // RegisterList::RegisterList
  
///////////////////////////////////////////////////////////////////////////////

// LOAD DCC PACKET INTO TEMPORARY REGISTER 0, OR PERMANENT REGISTERS 1 THROUGH DCC_PACKET_QUEUE_MAX (INCLUSIVE)
// CONVERTS 2, 3, 4, OR 5 BYTES INTO A DCC BIT STREAM WITH PREAMBLE, CHECKSUM, AND PROPER BYTE SEPARATORS
// BITSTREAM IS STORED IN UP TO A 10-BYTE ARRAY (USING AT MOST 76 OF 80 BITS)

void RegisterList::loadPacket(int nReg, byte *b, int nBytes, int nRepeat, int printFlag) volatile 
{  
#ifdef VISUALSTUDIO
  return;
#endif
  nReg=nReg%((maxNumRegs+1));          // obliga a nReg a estar entre 0 y maxNumRegs, inclusive

  while(nextReg!=NULL);              // pause while there is a Register already waiting to be updated -- nextReg will be reset to NULL by interrupt when prior Register updated fully processed
 
  if(regMap[nReg]==NULL)              // primera vez que se llama a este número de registro
   regMap[nReg]=maxLoadedReg+1;       // set Register Pointer for this Register Number to next available Register
 
  Register *r=regMap[nReg];           // establece Registro para ser actualizado
  Packet *p=r->updatePacket;          // establece el paquete en el registro para que se actualice
  byte *buf=p->buf;                   // set byte buffer in the Packet to be updated
		  
  b[nBytes]=b[0];                        // copy first byte into what will become the checksum byte  
  for(int i=1;i<nBytes;i++)              // XOR remaining bytes into checksum byte
	b[nBytes]^=b[i];
  nBytes++;                              // increment number of bytes in packet to include checksum byte
	  
  buf[0]=0xFF;                        // first 8 bytes of 22-byte preamble
  buf[1]=0xFF;                        // second 8 bytes of 22-byte preamble
  buf[2]=0xFC + bitRead(b[0],7);      // last 6 bytes of 22-byte preamble + data start bit + b[0], bit 7
  buf[3]=b[0]<<1;                     // b[0], bits 6-0 + data start bit
  buf[4]=b[1];                        // b[1], all bits
  buf[5]=b[2]>>1;                     // b[2], bits 7-1
  buf[6]=b[2]<<7;                     // b[2], bit 0
  
  if(nBytes==3){
	p->nBits=49;
  } else{
	buf[6]+=b[3]>>2;                  // b[3], bits 7-2
	buf[7]=b[3]<<6;                   // b[3], bit 1-0
	if(nBytes==4){
	  p->nBits=58;
	} else{
	  buf[7]+=b[4]>>3;                // b[4], bits 7-3
	  buf[8]=b[4]<<5;                 // b[4], bits 2-0
	  if(nBytes==5){
		p->nBits=67;
	  } else{
		buf[8]+=b[5]>>4;              // b[5], bits 7-4
		buf[9]=b[5]<<4;               // b[5], bits 3-0
		p->nBits=76;
	  } // >5 bytes
	} // >4 bytes
  } // >3 bytes
  
  nextReg=r;
  this->nRepeat=nRepeat;
  maxLoadedReg=max(maxLoadedReg,nextReg);
  
#ifdef DCCPP_DEBUG_MODE
#ifdef DCCPP_DEBUG_VERBOSE_MODE
  if(printFlag)       // for debugging purposes
		printPacket(nReg,b,nBytes,nRepeat);  
#endif
#endif

} // RegisterList::loadPacket

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setThrottle(int nReg, int cab, int tSpeed, int tDirection) volatile 
{
	byte b[5];                      // save space for checksum byte
	byte nB = 0;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convierte el número de tren en una dirección de dos bytes

	b[nB++] = lowByte(cab);
	b[nB++] = 0x3F;                        // byte de control de velocidad de 128 pasos
	if (tSpeed >= 0)
		b[nB++] = tSpeed + (tSpeed>0) + tDirection * 128;   // max speed is 126, but speed codes range from 2-127 (0=stop, 1=emergency stop)
	else {
		b[nB++] = 1;
		tSpeed = 0;
	}

	loadPacket(nReg, b, nB, 0, 1);
	CommManager::printf("<T%d %d %d %d>", nReg, cab, tSpeed, tDirection);	 
	#ifdef USE_OLED
		Oled::GetThrottle( cab ,tSpeed, tDirection);    // OLED
	#endif

	speedTable[nReg] = tDirection == 1 ? tSpeed : -tSpeed;

} // RegisterList::setThrottle(ints)

#ifdef USE_TEXTCOMMAND
void RegisterList::setThrottle(const char *s) volatile
{
  int nReg;
  int cab;
  int tSpeed;
  int tDirection;
  
  if (sscanf(s, "%d %d %d %d", &nReg, &cab, &tSpeed, &tDirection) != 4)
  {
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("t Syntax error");
		#endif
#ifdef DCCPP_DEBUG_MODE
    Serial.println(F("t Syntax error"));
#endif
	return;
  }

  this->setThrottle(nReg, cab, tSpeed, tDirection);
} // RegisterList::setThrottle(string)
#endif

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setFunction(int nReg, int cab, int fByte, int eByte) volatile
{
	byte b[5];                      // save space for checksum byte
	byte nB = 0;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

	b[nB++] = lowByte(cab);

	if (eByte < 0) {                      // this is a request for functions FL,F1-F12  
		b[nB++] = (fByte | 0x80) & 0xBF;     // for safety this guarantees that first nibble of function byte will always be of binary form 10XX which should always be the case for FL,F1-F12  
	}
	else {                             // this is a request for functions F13-F28
		b[nB++] = (fByte | 0xDE) & 0xDF;     // for safety this guarantees that first byte will either be 0xDE (for F13-F20) or 0xDF (for F21-F28)
		b[nB++] = eByte;
	}
	CommManager::printf("<F%d %d %d %d>", nReg, cab, fByte, eByte);	 

	/* La norma NMRA DCC solicita dos paquetes DCC en lugar de uno solo:
	"Command Stations genera estos paquetes y que no actualizan periódicamente estas funciones,
	debe enviar al menos dos repeticiones de estos comandos cuando se cambia el estado de cualquier función.."
	https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
	*/
	loadPacket(nReg, b, nB, 4, 1);
} // RegisterList::setFunction(ints)

#ifdef USE_TEXTCOMMAND
void RegisterList::setFunction(const char *s) volatile
{
	Serial.println(s);
	int reg, cab;
	int fByte, eByte;

	int a, b, c, d, e;

	switch (sscanf(s, "%d %d %d %d %d", &a, &b, &c, &d, &e))
	{
	case 2:
		reg = 0;
		cab = a;
		fByte = b;
		eByte = -1;
		break;

	case 3:
		reg = 0;
		cab = a;
		fByte = b;
		eByte = c;
		break;
	case 4:
		reg = b;
		cab = c;
		fByte = d;
		eByte = -1;
		break;
	case 5:
		reg = b;
		cab = c;
		fByte = d;
		eByte = e;
		break;

	default:
		Serial.println(F("f Syntax error functions"));
	#ifdef USE_SOUND
		Sound::ActionError();
	#endif
	#ifdef USE_OLED
		Oled::Monitor("func Syntax error");
	#endif
		return;
	}

	this->setFunction(reg, cab, fByte, eByte);

} // RegisterList::setFunction(string)
#endif

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::setAccessory(int aAdd, int aNum, int activate) volatile 
{
	byte b[3];                      // save space for checksum byte

	b[0] = aAdd % 64 + 128;         // first byte is of the form 10AAAAAA, where AAAAAA represent 6 least significant bits of accessory address  
	b[1] = ((((aAdd / 64) % 8) << 4) + (aNum % 4 << 1) + activate % 2) ^ 0xF8;      // second byte is of the form 1AAACDDD, where C should be 1, and the least significant D represent activate/deactivate

	loadPacket(0, b, 2, 4, 1);

} // RegisterList::setAccessory(ints)

#ifdef USE_TEXTCOMMAND
void RegisterList::setAccessory(const char *s) volatile
{
	int aAdd;                       // la dirección del accesorio (0-511 = 9 bits)
	int aNum;                       // el número de accesorio dentro de esa dirección (0-3)
	int activate;                   // flag que indica si el accesorio debe activarse (1) o desactivarse (0) siguiendo la convención recomendada por NMRA
	if (sscanf(s, "%d %d %d", &aAdd, &aNum, &activate) != 3)
	{
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
#ifdef DCCPP_DEBUG_MODE
		Serial.println(F("a Syntax error"));
#endif
		return;
	}

	this->setAccessory(aAdd, aNum, activate);
	#ifdef USE_OLED
		Oled::GetAccesories(aAdd, aNum, activate); // OLED
	#endif
	#ifdef USE_SOUND
		Sound::ActionOK();
	#endif

} // RegisterList::setAccessory(string)
#endif

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeTextPacket(int nReg, byte *b, int nBytes) volatile 
{
	if (nBytes<2 || nBytes>5) {    // invalid valid packet
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		CommManager::printf("<mInvalid Packet>");
		#if !defined(USE_ETHERNET)
			DCCPP_INTERFACE.println("");
		#endif
		return;
	}

	loadPacket(nReg, b, nBytes, 0, 1);

} // RegisterList::writeTextPacket(bytes)

#ifdef USE_TEXTCOMMAND
void RegisterList::writeTextPacket(const char *s) volatile
{
	int nReg;
	byte b[6];
	int nBytes;

	nBytes = sscanf(s, "%d %hhx %hhx %hhx %hhx %hhx", &nReg, b, b + 1, b + 2, b + 3, b + 4) - 1;

	this->writeTextPacket(nReg, b, nBytes);

} // RegisterList::writeTextPacket(string)
#endif

  ///////////////////////////////////////////////////////////////////////////////

int RegisterList::buildBaseAcknowlegde(int inMonitorPin) volatile
{
	int base = 0;
	for (int j = 0; j < ACK_BASE_COUNT; j++)
	{
		int val = (int)analogRead(inMonitorPin);
		base += val;
	}

	return base / ACK_BASE_COUNT;
}

bool RegisterList::checkAcknowlegde(int inMonitorPin, int inBase) volatile
{
	int c = 0;
	int max = 0;

#if defined(ARDUINO_ARCH_ESP32)
	int loopMax = 20;
#else
	int loopMax = 1;
#endif

#ifdef DCCPP_DEBUG_MODE
	int loop = 0;
#endif
	for (int a = 0; a < loopMax; a++)
	{
		c = 0;
		for (int j = 0; j < ACK_SAMPLE_COUNT; j++)
		{
			int val = (int)analogRead(inMonitorPin);
			c = (int)((val - inBase) * ACK_SAMPLE_SMOOTHING + c * (1.0 - ACK_SAMPLE_SMOOTHING));
			if (c > max)
			{
				max = c;
#ifdef DCCPP_DEBUG_MODE
				loop = a;
#endif
			}
		}
	}

#ifdef DCCPP_DEBUG_MODE
	Serial.print(F(" iter : "));
	Serial.print(loop);
	Serial.print(F(", max : "));
	Serial.println(max);
#endif

	return (max > ACK_SAMPLE_THRESHOLD);
}

int RegisterList::readCVraw(int cv, int callBack, int callBackSub) volatile
{
	byte bRead[4];
	int bValue;
	int ret, base;

	cv--;   // actual CV addresses are cv-1 (0-1023)

	byte MonitorPin = DCCppConfig::CurrentMonitorProg;
	if (DCCpp::IsMainTrack(this))
		MonitorPin = DCCppConfig::CurrentMonitorMain;

	// A read cannot be done if a monitor pin is not defined !
	if (MonitorPin == UNDEFINED_PIN)
		return -1;

#ifdef DCCPP_DEBUG_MODE
	CommManager::printf("readCVraw : start reading cv %d", cv+1);
	// Serial.print(F("readCVraw : start reading cv "));
	// Serial.println(cv+1);

#endif

	bRead[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	bRead[1] = lowByte(cv);

	bValue = 0;

	for (int i = 0; i<8; i++) {

		base = RegisterList::buildBaseAcknowlegde(MonitorPin);

#if defined(ARDUINO_ARCH_ESP32)
		delay(10);
#endif

		bRead[2] = 0xE8 + i;

		loadPacket(0, resetPacket, 2, 3);			// NMRA recomienda comenzar con 3 paquetes de reinicio
		loadPacket(0, bRead, 3, 5);           // NMRA recomienda 5 paquetes de verificación
		//loadPacket(0, resetPacket, 2, 1);   // obliga al código a esperar hasta que se completen todas las repeticiones de bRead (y el decodificador comience a responder)
		loadPacket(0, idlePacket, 2, 6);      // NMRA recomienda 6 paquetes inactivos o de reinicio para el tiempo de recuperación del decodificador

#if defined(ARDUINO_ARCH_ESP32)
		delay(2);
#endif

		ret = RegisterList::checkAcknowlegde(MonitorPin, base);

		bitWrite(bValue, i, ret);
	}

#if defined(ARDUINO_ARCH_ESP32)
		delay(10);
#endif

	base = RegisterList::buildBaseAcknowlegde(MonitorPin);

#if defined(ARDUINO_ARCH_ESP32)
		delay(10);
#endif

	bRead[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte
	bRead[2] = bValue;

	loadPacket(0, resetPacket, 2, 3);       // NMRA recomienda comenzar con 3 paquetes de reinicio
	loadPacket(0, bRead, 3, 5);             // NMRA recomienda 5 paquetes de verificación
	//loadPacket(0, resetPacket, 2, 1);     // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
	loadPacket(0, idlePacket, 2, 6);				// NMRA recomienda 6 paquetes inactivos o de reinicio para el tiempo de recuperación del decodificador

#if defined(ARDUINO_ARCH_ESP32)
		delay(2);
#endif

	ret = RegisterList::checkAcknowlegde(MonitorPin, base);

	if (ret == 0)    // verificación sin éxito
		bValue = -1;


CommManager::printf("<r%d|%d|%d %d>", callBack, callBackSub, cv + 1, bValue);	 

#ifdef DCCPP_DEBUG_MODE
	Serial.println(F("end reading"));
#endif
	return bValue;
}

int RegisterList::readCV(int cv, int callBack, int callBackSub) volatile 
{
	return RegisterList::readCVraw(cv, callBack, callBackSub);
} // RegisterList::readCV(ints)

#ifdef USE_TEXTCOMMAND
int RegisterList::readCV(const char *s) volatile
{
	int cv, callBack, callBackSub;

	if (sscanf(s, "%d %d %d", &cv, &callBack, &callBackSub) != 3)          // cv = 1-1024
	{	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
#ifdef DCCPP_DEBUG_MODE
		Serial.println(F("R Syntax error"));
#endif
		return -1;
	}

	return this->readCV(cv, callBack, callBackSub);
} // RegisterList::readCV(string)
#endif

int RegisterList::readCVmain(int cv, int callBack, int callBackSub) volatile
{
	return RegisterList::readCVraw(cv, callBack, callBackSub);

} // RegisterList::readCV_Main()

#ifdef USE_TEXTCOMMAND
int RegisterList::readCVmain(const char *s) volatile
{
	int cv, callBack, callBackSub;

	if (sscanf(s, "%d %d %d", &cv, &callBack, &callBackSub) != 3){          // cv = 1-1024
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
		#ifdef DCCPP_DEBUG_MODE
			CommManager::printf("r Syntax error");
		#endif
		return -1;
	}
   
	return this->readCVmain(cv, callBack, callBackSub);
} // RegisterList::readCVmain(string)
#endif

///////////////////////////////////////////////////////////////////////////////

bool RegisterList::writeCVByte(int cv, int bValue, int callBack, int callBackSub) volatile 
{
	byte bWrite[4];
	bool ok = false;
	int ret, base;

	cv--;                              // actual CV addresses are cv-1 (0-1023)

	bWrite[0] = 0x7C + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	bWrite[1] = lowByte(cv);
	bWrite[2] = bValue;

	loadPacket(0, resetPacket, 2, 3);        // NMRA recomienda comenzar con 3 paquetes de reinicio
	loadPacket(0, bWrite, 3, 5);             // NMRA recomienda 5 paquetes de verificación
	loadPacket(0, bWrite, 3, 6);             // NMRA recomienda 6 paquetes de escritura o reinicio para el tiempo de recuperación del decodificador

	/*loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, bWrite, 3, 4);
	loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, idlePacket, 2, 10);*/

	// Si el pin del monitor no está definido, escribira cv sin confirmación...
	if (DCCppConfig::CurrentMonitorProg != UNDEFINED_PIN){

		base = RegisterList::buildBaseAcknowlegde(DCCppConfig::CurrentMonitorProg);

		bWrite[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte

		loadPacket(0, resetPacket, 2, 3);          // NMRA recomienda comenzar con 3 paquetes de reinicio
		loadPacket(0, bWrite, 3, 5);               // NNMRA recomienda 5 paquetes de verificación
		//loadPacket(0, resetPacket, 2, 1);          // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
		loadPacket(0, bWrite, 3, 6);               // NMRA recomienda 6 paquetes de escritura o reinicio para el tiempo de recuperación del decodificador

		ret = RegisterList::checkAcknowlegde(DCCppConfig::CurrentMonitorProg, base);

		loadPacket(0, resetPacket, 2, 1);        // Final reset packet (and decoder begins to respond)

		if (ret != 0)    // verify successful
			ok = true;
	}
	
	CommManager::printf("<r%d|%d|%d %d>", callBack, callBackSub, cv + 1, bValue);	 

	return ok;
} // RegisterList::writeCVByte(ints)

#ifdef USE_TEXTCOMMAND
bool RegisterList::writeCVByte(const char *s) volatile
{
	int bValue, cv, callBack, callBackSub;

	if (sscanf(s, "%d %d %d %d", &cv, &bValue, &callBack, &callBackSub) != 4)          // cv = 1-1024
	{
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
		#ifdef DCCPP_DEBUG_MODE
			CommManager::printf("W Syntax error");
		#endif
		return false;
	}

	return this->writeCVByte(cv, bValue, callBack, callBackSub);
} // RegisterList::writeCVByte(string)
#endif

  ///////////////////////////////////////////////////////////////////////////////

bool RegisterList::writeCVBit(int cv, int bNum, int bValue, int callBack, int callBackSub) volatile 
{
	byte bWrite[4];
	bool ok = false;
	int ret, base;

	cv--;                              // actual CV addresses are cv-1 (0-1023)
	bValue = bValue % 2;
	bNum = bNum % 8;

	bWrite[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	bWrite[1] = lowByte(cv);
	bWrite[2] = 0xF0 + bValue * 8 + bNum;

	/*loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, bWrite, 3, 4);
	loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, idlePacket, 2, 10);*/

	loadPacket(0, resetPacket, 2, 3);        // NMRA recomienda comenzar con 3 paquetes de reinicio
	loadPacket(0, bWrite, 3, 5);             // NMRA recomienda 5 paquetes de verificación
	loadPacket(0, bWrite, 3, 6);             // NMRA recomienda 6 paquetes de escritura o reinicio para el tiempo de recuperación del decodificador

	// If monitor pin undefined, write cv without any confirmation...
	if (DCCppConfig::CurrentMonitorProg != UNDEFINED_PIN)
	{
		base = RegisterList::buildBaseAcknowlegde(DCCppConfig::CurrentMonitorProg);

		bitClear(bWrite[2], 4);              // change instruction code from Write Bit to Verify Bit

		loadPacket(0, resetPacket, 2, 3);      // NMRA recomienda comenzar con 3 paquetes de reinicio
		loadPacket(0, bWrite, 3, 5);           // NMRA recomienda 5 paquetes de verificación
		//loadPacket(0, resetPacket, 2, 1);    // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
		loadPacket(0, bWrite, 3, 6);           // NMRA recomienda 6 paquetes de escritura o reinicio para el tiempo de recuperación del decodificador

		ret = RegisterList::checkAcknowlegde(DCCppConfig::CurrentMonitorProg, base);

		loadPacket(0, resetPacket, 2, 1);      // Paquete de reinicio final completado (y el decodificador comienza a responder)

		if (ret != 0)    // Verificación exitosa
			ok = true;
	}

	CommManager::printf("<r%d|%d|%d %d %d>", callBack, callBackSub, cv + 1, bNum, bValue);
	return ok;
} // RegisterList::writeCVBit(ints)

#ifdef USE_TEXTCOMMAND
bool RegisterList::writeCVBit(const char *s) volatile {
  int bNum, bValue, cv, callBack, callBackSub;

  if(sscanf(s,"%d %d %d %d %d",&cv,&bNum,&bValue,&callBack,&callBackSub) != 5)          // cv = 1-1024
  {
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
		#ifdef DCCPP_DEBUG_MODE
	  		CommManager::printf(F("W Syntax error"));
		#endif
	  	return false;
  }

  return this->writeCVBit(cv, bNum, bValue, callBack, callBackSub);
} // RegisterList::writeCVBit(string)
#endif

///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVByteMain(int cab, int cv, int bValue) volatile 
{
	byte b[6];                      // save space for checksum byte
	byte nB = 0;

	cv--;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convierte el número de tren en una dirección de dos bytes

	b[nB++] = lowByte(cab);
	b[nB++] = 0xEC + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	b[nB++] = lowByte(cv);
	b[nB++] = bValue;

	loadPacket(0, b, nB, 4);

} // RegisterList::writeCVByteMain(ints)

#ifdef USE_TEXTCOMMAND
void RegisterList::writeCVByteMain(const char *s) volatile
{
	int cab;
	int cv;
	int bValue;

	if (sscanf(s, "%d %d %d", &cab, &cv, &bValue) != 3)
	{
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
		#ifdef DCCPP_DEBUG_MODE
			CommManager::printf(F("w Syntax error"));
		#endif
		return;
	}

	this->writeCVByteMain(cab, cv, bValue);
} // RegisterList::writeCVByteMain(string)
#endif

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVBitMain(int cab, int cv, int bNum, int bValue) volatile 
{
	byte b[6];                      // save space for checksum byte
	byte nB = 0;

	cv--;

	bValue = bValue % 2;
	bNum = bNum % 8;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convertir el número de tren en una dirección de dos bytes

	b[nB++] = lowByte(cab);
	b[nB++] = 0xE8 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	b[nB++] = lowByte(cv);
	b[nB++] = 0xF0 + bValue * 8 + bNum;

	loadPacket(0, b, nB, 4);

} // RegisterList::writeCVBitMain(ints)

#ifdef USE_TEXTCOMMAND
void RegisterList::writeCVBitMain(const char *s) volatile
{
	int cab;
	int cv;
	int bNum;
	int bValue;

	if (sscanf(s, "%d %d %d %d", &cab, &cv, &bNum, &bValue) != 4)
	{
		CommManager::printf("<X>");	 	
		#ifdef USE_SOUND
			Sound::ActionError();
		#endif
		#ifdef USE_OLED
			Oled::Monitor("Syntax error");
		#endif
#ifdef DCCPP_DEBUG_MODE
		CommManager::printf(F("w Syntax error"));
#endif
		return;
	}

	this->writeCVBitMain(cab, cv, bNum, bValue);
} // RegisterList::writeCVBitMain(string)
#endif

///////////////////////////////////////////////////////////////////////////////

#ifdef DCCPP_DEBUG_MODE
void RegisterList::printPacket(int nReg, byte *b, int nBytes, int nRepeat) volatile 
{  
	DCCPP_INTERFACE.print(F("<*"));
	DCCPP_INTERFACE.print(nReg);
	DCCPP_INTERFACE.print(F(":"));
  for(int i=0;i<nBytes;i++){
	  DCCPP_INTERFACE.print(F(" "));
	  DCCPP_INTERFACE.print(b[i],HEX);
  }
  DCCPP_INTERFACE.print(F(" / "));
  DCCPP_INTERFACE.print(nRepeat);
  DCCPP_INTERFACE.print(F(">");
#if !defined(USE_ETHERNET)
  DCCPP_INTERFACE.println(F("");
#endif
} // RegisterList::printPacket()
#endif

///////////////////////////////////////////////////////////////////////////////

byte RegisterList::idlePacket[3]={0xFF,0x00,0};	// siempre deje un byte adicional para el cálculo de la suma de comprobación
byte RegisterList::resetPacket[3]={0x00,0x00,0};

byte RegisterList::bitMask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01}; // máscaras utilizadas en la rutina de interrupción para acelerar la consulta de un solo bit en un paquete
