/**
 * Toda la información para conectar la central puedes encontrar
 * en http://lamaquetade.infotronikblog.com/conectando-dcc-con-wifi-esp8266.html
**/

#include "Arduino.h"
#include "DCCpp.h"
#include "Oled.h"
#ifdef USE_SERIALWIFI
#include "SerialWifi.h"
#include "CommInterface.h" 
#include "Config.h"
#include "TextCommand.h"

char Wifi::wfCommandString[WF_MAX_COMMAND_LENGTH+1];
int bytesEsperados = 0;    // Bytes que esperamos recibir
int conexionId = 0;        // ID de la conexión

bool clienteConectado[MAX_CLIENTES] = {false};  // Estado de cada cliente
int lastClient = -1;  // Último que envió datos

// Inicia la configuración del módulo
void Wifi::InitWifiModule()
/** Start configuration module with AT commands to ESP01.
    @param AT+RST ESP01 reset
    @param AT+CWLAP Shows available networks
    @param AT+CWJAP_CUR="ssid","pass"  Connect to the ssid
    @param AT+CIFSR Show IP address
    @param AT+CIPMUX=1 Configure multiple connections
    @param AT+CIPSERVER=1,2560 Start the server on the defined port
    */
{
  sendATData("AT+RST\r\n", 500);
  sendATData("AT+CWMODE=1\r\n", 1000);
  sendATData("AT+CWLAP\r\n", 5000); // Más tiempo para buscar redes default: 3000

  char connection[200];
  sprintf(connection, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD); 
  sendATData((String)connection, 5000);

  sendATData("AT+CIFSR\r\n", 2000); // Devuelve la dirección IP         
  delay (1500);
  sendATData("AT+CIPMUX=1\r\n", 2500); // Configura multiconexión         
  delay (1500);
  sendATData("AT+CIPSERVER=1,2560\r\n", 1500); // Pone el módulo en modo servidor con el puerto 2560
  //sendATData("AT+CIPSTATUS\r\n", 2000);
  delay(500);
  Wifi::getWifiIP();
}

void Wifi::sendATData(String command, const int timeout)
/** Send Serial AT commands to ESP01.
    @param command command sent to ESP01
    @param timeout Command timeout
    */
{
  // Serial.print(F("enviando datos: "));
  // Serial.println(command);
  Serial.println(command);
  WIFI.print(command);
	String response = "";                                             
                                     
  long int time = millis();                                      
  while( (time+timeout) > millis())                                 
  {      
    while(WIFI.available())                                      
    {
      char c = WIFI.read();
      response+=c;                                                  
    }
  }    
}

void Wifi::wifiProcess()
/** Lee los datos almacenados en el bufer hasta 
 * el siguiente retorno de carro
 */
{

  char w;
  WIFI.flush();
  while (WIFI.available()>0) 
  {    // while there is data on the serial line c= 
    String cliente = WIFI.readStringUntil('\n');
    Wifi::procesarLineaESP(cliente);
  }    
} // TextCommand:process

void Wifi::TextCommadParse(String command_DCC)
{	//The +1 is for the 0x00h Terminator
  int   ArrayLength = command_DCC.length()+1;    
  char  CharArray[ArrayLength];
  command_DCC.toCharArray(CharArray,ArrayLength);

  for (int character = 0; character < ArrayLength; character++)
  {
    if (CharArray[character] == '<')	// start of new command
      wfCommandString[0] = '\0';
    else if (CharArray[character] == '>') // end of new command
      TextCommand::parse(wfCommandString);
    else if (strlen(wfCommandString) < WF_MAX_COMMAND_LENGTH) // if btCommandString still has space, append character just read from serial line
    {
      int len = strlen(wfCommandString);
      wfCommandString[len] = CharArray[character];
      wfCommandString[len + 1] = '\0';  // Añadir terminador nulo     
    }
  }	// otherwise, character is ignored (but continue to look for '<' or '>')
}

