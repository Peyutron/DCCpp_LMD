//Wifi.h

#ifndef Wifi_h
#define Wifi_h

#include "Arduino.h"

#include "DCCpp.h"

#ifdef USE_SERIALWIFI

#define WIFI Serial1				/**< Puerto de comunicación serial con modulo WiFi ESP01 */
#define WF_MAX_COMMAND_LENGTH 	30	/**< Número de caracteres máximos que puede recibir. */


class Wifi {
public:
	static char wfCommandString[WF_MAX_COMMAND_LENGTH+1];
 	static void InitWifiModule();
 	static void wifiProcess();
 	static String getWifiIP();
 	static void broadcast(char *mensaje);

private:
 	static void sendATData(String, const int);
 	static int isActiveClient();
 	static void procesarLineaESP(String linea);
 	static void TextCommadParse(String command_DCC);
 	static void sendDataToclient(int id, String command_DCC);
};

#endif

#endif // USE_WIFIAT