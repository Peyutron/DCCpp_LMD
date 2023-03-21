#ifndef _SOUND_H_
#define _SOUND_H_

#include "Config.h"
#include "DCCpp.h"

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE EL PIN PARA EL ALTAVOZ BUZZER
#define PIN_SOUND 6

class Sound {
  public:
    static void SoundOn();
    static void SoundOff();
    static void ActionError();
    static void ActionErrorCurrent();
    static void ActionOK();
  private:
  static void nota(int, int);
    
};
#endif
