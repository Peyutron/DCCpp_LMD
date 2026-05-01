#include <Arduino.h>
#include <DCCpp.h>

struct Animations 
{
  Turnout turnout;          // Turnout or accessory
  uint8_t state;            // Animation state true/false
  unsigned long tDuration;  // Start time
  unsigned long tFinish ;   // Finish time
  uint8_t type;             // Accessory type
  char comment[18];         // Comment
  char command[20];         // Command
};


enum AnimState {
  LIGHT,
  EFFECT,
  SEMAPHORE,
  COMMAND
};