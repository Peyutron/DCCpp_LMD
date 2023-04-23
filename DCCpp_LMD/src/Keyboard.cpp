#include "Keyboard.h"
#include "Oled.h"
#include "TextCommand.h"
#include "DCCpp_Uno.h"
#include "Turnout.h"
#include "Sensor.h"
#include "Outputs.h"
#include "EEStore.h"
#include "Comm.h"




/*
void Sound::SoundOn(){
  
  nota(e[5],150);noTone(PIN_SOUND);delay(40);
}

void Sound::SoundOff(){

  nota(f[1], 250);noTone(PIN_SOUND);delay(40); // no comentar

  noTone(PIN_SOUND);
}
void Sound::ActionError() {
  nota(e[1], 250); noTone(PIN_SOUND); delay(50);
}
void Sound::ActionErrorCurrent(){
  for (int errorMain = 0; errorMain <= 2; errorMain++){
    nota(cs[1], 500); noTone(PIN_SOUND); delay(50);
    delay (200);
  }
}

void Sound::ActionOK() {
  nota(b[3], 100); noTone(PIN_SOUND); delay(50);
}
//  nota(b[3], 100); noTone(PIN_SOUND); delay(50);
void Sound::nota(int frec, int t)
{
    tone(PIN_SOUND,frec);      // suena la nota frec recibida
    delay(t);                // para despues de un tiempo t
}

*/