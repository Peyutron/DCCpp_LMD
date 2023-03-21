#include "Oled.h"
// U8g2lib -> https://github.com/olikraus/u8g2/wiki/fntlistallplain

#ifdef USE_OLED //Evita cargar la librería si #define USE_OLED no esta activo en DCCpp.h 

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define ACTUALIZA_INFO 250  // La pantalla se actualiza cada 250ms 1/4
#define PAUSA_          14  // Pausa de 6 segundos para mostrar la información. 14x250 = 3500ms
#define ACCESORIO 1
#define SENSORES  2
#define SALIDAS   3

// Select your screen on Oled.h list
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

bool on_off = false;
String serialIn = "<  >";

int timerpantalla = 0;
int cab = 0;
int velocidad = 0;
int direccion = 0;
int address = 0;
int subAddress = 0;
int tStatus = 0;
int idsensor = 0;
int pinsensor = 0;
int estadosensor = 0;
int idsalida = 0;
int pinsalida = 0;
int estadosalida = 0;
float current = 0.00;
long previousMillis = 0;
const long interval = ACTUALIZA_INFO;



void Oled::init() { //  DCCpp::begin
  u8g2.begin();       // Inicia la pantalla y presentación
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(u8g2_font_sirclivethebold_tr); // choose a suitable font
  u8g2.drawStr(40,15,"DCC++"); // write something to the internal memory
  u8g2.drawStr(5,40,LIBRARY_VERSIONL1);
  u8g2.drawStr(50,55,LIBRARY_VERSIONL2);
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(500);
}


void Oled::initScreen() { // Oled.cpp (interno)
  u8g2.clearBuffer(); // clear the internal memory
  if (on_off) {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(5, 15);  
    u8g2.print(serialIn);
    u8g2.setCursor(85, 15);
    current = int(DCCpp::getCurrentMain());
    current = (current / 1024 * 5 / 0.5);
    u8g2.print(current);
    u8g2.print(F(" mA"));
    // CAB
    u8g2.setCursor(5, 25);
    u8g2.print(F("Throttle: "));
    u8g2.print(cab);
    u8g2.print(F(" "));
    u8g2.print(velocidad);
    u8g2.print(F(" "));
    if (direccion) u8g2.println(F(">>"));
    else u8g2.println(F("<<"));
    // Accessorios:
    u8g2.setCursor(5, 35);
    u8g2.print(F("Accessor: "));
    u8g2.print(address);
    u8g2.print(F(" "));
    u8g2.print(subAddress);
    u8g2.print(F(" "));
    if (tStatus == 0) u8g2.println(F("Closed"));
    else u8g2.println(F("Open"));
    // Sensores:
    u8g2.setCursor(5, 45);
    u8g2.print(F("Sensores: "));
    u8g2.print(idsensor);
    u8g2.print(F(" "));
    u8g2.print(pinsensor);
    u8g2.print(F(" "));
    u8g2.print(estadosensor);
    // Pines de Salida
    u8g2.setCursor(5, 55);
    u8g2.print(F("Salidas:  "));
    u8g2.print(idsalida);
    u8g2.print(F(" "));
    u8g2.print(pinsalida);
    u8g2.print(F(" "));
    u8g2.print(estadosalida);
    // Pines de Salida

  } else {        // PANTALLA INICIAL

    u8g2.setFontMode(1);
    u8g2.setFont(u8g2_font_sirclivethebold_tr);  
    u8g2.setCursor(20,15);
    u8g2.print(F("DCC++ "));
    u8g2.print(F("OFF"));
    u8g2.setFont(u8g2_font_7x13_tr); // choose a suitable font
    u8g2.setCursor(10,30);
    u8g2.print(F("OLED: "));
    u8g2.print(OLED_NAME);
    u8g2.setCursor(10, 45);
    u8g2.print(F("com: "));
#if COMM_TYPE == 0          // Serial Selected
    u8g2.print(COMM_NAME);
#elif COMM_TYPE == 1        // Ethernet Shield Card Selected
    u8g2.print(COMM_SHIELD_OLED_NAME);
#endif
    u8g2.setCursor(10, 60);
    u8g2.print("D:  ");  u8g2.print(EEStore::data.nTurnouts);
    u8g2.print(" S:  "); u8g2.print(EEStore::data.nSensors);
    u8g2.print(" O:  "); u8g2.print(EEStore::data.nOutputs);
  }
  u8g2.sendBuffer();  // transfer internal memory to the display
}

void Oled::updateOled() { //  DCCpp::loop

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (timerpantalla >= 0) { // si el temporizador es mayor que 0 muestra pantallas
        
      timerpantalla--;
      
    } else { // Si el temporizador es = o menor que 0 muestra la pantalla inicial
      
      Oled::initScreen();

    }
  }
}

void Oled::showOledOn(bool onOff) { // DCCpp::powerOn
  on_off = onOff;
  timerpantalla = 0;
}

void Oled::GetThrottle(int tCab, int vel, int direc) { // PacketRegister -> RegisterList::setThrottle 
  cab = tCab;
  velocidad = vel;
  direccion = direc;
}

void Oled::GetAccesories(int addr, int subAddr, int Status) { // Turnout::activate
  address = addr;
  subAddress = subAddr;
  tStatus = Status;
}

void Oled::GetSensor(int num, int pullUp) { // Sensor::status
  idsensor = num;
  // pinsensor = Pin;
  estadosensor = pullUp;
}

void Oled::GetOutput(int num, int Pin, int state) { // Output::activate
  idsalida = num;
  pinsalida = Pin;
  estadosalida = state;
}

void Oled::printReadCv(int cv, int resultado){
  // TODO
}

