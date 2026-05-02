/**
  * project: <DCCpp LMD>
  * author: <Carlos M.C>
  * Name: <DCCpp Animation>
  * GitHub: https://github.com/Peyutron/DCCpp_LMD
  * 
  * Description: Inicia la librería DCCppLMD con todos los seriales activados:
  * - Serial  - Comunicación USB
  * - LMD V2.1.1 Los Serialx.begin() se inician el SerialWifi.cpp, SerialBluetooth.cpp y SerialAux.cpp
  *   Para Activar/Desactivar los diferentes modulos de comunicación vaya al archivo src/DCCpp.h.
  *  - Serial1 -> USE_SERIALWIFI para Comunicación Wifi.
  *  - Serial2 -> USE_SERIALBLUETOOTH para Comunicación Bluetooth.
  *  - Serial3 -> USE_SERIALAUX para Comunicación Auxiliar.
  * 
  * Board: Arduino MEGA 2560
  * Shield: Ardumoto L298
  */

#include "DCCpp.h"

#ifndef USE_TEXTCOMMAND
#error To be able to compile this sample,the line #define USE_TEXTCOMMAND must be uncommented in DCCpp.h
#endif

void setup()
{
  Serial.begin(115200); // Inicia la comunicación serial USB 
 
  // LMD V2.1.1
  // Los Serialx.begin() se inician el SerialWifi.cpp, SerialBluetooth.cpp y SerialAux.cpp 
  // Serial1.begin(115200); // Inicia la comunicación serial para WiFi ( SOLO MEGA) 
  // Serial2.begin(115200); // Inicia la comunicación serial para Bluetooth ( SOLO MEGA) 
  // Serial3.begin(115200); // Inicia la comunicación serial para SerialAux ( SOLO MEGA) 
 
  DCCpp::begin(); 
  DCCpp::beginMainMotorShield(); // Inicia la vía principal motor Shield Ardumoto 
  DCCpp::beginProgMotorShield(); // Inicia la vía de programación en motor Shield Ardumoto
}

void loop()
{
  DCCpp::loop();  // Funcion del Loop principal
}
