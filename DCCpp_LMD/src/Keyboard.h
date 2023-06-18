#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "Config.h"
#include "DCCpp.h"
#ifdef USE_KEYBOARD


#define  MAX_COMMAND_LENGTH         30

/////////////////////////////////////////////////////////////////////////////////////
// https://forum.pololu.com/t/quadrature-encoder-counts/7410/10
// Activa el encoder:
//
  #define USE_ENCODER


/////////////////////////////////////////////////////////////////////////////////////
//
// Selección de teclado:
//
  #define USE_DIGITAL_4x3 
// #define USE_DGITAL_4x4 

/////////////////////////////////////////////////////////////////////////////////////
//
// Define el teclado, número de columnas y filas y si va conectado pin a pin o 
// de manera analógica con resistencias.


#ifdef USE_DIGITAL_4x3 
  #define PIN_ROW1 44
  #define PIN_ROW2 46
  #define PIN_ROW3 48
  #define PIN_ROW4 50 
  #define PIN_COL1 45 // 43
  #define PIN_COL2 47 // 45
  #define PIN_COL3 49 // 47
#endif

#ifdef USE_DIGITAL_4x4 
  #define PIN_ROW1 44
  #define PIN_ROW2 46
  #define PIN_ROW3 48
  #define PIN_ROW4 50 
  #define PIN_COL1 43
  #define PIN_COL2 45 
  #define PIN_COL3 47 
  #define PIN_COL4 49 
#endif


#ifdef USE_ENCODER
  
  #define PINCLK  51
  #define PINDT   53
  #define PINSW   52

#endif

enum CmdStatus {halt, loco, go, progread, progwrite};


class Keyboard {
  public:
    static void init();
    static void KeyLoop();
    static void KeyboardProcess(char);
    static char ReadKey();
    static void initEncoder();
    static int8_t Encoder(int8_t);




  private:

    static CmdStatus mStatus;
    static bool OnOff;
    static char rKey[MAX_COMMAND_LENGTH];

  // static uint8_t 
    
};
#endif  // USE_KEYBOARD
#endif  // _KEYBOARD_H_
