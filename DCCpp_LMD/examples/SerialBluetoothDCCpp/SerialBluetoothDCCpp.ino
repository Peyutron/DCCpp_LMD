
/*
 * Programa básico para librería DCC++ 
 * Para Activar/Desactivar los diferentes modulos vaya al archivo DCCpp.h 
 */

#include "DCCpp.h"

#ifndef USE_TEXTCOMMAND
#error To be able to compile this sample,the line #define USE_TEXTCOMMAND must be uncommented in DCCpp.h
#endif

void setup()
{
 Serial.begin(115200); // Inicia la comunicación serial USB 
 Serial1.begin(115200); // Inicia la comunicación serial para Bluetooth ( SOLO MEGA) 
 Serial2.begin(115200); // Inicia la comunicación serial para Bluetooth ( SOLO MEGA) 
 Serial3.begin(115200); // Inicia la comunicación serial para SerialAux ( SOLO MEGA) 
 DCCpp::begin(); 
 DCCpp::beginMainMotorShield(); // Inicia la vía principal motor Shield Ardumoto 
 DCCpp::beginProgMotorShield(); // Inicia la vía de programación en motor Shield Ardumoto
 
}

void loop()
{
  DCCpp::loop();  // Funcion del Loop principal
}
