
/*
 * Programa básico para librería DCCpp LMD con retromodulos S88
 * Para Activar/Desactivar los diferentes modulos vaya al archivo DCCpp.h 
 */

#include "DCCpp.h"

void setup()
{
  Serial.begin(115200);   // Inicia la comunicación serial USB
  // Serial1.begin(115200);  // Inicia la comunicacion serial para Wifi ( SOLO MEGA)
  // Serial2.begin(115200);  // Inicia la comunicacion serial para Bluetooth ( SOLO MEGA)
  // Serial3.begin(115200);  // Inicia la comunicacion serial para Auxiliar ( SOLO MEGA)

  #ifdef USE_S88                       // Les pins sont déclarées dans S88.h
  pinMode(S88_LOAD_PS_PIN, OUTPUT);  // THIS ARDUINO OUPUT PIN MUST BE PHYSICALLY CONNECTED TO THE S88 Load PIN
  pinMode(S88_Reset_PIN, OUTPUT);    // THIS ARDUINO OUPUT PIN MUST BE PHYSICALLY CONNECTED TO THE S88 Reset PIN
  pinMode(S88_Clock_PIN, OUTPUT);    // THIS ARDUINO OUPUT PIN MUST BE PHYSICALLY CONNECTED TO THE S88 Clock PIN
  pinMode(S88_DataL_PIN, INPUT);     // THIS ARDUINO OUPUT PIN MUST BE PHYSICALLY CONNECTED TO THE S88 DataL PIN (1st data in the buffer) with 10k pulldown on your board
  pinMode(S88_DataR_PIN, INPUT);     // THIS ARDUINO OUPUT PIN MUST BE PHYSICALLY CONNECTED TO THE S88 DataR PIN (last data in the buffer) with 10k pulldown on your board

  digitalWrite(S88_LOAD_PS_PIN, LOW);
  digitalWrite(S88_Clock_PIN, LOW);
  digitalWrite(S88_Reset_PIN, LOW);
#endif
  
  DCCpp::begin(); 
  DCCpp::beginMainMotorShield(); // Inicia la vía principal motor Shield Ardumoto
  DCCpp::beginProgMotorShield(); // Inicia la vía de programación  en motor Shield Ardumoto
}

void loop()
{
  DCCpp::loop();  // Funcion del Loop principal
}
