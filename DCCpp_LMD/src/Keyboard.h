#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "Config.h"
#include "DCCpp.h"

/////////////////////////////////////////////////////////////////////////////////////
//
// Define el teclado, número de columnas y filas y si va conectado pin a pin o 
// de manera analógica con resistencias.


#define USE_DGITAL_4x3 1
#define USE_ANALOG_4x3 2
#define USE_DGITAL_4x4 3
#define USE_ANALOG_4x4 4




#ifdef USE_ANALOG_4x3 || USE_ANALOG_4x4
#define PIN_ROW1 6
#define PIN_ROW2
#define PIN_ROW3
#define PIN_ROW4
#define PIN_COL1
#define PIN_COL2
#define PIN_COL3
#ifdef USE_ANALOG_4x4
  #define PIN_COL4
#endif
#endif



class Keyboard {
  public:
    static void LocoAction();
    static void TurnoutAction();
    static void OutputAction();
    static void ActionErrorCurrent();
    static void ActionOK();
  private:
  static void ReadKey(int, int);
    
};
#endif
