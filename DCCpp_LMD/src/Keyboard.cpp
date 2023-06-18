
#include "Keyboard.h"
#include "Oled.h"
#include "TextCommand.h"
#include "DCCpp_Uno.h"
#include "Turnout.h"
#include "Sensor.h"
#include "Outputs.h"
#include "EEStore.h"
#include "Comm.h"
#ifdef USE_KEYBOARD
#ifdef USE_DIGITAL_4x3
  const char rows = 4; // set display to four rows
  const char cols = 3; // set display to three columns

  const char keys[rows][cols] = {
                {'1','2','3'},
                {'4','5','6'},
                {'7','8','9'},
                {'*','0','#'}
              };
  char rowPins[rows] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};
  char colPins[cols] = {PIN_COL1, PIN_COL2, PIN_COL3};
#endif


#ifdef USE_DIGITAL_4x4
  const char rows = 4; // set display to four rows
  const char cols = 4; // set display to four columns

  const char keys[rows][cols] = {
              {'1','2','3', 'A'},
              {'4','5','6', 'B'},
              {'7','8','9', 'C'},
              {'*','0','#', 'D'}
               };
  char rowPins[rows] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};
  char colPins[cols] = {PIN_COL1, PIN_COL2, PIN_COL3, PIN_COL4};
#endif


CmdStatus Keyboard::mStatus=halt;
bool Keyboard::OnOff = false;
char Keyboard::rKey[30];




void Keyboard::init(){
   for(char r = 0; r < rows; r++){
         pinMode(rowPins[r], INPUT);    //set the row pins as input
         digitalWrite(rowPins[r], HIGH);    //turn on the pullups
   }
   
   for(char c = 0; c < cols; c++){
         pinMode(colPins[c], OUTPUT);   //set the column pins as output
   }

}

void Keyboard::KeyLoop(){

  char key = ReadKey();
  if(key != 0) KeyboardProcess(key);
}


char Keyboard::ReadKey(){
      char k = 0;
      
      for(char c = 0; c < cols; c++){
        digitalWrite(colPins[c], LOW);
         for(char r = 0; r < rows; r++){
            if(digitalRead(rowPins[r]) == LOW){
            delay(10);    //20ms debounce time
            while(digitalRead(rowPins[r])== LOW);
            k = keys[r][c];
            }
         }
   digitalWrite(colPins[c], HIGH); 
      }
      return k;
}

void Keyboard::KeyboardProcess(char customKey){
char rKeyOld;

//Serial.println(Oled::PrintScreen);
switch (customKey)
  {
    case '*':
      OnOff = !OnOff; 
      if (OnOff){
        /***** TURN ON POWER FROM MOTOR SHIELD TO TRACKS  ****/
        DCCpp::powerOn();
        mStatus=go;

      } else {
        /***** TURN OFF POWER FROM MOTOR SHIELD TO TRACKS  ****/    
        DCCpp::powerOff();
        mStatus=halt;
      }    
    case '#':
      #ifdef USE_OLED
        // Oled::Menu_On_Off = !Oled::Menu_On_Off;
        // Oled::PrintScreen = true;
      #endif // USE_OLED 
      break;
    case '1':        // 
      Oled::Menu_On_Off = !Oled::Menu_On_Off;
      Oled::PrintScreen = true;
      Oled::nMenu = MLOC;
      break;
    case '2':        // 
      Oled::Menu_On_Off = !Oled::Menu_On_Off;
      Oled::PrintScreen = true;
      Oled::nMenu = MDES;
      break;
    case '3':        // 
      Oled::Menu_On_Off = !Oled::Menu_On_Off;
      Oled::PrintScreen = true;
      Oled::nMenu = MACC;
      break;
    case '0':
      Oled::Menu_On_Off = !Oled::Menu_On_Off;
      Oled::PrintScreen = true;
      Oled::nMenu = MLIST;
      break;      
    default:

      break;
   }

}
#ifdef USE_ENCODER

void Keyboard::initEncoder(){
  pinMode (PINCLK,INPUT);
  pinMode (PINDT, INPUT);
  pinMode (PINSW, INPUT_PULLUP);

  PCICR |= (1 << PCIE0); ///ESPARAMOS INTERRUPCIONES
  //PCMSK0 |= (1 << PCINT0); ///DECLARANDO PINES PARA INTERRUPCION pin 50
  PCMSK0 |= (1 << PCINT1); // pin 51
  PCMSK0 |= (1 << PCINT2); // pin 52
  PCMSK0 |= (1 << PCINT3); // PIN 53
}


#endif // USE_ENCODER

#endif // USE_KEYBOARD