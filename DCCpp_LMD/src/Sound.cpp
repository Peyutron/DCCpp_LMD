#include "Sound.h"
#include "Oled.h"
#include "TextCommand.h"
#include "DCCpp_Uno.h"
#include "Turnout.h"
#include "Sensor.h"
#include "Outputs.h"
#include "EEStore.h"
#include "Comm.h"

const int c[5]={131,262,523,1046,2093};       // frecuencias 4 octavas de Do
const int cs[5]={139,277,554,1108,2217};      // Do#
const int d[5]={147,294,587,1175,2349};       // Re
const int ds[5]={156,311,622,1244,2489};    // Re#
const int e[5]={165,330,659,1319,2637};      // Mi
const int f[5]={175,349,698,1397,2794};       // Fa
const int fs[5]={185,370,740,1480,2960};     // Fa#
const int g[5]={196,392,784,1568,3136};     // Sol
const int gs[5]={208,415,831,1661,3322};   // Sol#
const int a[5]={220,440,880,1760,3520};      // La
const int as[5]={233,466,932,1866,3729};    // La#
const int b[5]={247,494,988,1976,3951};      // Si



void Sound::SoundOn(){
  
  nota(e[3],250);noTone(PIN_SOUND); // delay(40);
  /*nota(d[1],150);noTone(PIN_SOUND);delay(40);
 /* nota(d[1],150);noTone(PIN_SOUND);delay(40);
  nota(g[1],500);noTone(PIN_SOUND);delay(40); // 900
  nota(d[2],500);noTone(PIN_SOUND);delay(40); // 900*/
}

void Sound::SoundOff(){
  
  /*nota(e[2], 200);noTone(PIN_SOUND);delay(150); //MI,FA,SOL,DO2 c,c,n,b,b=800, n=400, c=200, s=100;c,c,n,b,RE,MI,FA,c,c,b,
  nota(f[2], 200);noTone(PIN_SOUND);delay(150);
  nota(g[1], 400);noTone(PIN_SOUND);delay(150);
  nota(c[3], 800);noTone(PIN_SOUND);delay(800);
       
    nota(f[1], 200);noTone(PIN_SOUND);delay(50);
    nota(e[2], 200);noTone(PIN_SOUND);delay(50);
    nota(d[2], 400);noTone(PIN_SOUND);delay(50);
  //  nota(f[1], 250);noTone(PIN_SOUND);delay(50);
  nota(f[1], 250);  
  nota(gs[1], 500);  
  nota(f[1], 350);  
  nota(a[1], 125);
  nota(c[2], 500);
  nota(a[1], 375);  
  nota(c[2], 125);
  nota(e[2], 650);
  noTone(PIN_SOUND);*/
  //delay (500);
  nota(f[1], 250);noTone(PIN_SOUND);delay(40); // no comentar
  /*nota(gs[1], 500);noTone(PIN_SOUND);delay(40);
  nota(f[1], 375);noTone(PIN_SOUND);delay(40); 
  nota(c[2], 125);noTone(PIN_SOUND);delay(40);
  nota(a[1], 500); noTone(PIN_SOUND);delay(40); 
  nota(f[1], 375);noTone(PIN_SOUND);delay(40);  
  nota(c[2], 125);noTone(PIN_SOUND);delay(40);
  nota(a[1], 650);noTone(PIN_SOUND);delay(40); */
  noTone(PIN_SOUND);
}
void Sound::ActionError() {
  nota(e[1], 250); noTone(PIN_SOUND); delay(50);
}
void Sound::ActionErrorCurrent(){
  for (int errorMain = 0; errorMain <= 2; errorMain++){
    nota(cs[1], 500); noTone(PIN_SOUND); delay(50);
    // delay (200);
  }
}

void Sound::ActionOK() {
  nota(b[3], 100); noTone(PIN_SOUND); // delay(50);
}
//  nota(b[3], 100); noTone(PIN_SOUND); delay(50);

void Sound::SensorAction() {
  nota(g[2], 50); noTone(PIN_SOUND); // delay(50);
}

void Sound::nota(int frec, int t)
{
    tone(PIN_SOUND,frec);      // suena la nota frec recibida
    delay(t);                // para despues de un tiempo t
}
//"indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";
/*long DO=523.25, // notas musicales con sus frecuencias
RE=587.33,
MI=659.26,
FA=698.46,
SOL=783.99,
LA=880,
SI=987.77,
DO2=1046.50,
RE2=1174.66,
MI2=1318.51,
FA2=1396.91;
long melody[] = {MI,FA,SOL,DO2, // nuestra melodía
RE,MI,FA,
SOL,LA,SI,FA2,*/
