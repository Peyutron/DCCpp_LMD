
/*    
 * Programa básico para librería LMDCC++ 
 * Para Activar/Desactivar los diferentes modulos vaya al archivo DCCpp.h 
 */

#include "DCCpp.h"

#ifndef USE_TURNOUT
#error To be able to compile this sample,the line #define USE_TURNOUT must be uncommented in DCCpp.h
#endif

Turnout T1, T2, T3, T4, T5, T6, T7, T8, T9, T10;
int storedata[10 + 1][3]{ {0, 0, 0},
                        {1, 1, 0}, {2, 1, 1}, {3, 1, 2}, {4, 1, 3},
                        {5, 2, 0}, {6, 2, 1}, {7, 2, 2}, {8, 2, 3},
                        {9, 3, 0}, {10, 3, 1}
                        };
  
void setup()
{
  
  Serial.begin(115200);   // Inicia la comunicación serial USB
  Serial.println(DCCPP_LIBRARY_VERSION); 
  
  DCCpp::begin();

  
  DCCpp::beginMainMotorShield(); // Inicia la vía principal motor Shield Ardumoto
  DCCpp::beginProgMotorShield(); // Inicia la vía de programación  en motor Shield Ardumoto

    T1.begin(storedata[1][0], storedata[1][1], storedata[1][2]);
    T2.begin(storedata[2][0], storedata[2][1], storedata[2][2]);
    T3.begin(storedata[3][0], storedata[3][1], storedata[3][2]);
    T4.begin(storedata[4][0], storedata[4][1], storedata[4][2]);
    T5.begin(storedata[5][0], storedata[5][1], storedata[5][2]);
    T6.begin(storedata[6][0], storedata[6][1], storedata[6][2]);
    T7.begin(storedata[7][0], storedata[7][1], storedata[7][2]);
    T8.begin(storedata[8][0], storedata[8][1], storedata[8][2]);
    T9.begin(storedata[9][0], storedata[9][1], storedata[9][2]);
    T10.begin(storedata[10][0], storedata[10][1], storedata[10][2]);

  
}

void loop()
{

  DCCpp::loop();  // Funcion del Loop principal

}
