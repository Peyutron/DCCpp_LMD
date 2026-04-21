/*
 * project: <DCCpp LMD>
 * author: <Carlos M.C>
 * description: <DCCpp Animation>
 * Placa Arduino MEGA
 * Shield Ardumoto L298
 *
 *
 * Se pueden iniciar los desvíos creando objetos "Turnout":
 * Turnout T1; 
 * En setup():
 * T1.begin(ID, DIRECCIÓN, SUBDIRECCIÓN);
 *
*/

#include "DCCpp.h"
#include "Animation_DCCpp.h"

#ifndef USE_TEXTCOMMAND
#error To be able to compile this sample,the line #define USE_TEXTCOMMAND must be uncommented in DCCpp.h
#endif

// Indice
uint8_t nextStep = 0;

// Initialize the accessories
Turnout ACC1, ACC3, ACC4, ACC5, ACC6, ACC7, ACC9;

// Number of animations
#define NUM_ANIMAT 7

/** Initialize the animations:
  * Turnour: Accessory or turnout. 
  * status: Estado true/false.
  * tDuration: Duration time in seconds. 
  * tFinish: Always 0.
  * type: #0-Accessory #1-Traffic light #2-Turnouts
  * comment: Comment about the animation (serial monitor only) can be left blank " ".
*/
Animations animation[NUM_ANIMAT] =
{
// Turnour | Status | St1 | Et2 | Type | Comment  
  {ACC1, false,  30, 0, LIGHT, "Soldador"},       // Iluminación exteriores 
  {ACC3, false,  60, 0, LIGHT, "Ilum. calles"},   // Iluminación casas
  {ACC4, false,  65, 0, LIGHT, "Ilum. casas"},    // Iluminación taller
  {ACC5, false,  60, 0, EFFECT, "Paso a nivel"},  // Iluminación televisor
  {ACC6, false,  59, 0, LIGHT, "Ilun. talleres"}, // Iluminación barriada
  {ACC7, false,  35, 0, EFFECT, "Tv Talleres"},   // Iluminación soldador
  {ACC9, false,  50, 0, EFFECT, "Barriada"}       // Iluminación soldador
};

// Initialize the turnouts
Turnout T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28; 

void setup()
{
  // Inicia la comunicación serial USB 
  Serial.begin(115200); 

 
  DCCpp::begin();
  delay(500);
  // Inicia la vía principal motor Shield Ardumoto
  DCCpp::beginMainMotorShield();
  
  // Inicia la vía de programación en motor Shield Ardumoto
  DCCpp::beginProgMotorShield(); 
  
  // Inicia todos los desvíos y accesorios
  statTurnoutsAndAccessories();
  
  DCCpp::powerOn();

}

void loop()
{
  // Función del loop principal de la librería
  DCCpp::loop(); 

  // Actualiza las animaciones
  updateAnimation();
}

void turnoutActivated(uint8_t index)
{
  if (animation[index].turnout.isActivated() == 0) animation[index].state = 1;
  else animation[index].state = 0;



  for (uint8_t resend = 0; resend < 2; resend++)
  {
    animation[index].turnout.activate(animation[index].state);
  }
  
  // Serial monitor information
  Serial.print(animation[index].comment);
  Serial.print(F(" "));
  Serial.println(animation[index].turnout.isActivated());
  
}

void updateAnimation()
{
  // Update the animation (call periodically from loop())
  unsigned long now = millis();
  
  if (now - animation[nextStep].tFinish >= animation[nextStep].tDuration * 1000) 
  {
    // Animation check
    turnoutActivated(nextStep);
    // Update the completion time
    animation[nextStep].tFinish = now;
  } 

  // Next animation for check
  nextStep++;
  // nextStep restart
  if (nextStep >= NUM_ANIMAT ) nextStep = 0; 
}

void statTurnoutsAndAccessories()
{
  Serial.println("Iniciando Accesorios...");

  // Accesorie ID DIR SUBDIR
  animation[0].turnout.begin(1, 2, 1);  // Efecto soldador B1
  animation[1].turnout.begin(3, 1, 1);  // Iluminación exteriores
  animation[2].turnout.begin(4, 1, 2);  // Iluminación casas
  animation[3].turnout.begin(5, 2, 2);  // Paso a nivel estación 
  animation[4].turnout.begin(6, 1, 3);  // Luz taller B1
  animation[5].turnout.begin(7, 2, 2);  // Luz TV taller B1 ACC7
  animation[6].turnout.begin(9, 3, 0);  // Barriada B1

  // Turnout ID DIR SUBDIR
  T17.begin(17, 5, 0);  // Acceso a talleres bucle 1
  T18.begin(18, 5, 1);  // Talleres Módulo bucle 1 
  T19.begin(19, 5, 2);  // Acceso a estación y deposito Módulo 2
  T20.begin(20, 5, 3);  // Deposito 1 Módulo 2
  T21.begin(21, 6, 0);  // Estación 1 Módulo 3
  T22.begin(22, 6, 1);  // Estación 3 Módulo 4
  T23.begin(23, 6, 2);  // Cocheras Módulo 4
  T24.begin(24, 6, 3);  // Cruce Módulo 4
  T25.begin(25, 7, 0);  // Estación 2 Módulo 4
  T26.begin(26, 7, 1);  // Estación 2 Módulo 5
  T27.begin(27, 7, 2);  // Estación 3 Módulo 5
  T28.begin(28, 7, 3);  // Cruce Módulo 5 
  
}