void Wifi::procesarLineaESP(String linea) 
/** Procesa el comando AT del ESP01 donde esta la cabecera
 * que nos informa de un nuevo cliente y le asigna una ID
 * @param String linea: Lectura desde wifiProcess
 */
{
  linea.trim(); // Quitar \r\n
    
  // 1. Detectar nuevas conexiones
  if (linea.endsWith(",CONNECT")) 
  {
    int id = linea.substring(0, linea.indexOf(',')).toInt();
    clienteConectado[id] = true;
    lastClient = id;
    Serial.print(F("Cliente "));
    Serial.print(id);
    Serial.println(F(" conectado"));
 }
 else if (linea.endsWith(",CLOSED")) 
 {
    int id = linea.substring(0, linea.indexOf(',')).toInt();
    clienteConectado[id] = false;
    Serial.print(F("Cliente "));
    Serial.print(id);
    Serial.println(F(" bye"));

    // Si era el último, buscar otro activo
    if (id == lastClient) 
    {
      lastClient = isActiveClient();
    }
  }   // 3. Recibir datos de clientes (+IPD)
  else if (linea.startsWith("+IPD,")) 
  {
    // Formato: +IPD,ID,longitud:datos
    int idx1 = linea.indexOf(',');
    int idx2 = linea.indexOf(',', idx1 + 1);
    int idx3 = linea.indexOf(':');
      
    if (idx1 > 0 && idx2 > 0 && idx3 > 0) 
    {
      int id = linea.substring(idx1 + 1, idx2).toInt();
      int len = linea.substring(idx2 + 1, idx3).toInt();
      String datos = linea.substring(idx3 + 1);
        
      lastClient = id;  // Actualizar último activo
        

      // PROCESAR COMANDO DCC AQUÍ
      Wifi::TextCommadParse(datos);
        
    }
  }
}

void Wifi::sendDataToclient(int id, String command_DCC)
/** Send data to ID connected client
 * @param int id: Connection identifier
 * @param String command_DCC: DCC comand with \< and \>
 * @return none
 */
{
  if (!clienteConectado[id]) return;
  // Serial.print("sendDataToclient: ");
  // Serial.print(id);
  // Serial.print(" data: ");
  // Serial.println(command_DCC);  
  WIFI.print("AT+CIPSEND=");
  WIFI.print(id);
  WIFI.print(",");
  WIFI.print(command_DCC.length());
  WIFI.print("\r\n");
  delay(10);
  WIFI.print(command_DCC);
}

int Wifi::isActiveClient()
/** Busca los clientes activos en 
 * el array "clienteConectado[MAX_CLIENTES]"
 */
{
  for (int i = 0; i < MAX_CLIENTES; i++)
  {
    if (clienteConectado[i]) return i;
  }
  return -1;
}

void Wifi::broadcast(char *command_DCC)
/** Send data to all connected clients
 * @param char command_DCC
 * @return none
 */
{
  String mensaje = String(command_DCC);

  for (int i = 0; i < MAX_CLIENTES; i++) {
    if (clienteConectado[i]) {
      Wifi::sendDataToclient(i, mensaje);
      delay(5); // Pequeña pausa entre envíos
    }
  }  
}

String Wifi::getWifiIP()
/** AT commands to get IP from ESP01.
 * @return none
 */
{
  Serial.print(F("Buscando IP...")); 
  String data_ip;
  WIFI.write("AT+CIFSR\r\n"); // Devuelve la dirección IP
  long int time = millis();                                      
  while( (time + 1000) > millis())                                 
  {      
    while (WIFI.available()>0)
    {    // while there is data on the serial line c= 
      char ip = WIFI.read();
      if (ip != '\n') data_ip+=ip;
    }
  }
  // Método directo con indexOf y substring
  int inicio = data_ip.indexOf('"') + 1;  // Posición después de la primera comilla
  int fin = data_ip.indexOf('"', inicio); // Posición de la segunda comilla
  String ip_ = data_ip.substring(inicio, fin);

  int   ArrayLength = ip_.length()+1;    //The +1 is for the 0x00h Terminator
  char  CharArray[ArrayLength];
  ip_.toCharArray(CharArray,ArrayLength);
  CommManager::printf(CharArray);
  #ifdef USE_OLED
    Oled::printWifiIp(ip_);
  #endif
  return ip_;
}

#endif // USE_SERIALWIFI

/**
 * Información sobre los comandos AT:
 * 
 * AT+GMR -> Devuelve información sobre versión AT
 * AT+CWMODE? -> devuelve el modo 1)cliente 2)servidor 3)cliente/servidor
 * AT+CWMODE=1-3 -> devuelve OK
 * AT+CIPSTA? -> devuelve información de la conexión IP, Gateway, netmask
 * AT+CWLAP -> devuelve una lista con las conexiones disponibles, no funciona si no esta definido el modo. CWMODE=1 o CWMODE=3
 * AT+CWLIF -> Devuelve el AP al que estamos conectado
 * AT+RESTORE -> Resetea el modulo con valores de origen
 * 
 * Modo normal conectado al Router:
 * AT+CWMODE_CUR=1 -> modo cliente
 * AT+CWJAP_CUR:<ssid>,<bssid>,<channel>,<rssi>,<pci_en>
 * AT+CWJAP_CUR="SSID","PASSWORD"
 * char connection[200];
 * sprintf(connection, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD); 
 * sendATData((String)connection, 5000);
 * 
 * AT+CWMODE_DEF=2 -> modo servidor
 * AT+CWSAP_CUR="ssid","password", canal, codificacion WEP/WPA
 * AT+CWSAP_CUR="SSID","password"
 * AT+RST -> Reinicia el modulo.
*/