void Oled::Monitor(char *m){
  serialIn = "";
  serialIn += m;
}

void Oled::printList(int option, int nObjeto) { // Turnout::show - Sensor::show - Output::show
  String ln1 = "";
  switch (option){
    case ACCESORIO:
      ln1 = "Desvios";
    break;
    case SENSORES:
      ln1 = "Sensores";
    break;
    case SALIDAS:
      ln1 = "Salidas";
    break;
  }
  u8g2.clearBuffer();   // clear the internal memory
  u8g2.setFont(u8g2_font_10x20_tr); // choose a suitable font
  u8g2.setCursor(55, 30);
  u8g2.println((String)nObjeto);
  u8g2.setCursor(30, 50);
  u8g2.println(ln1);
  u8g2.sendBuffer();    // transfer internal memory to the display
  timerpantalla = PAUSA_;
}

void Oled::printErrorList(int option) { // Output::show - Turnout::show - Sensor::show
  String ln1;
  switch (option) {
    case ACCESORIO:
      ln1 = "Sin Desvios";
      break;
    case SENSORES:
      ln1 = "Sin Sensores";
      break;
    case SALIDAS:
      ln1 = "Sin Salidas";
      break;

  }
  u8g2.clearBuffer();   // clear the internal memory
  u8g2.setFont(u8g2_font_10x20_tr); // choose a suitable font
  u8g2.setCursor(5,30);
  u8g2.println(ln1);
  u8g2.setCursor(35 , 50);
  u8g2.println(F("definidos"));
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;
}

void Oled::printDefined(int option, int n, int s, int m) { // Turnout::parse - Sensor::parse - Output::parse
  String ln2, ln3;
  switch (option) {
    case ACCESORIO:
      ln2 = "Desvio definido";
      ln3 = (String) "Id:" + n + " D:" + s + " S:" + m;
      break;
    case SENSORES:
      ln2 = "Sensor definido";
      ln3 = (String) "Id:" + n + " Pin:" + s + " P:" + m;
      break;
    case SALIDAS:
      ln2 = "Salida definida";
      ln3 = (String) "Id:" + n + " Pin:" + s + " F:" + m;
      break;
  }
  u8g2.clearBuffer();   // clear the internal memory
  u8g2.setFont(u8g2_font_10x20_tr); // choose a suitable font
  u8g2.setCursor(30, 15);
  u8g2.print(F("Exito"));
  u8g2.setFont(u8g2_font_7x13_tr); // choose a suitable font
  u8g2.setCursor(10, 35);
  u8g2.print(ln2);
  u8g2.setCursor(10, 50);
  u8g2.print(ln3);
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;
} 

void Oled::printSaved(bool estado) {  // Turnout::store(), Sensor::store(), Output::store()
  serialIn = "Guardado OK";
  String ln1, ln2, ln3;
  u8g2.clearBuffer();               // clear the internal memory
  u8g2.setFont(u8g2_font_10x20_tr); // choose a suitable font

  if (estado) {
    ln1 = "Exito"; 
    ln2 = "Memoria";
    ln3 = "Guardada";
  } else {
    ln1 = "Error";
    ln2 = "al ";
    ln3 = "Almacenar";
  }
  u8g2.setCursor(15, 15);
  u8g2.print(ln1);
  u8g2.setCursor(15, 35);
  u8g2.print(ln2);
  u8g2.setCursor(15, 55);
  u8g2.print(ln3);
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;
}

void Oled::printDelete(int option, boolean estado, int n) { // Turnout::remove - Sensor::remove - Output::remove
  String id = (String)n;
  String ln1, ln2, ln3;
  if (estado){
    ln2 = "Eliminado";
    ln3 = "Con exito";
  } else {
    ln1 = "Error"; 
    ln2 = "al eliminar";
  }
  switch (option) {
    case ACCESORIO:
      if (estado) ln1 = "Desvio " + id;
      else ln3 = "Desvio " + id;
      break;    
    case SENSORES:
      if (estado) ln1 = "Sensor " + id;
      else ln3 = "Sensor " + id;
      break;
    case SALIDAS:
      if (estado) ln1 = "Salida" + id;
      else  ln3 = "Salida " + id;
      break;
  }
  u8g2.clearBuffer();     // clear the internal memory
  u8g2.setFont(u8g2_font_7x13_tr); // choose a suitable font
  u8g2.setCursor(15, 25);
  u8g2.print(ln1);
  u8g2.setCursor(15, 40);
  u8g2.print(ln2);
  u8g2.setCursor(15, 55);
  u8g2.print(ln3);
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;
}

void Oled::printDeleteMemory() {  // EEStore.cpp EEStore::clear()
  u8g2.clearBuffer();   // clear the internal memory
  u8g2.setFont(u8g2_font_10x20_tr); // choose a suitable font
  u8g2.setCursor(15, 30);
  u8g2.print(F("Memoria"));
  u8g2.setCursor(15, 50);
  u8g2.print(F("Eliminada"));
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;
}

void Oled::printSram(int ram) { // TextCommand::parse -> 'F'

  String ln2 = (String) ram;  // Comando <F> DETERMINA CUANTA SRAM LIBRE DISPONIBLE EN ARDUINO  */
  timerpantalla = PAUSA_;
  u8g2.clearBuffer();     // clear the internal memory
  u8g2.setFont(u8g2_font_7x13_tr); // choose a suitable font
  u8g2.setCursor(1, 30);
  u8g2.println(F("Memoria disponible"));  //2KB para UNO - 8KB para MEGA
  u8g2.setFont(u8g2_font_10x20_tr); // choose a suitable font
  u8g2.setCursor(15, 50);
  u8g2.print(ln2);
  u8g2.println(F(" bytes"));
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;

}

#endif