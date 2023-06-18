#include "Oled.h"

// U8g2lib -> https://github.com/olikraus/u8g2/wiki/fntlistallplain7
// Algunas funciones están basadas en el código ->
// de "Une manette simple et autonome pour LaBox" en la ->
// web https://www.locoduino.org/spip.php?article286

#ifdef USE_OLED // Evita cargar la librería si #define USE_OLED no esta activo en DCCpp.h 
#include "Config.h"
#include "Turnout.h"
#include "DCCpp.h"
#include "CommInterface.h"
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef OLED_ANIMATION
  #include <logo.h>
#endif  // OLED_ANIMATION




////////////////////////////////////
// Select your screen on Oled.h list
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display


bool Oled::Menu_On_Off = false;
bool Oled::PrintScreen = true; // dibuja en pantalla
int8_t Oled::nMenu = 0;
bool on_off = false;
String serialIn;
String wifiIp = " ";
String S88Binary = ""; 
int16_t timerpantalla = 0;
uint8_t cab, speed, direccion; 
uint8_t address, subAddress, tStatus;
uint8_t idturnout, turnoutStatus;

uint8_t idsensor, pinsensor, estadosensor;
uint8_t idsalida, pinsalida, estadosalida;

#ifdef USE_KEYBOARD
const char *menu_strings[MENU_ITEMS] = { "(1)Locomotoras", "(2)Desvios", "(3)Accesorios" }; //, "(4)Sensores", "(5)Info" };
uint8_t ActiveAddress = 0;
const uint8_t LMAX = 4; // number of loco addresses
uint16_t LocoAddress[LMAX] = {2, 7, 10, 5};
uint16_t LocoDirection[LMAX] = {0, 0, 0, 0};
uint8_t LocoSpeed[LMAX] = {0, 0, 0, 0};
uint8_t LocoSpeedOld[LMAX] = {0, 0, 0, 0};
uint16_t LocoFN[LMAX][LMAX]= {{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0},{0, 0, 0, 0}} ; // [LIGNES][COLONNES] for 13-20 and 21-28 2&3 groups / loco addresses
byte LocoFN0to4[LMAX] = {0, 0, 0, 0};   // 0 group
byte LocoFN5to8[LMAX] = {0, 0, 0, 0};    // 1 group
byte LocoFN9to12[LMAX] = {0, 0, 0, 0};   // 2 group
byte LocoFN13to20[LMAX] = {0, 0, 0, 0};  // 3 group
byte LocoFN21to28[LMAX] = {0, 0, 0, 0};  // 4 group
uint8_t group[LMAX] = {0, 0, 0, 0}; // 9-24 in progress

int8_t idturnoutold = -1;
uint8_t Testado[N_DESVIOS + 1];

uint8_t accAddr, accSubAddr, accStatus;
uint8_t accAddrOld = 255;
uint8_t accSubAddrOld = 255;
uint8_t accSubScreen = false;
  
int8_t nDigito[3] = {-1, -1, -1};
int8_t nDigitoOld[3] = {-1, -1, -1};
uint8_t posDig = 0; // posicion del cursor al introducir una cadena de digitos
#ifdef USE_ENCODER
int8_t positionSpeed = 0;
uint8_t locDireccion = 0;
int8_t aState;
int8_t aLastState;  
#endif

bool exitMenu = true;

bool mFunciones = false;
u8g2_uint_t hc = 23;

u8g2_uint_t i = 0;
u8g2_uint_t h = 0;
u8g2_uint_t w = 0;
u8g2_uint_t d = 0;

#endif // USE_KEYBOARD

float current = 0.00;
long previousMillis = 0;
const long interval = ACTUALIZA_INFO;



void Oled::init() { //  DCCpp::begin
  u8g2.begin();       // Inicia la pantalla 
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  #ifdef OLED_ANIMATION
    u8g2.drawXBM( 0, 0, u8g_logo_width, u8g_logo_height, image_data_array);  // u8g2.setFont(u8g2_font_sirclivethebold_tr); // choose a suitable font
    u8g2.setFont(u8g2_font_cu12_tr); // Fuente cu12
    u8g2.setCursor(5, 13);
    u8g2.print(DCCPP_VERSION1);  // Oled.h
    u8g2.print(DCCPP_VERSION2);  // Oled.h
  #else
    // OLED_ANIMATION
    u8g2.setFont(u8g2_font_ncenR14_te); // Fuente R14
    u8g2.drawStr(43,15,"DCC++"); // write something to the internal memory
    u8g2.setCursor(10, 35);  
    u8g2.print(DCCPP_VERSION1);  // Oled.h
    u8g2.setCursor(50, 60);  
    u8g2.print(DCCPP_VERSION2);  // Oled.h
  #endif

    u8g2.sendBuffer();
    delay(2500);
}

void Oled::initScreen() { // Oled.cpp (interno)
 
    if (PrintScreen){
      u8g2.clearBuffer(); // Limpia el buffer de la pantalla
      Oled::printSerial(); // Linea 0 (amarilla) 
      if (on_off) {
        u8g2.setFont(u8g2_font_6x10_tr); // Fuente 6x10
        u8g2.setCursor(90, 14);
        current = int(DCCpp::getCurrentMain());
        current = (current / 1024 * 5 / 0.5);
        u8g2.print(current);
        u8g2.print(F("mA"));
        // CAB
        u8g2.setCursor(5, 25);
        u8g2.print(F("Throttle: "));
        u8g2.print(cab);
        u8g2.print(F(" "));
        u8g2.print(speed);
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
    
        #ifdef USE_SENSOR
        // Sensores:
          u8g2.setCursor(5, 45);
          u8g2.print(F("Sensores: "));
          u8g2.print(idsensor);
          u8g2.print(F(" "));
          u8g2.print(pinsensor);
          u8g2.print(F(" "));
          u8g2.print(estadosensor);
        #endif  // USE_SENSOR
        #ifdef USE_S88
        // S88:
          if (S88Binary.equals("")){
            u8g2.setCursor(5, 45);
            u8g2.print(F("S88: "));
            u8g2.print(idsensor);
            u8g2.print(F(" "));
            if (estadosensor == 0) u8g2.println(F("occupied"));
            else u8g2.println(F("unoccupied"));
          } else {
            u8g2.setCursor(5, 45);
            u8g2.print(S88Binary);
          }
        #endif  // USE_S88
        #ifdef USE_OUTPUT
         // Pines SalidaS
          u8g2.setCursor(5, 55);
          u8g2.print(F("Salidas:  "));
          u8g2.print(idsalida);
          u8g2.print(F(" "));
          u8g2.print(pinsalida);
          u8g2.print(F(" "));
          u8g2.print(estadosalida);
        #endif // USE_OUTPUT

        } else {  // PANTALLA INICIAL
          u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
          u8g2.setFontMode(0);
          u8g2.setCursor(96,15);
          u8g2.print(F("OFF"));
          u8g2.setCursor(5,30);
          u8g2.print(F("Com: "));
          if (Oled::SwInput() != "") u8g2.print(Oled::SwInput());
          else u8g2.print(COMM_NAME);
          u8g2.setCursor(5, 45);
          if (!wifiIp.equals(" ")) u8g2.print(wifiIp); // wifi WebSocket conectado
          else {
            u8g2.setCursor(10, 45);
            u8g2.setFont(u8g2_font_6x10_tr); // Fuente 6x10
            u8g2.print(DCCPP_LIBRARY_VERSION);
            u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
          }
          uint8_t a, b, c;
          u8g2.setCursor(5, 60);
          #ifdef USE_TURNOUT
            a = EEStore::data.nTurnouts;
          #endif
          #ifdef USE_SENSOR
            a = EEStore::data.nSensors;
          #endif
          #ifdef USE_OUTPUT
            a = EEStore::data.nOutputs;
          #endif

          u8g2.print(F("D:  "));  u8g2.print(a);
          u8g2.print(F(" S:  ")); u8g2.print(b);
          u8g2.print(F(" O:  ")); u8g2.print(c);
        }
      u8g2.sendBuffer();  // transfer internal memory to the display
      PrintScreen = false;
    }
  
}

#ifdef USE_KEYBOARD
// Muestra la lista de opciones
void Oled::MenuLista(){ 
  if (PrintScreen){
    u8g2.clearBuffer(); // Limpia el buffer de la pantalla
    u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
    h = 25;
    for( i = 0; i < MENU_ITEMS; i++ ) {
      d = (w-u8g2.getStrWidth(menu_strings[i]))/2;
      u8g2.drawStr(10 , h , menu_strings[i]);
      h = h + 13;
    }

    u8g2.sendBuffer();  // transfer internal memory to the display
    PrintScreen = false;
  }
  char key = Keyboard::ReadKey();
  if (key == '#') Oled::ExitPage();
}

void Oled::MenuKeyboard(){    // desde Keyboard::KeyboardProcess(char)
 // Serial.print("MenuKeyboard n: "); Serial.println(nMenu);
  switch (nMenu) {
    case MLIST:
      Oled::MenuLista();
      break;
    case MLOC:
      Oled::LocomotiveOled();
      break;
    case MDES:
      Oled::TurnoutOled();
      break;  
    case MACC:
      Oled::AccesoryOled();
      break;
  }
}

void Oled::LocomotiveOled(){
  
    if (PrintScreen){
      u8g2.clearBuffer(); // Limpia el buffer de la pantalla
      Oled:printSerial();
      u8g2.setCursor(90, 13);   // Linea 0 (amarilla)
      u8g2.print(F("#="));
      u8g2.print(ActiveAddress + 1);
      // u8g2.setCursor(0, 25);    // Linea 1
      u8g2.setCursor(0, 25);    // Linea 1
      u8g2.print(F("D="));
      u8g2.print(LocoAddress[ActiveAddress]);
      // u8g2.setCursor(35, 25);   // Linea 1
       u8g2.setCursor(40, 25);   // Linea 1
       u8g2.print(F(" V="));
       u8g2.print(LocoSpeed[ActiveAddress]);
      // u8g2.setCursor(65, 25);   // Linea 1
      u8g2.setCursor(80, 25);   // Linea 1
      if (LocoDirection[ActiveAddress] == 1) u8g2.print(F(" <<<"));
      else u8g2.print(F(" >>>"));

      // u8g2.setCursor(0, 35);    // Linea 2
      u8g2.setCursor(0, 38);    // Linea 2
      String temp = "00000" + String(LocoFN0to4[ActiveAddress], BIN);  // pad with leading zeros
      int tlen = temp.length() - 5;
      u8g2.print(temp.substring(tlen));
      u8g2.print(F(" "));  // modif
      
      temp = "0000" + String(LocoFN5to8[ActiveAddress], BIN);
      tlen = temp.length() - 4;
      // u8g2.setCursor(40, 35); // Linea 2
      u8g2.setCursor(45, 38); // Linea 2
      u8g2.print(temp.substring(tlen));
      u8g2.print(F("           ")); 
        
        // u8g2.setCursor(0, 45); // Linea 3
      u8g2.setCursor(0, 51); // Linea 3
      temp = "000" + String(LocoFN9to12[ActiveAddress], BIN);
      tlen = temp.length() - 4;  
      u8g2.print(temp.substring(tlen));
      u8g2.print(F(" ")); 
        
      u8g2.setCursor(45, 51); // Linea 4 
      temp = "0000000" + String(LocoFN13to20[ActiveAddress], BIN);
      tlen = temp.length()-8;  
      u8g2.print(temp.substring(tlen));
      u8g2.print(String(LocoFN13to20[ActiveAddress])); 
      u8g2.print(F(" 2")); 
        
      u8g2.setCursor(0, 64);       
      temp = "0000000" + String(LocoFN21to28[ActiveAddress], BIN);
      tlen = temp.length()- 8;  
      u8g2.print(temp.substring(tlen));
      u8g2.print(F(" 3"));       
      // Serial.println("Imprime LocomotoraOled FUN");
      if (mFunciones){ // False loc / True fun
        u8g2.setDrawColor(0); // color invertido
        u8g2.drawBox(15, 18,   85, 40);
        u8g2.setDrawColor(1); // Color normal
        u8g2.drawRFrame(15, 18, 85, 40,7);
        u8g2.setCursor(30, 35);
        u8g2.print(F("Grupo: "));
        u8g2.print(group[ActiveAddress]);
        u8g2.setCursor(22, 49);
        u8g2.print(F("Funcion: ?"));
      } // fin mFunciones 
      
      u8g2.sendBuffer();  // transfer internal memory to the display
      PrintScreen = false;
    } // fin PrintScreen

  char key = Keyboard::ReadKey();
  if (key != 0){
    if (!mFunciones){ // False 
      switch (key) {
        case '#': // Salida
          // Serial.println("Sale LocomotoraOled");   // DEBUG
          Oled::ExitPage();
          break;
        case '*':
          ActiveAddress++;
          if (ActiveAddress > 3) ActiveAddress = 0;
          PrintScreen = true;
          positionSpeed = LocoSpeed[ActiveAddress];
          locDireccion = LocoDirection[ActiveAddress];
          delay(150); // no afecta al bucle   
          break;
        /*case '1':   // Grupo 0 - 4
        case '2':   // Grupo 5 - 8
        case '3':   // Grupo 9 - 13
        case '4':   // Grupo 14 - 20
        case '5':   // Grupo 21 - 28
          char keygroup = key;      
          group[ActiveAddress] = keygroup - 48;
          PrintScreen = true;
          mFunciones = !mFunciones;
          key = 9;
          delay(150); // no afecta al bucle   
          break;*/
      }
    } else { // Modo funciones
      uint8_t nFun = key - 48;
      // Serial.print("Key nFun: ");
      // Serial.println(nFun);
      switch (group[ActiveAddress]) {
          case GRUPO_F04:
            if (nFun <= 4){
              if (bitRead(LocoFN0to4[ActiveAddress], nFun) == 0 ) {
                bitWrite(LocoFN0to4[ActiveAddress], nFun, 1);
                //if (nFun == 0) LocoFN0to4[ActiveAddress] = LocoFN0to4[ActiveAddress] + 16;
                // 

              } else {
                if (bitRead(LocoFN0to4[ActiveAddress], nFun) == 1 ) {
                  bitWrite(LocoFN0to4[ActiveAddress], nFun, 0);
                  // LocoFN0to4[ActiveAddress] = LocoFN0to4[ActiveAddress] - 16;
                  //if (nFun == 0) LocoFN0to4[ActiveAddress] = LocoFN0to4[ActiveAddress] - 16;

                }
              }
              // Serial.print(LocoFN0to4[ActiveAddress], BIN);
              // Serial.println(" LocoFN0to4[ActiveAddress] d ");
              // Serial.print(LocoFN0to4[ActiveAddress], DEC);
              // Serial.println(" LocoFN0to4[ActiveAddress]");
            }
            break;
          case GRUPO_F58:
            if (nFun <= 4){
              if (bitRead(LocoFN5to8[ActiveAddress], nFun) == 0 ) {
                bitWrite(LocoFN5to8[ActiveAddress], nFun, 1);
              } else {
                if (bitRead(LocoFN5to8[ActiveAddress], nFun) == 1 ) {
                  bitWrite(LocoFN5to8[ActiveAddress], nFun, 0);
                }
              }
              // Serial.print(LocoFN5to8[ActiveAddress], BIN);
              // Serial.println(" LocoFN5to8[ActiveAddress] d ");
              // Serial.print(LocoFN5to8[ActiveAddress], DEC);
              // Serial.println(" LocoFN5to8[ActiveAddress]");
            }
            break;
          case GRUPO_F0912:
            if (nFun <= 4){
              if (bitRead(LocoFN9to12[ActiveAddress], nFun) == 0 ) {
                bitWrite(LocoFN9to12[ActiveAddress], nFun, 1);
              } else {
                if (bitRead(LocoFN9to12[ActiveAddress], nFun) == 1 ) {
                  bitWrite(LocoFN9to12[ActiveAddress], nFun, 0);
                }
              }
            // Serial.print(LocoFN9to12[ActiveAddress], BIN);
            // Serial.println(" LocoFN9to12[ActiveAddress] d ");
            // Serial.print(LocoFN9to122[ActiveAddress], DEC);
            // Serial.println(" LocoFN9to12[ActiveAddress]");
            }
            break;
          case GRUPO_F1320:
            if (nFun <= 8 ){
              if (bitRead(LocoFN13to20[ActiveAddress], nFun) == 0 ) {
                bitWrite(LocoFN13to20[ActiveAddress], nFun, 1);
              } else {
                if (bitRead(LocoFN13to20[ActiveAddress], nFun) == 1 ) {
                  bitWrite(LocoFN13to20[ActiveAddress], nFun, 0);
                }
              }
            // Serial.print(LocoFN13to20[ActiveAddress], BIN);
            // Serial.println(" LocoFN13to20[ActiveAddress] d ");
            // Serial.print(LocoFN13to20[ActiveAddress], DEC);
            // Serial.println(" LocoFN13to20[ActiveAddress]");
            }
            break;
          case GRUPO_F2128:
            if (nFun <= 8){
              if (bitRead(LocoFN21to28[ActiveAddress], nFun) == 0 ) {
                bitWrite(LocoFN21to28[ActiveAddress], nFun, 1);
              } else {
                if (bitRead(LocoFN21to28[ActiveAddress], nFun) == 1 ) {
                  bitWrite(LocoFN21to28[ActiveAddress], nFun, 0);
                }
              }
            // Serial.print(LocoFN21to28[ActiveAddress], BIN);
            // Serial.println(" LocoFN21to28[ActiveAddress] d ");
            // Serial.print(LocoFN21to28[ActiveAddress], DEC);
            // Serial.println(" LocoFN21to28[ActiveAddress]");
            }
            break;
          case '#':   // Salida
            PrintScreen = true;
            mFunciones = !mFunciones;
            group[ActiveAddress] = 0;
            delay(150); // no afecta al bucle
            break;
      } // fin switch     
      if (Oled::CreateFunctionCommand != ""){
        Oled::SendCommand(Oled::CreateFunctionCommand((group[ActiveAddress])));
        PrintScreen = true;
        mFunciones = !mFunciones;
        group[ActiveAddress] = 0;
      } // fin Oled::CreateFunctionCommand
    } // fin modo locomotora / funciones
  } // fin if key 
  
if (locDireccion != LocoDirection[ActiveAddress]){
  if (LocoDirection[ActiveAddress] == 0) LocoDirection[ActiveAddress] = 1;
  else LocoDirection[ActiveAddress] = 0;
  Oled::SendCommand(Oled::CreateLocoCommand());
  PrintScreen = true;
}

if (positionSpeed != LocoSpeed[ActiveAddress]){
    PrintScreen = true;
    LocoSpeed[ActiveAddress] = positionSpeed;
    Oled::SendCommand(Oled::CreateLocoCommand());
    //Serial.println("Encoder: "); Serial.print(positionSpeed);
  }

}
void Oled::TurnoutOled(){
    
    if (PrintScreen){
      u8g2.clearBuffer(); // Limpia el buffer de la pantalla
      Oled:printSerial();
      u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
      u8g2.setCursor(70,13); // 
      u8g2.print(F("Desvios"));
      u8g2.setCursor(5,30); // // line 2
      u8g2.print(F("Id: "));
      u8g2.print(idturnout);
      if (!Testado[idturnout]) {
        u8g2.print(F(" Desviado      "));
      }
      else {
        u8g2.print(F(" Recto     "));
      } 
      u8g2.setCursor(5, 48); // line 3
      for (uint8_t z = 1; z < (sizeof(Testado)/2)+1; z++) u8g2.print(Testado[z]);
      u8g2.setCursor(5, 60); // line 3
      for (uint8_t z = (sizeof(Testado)/2)+1; z < sizeof(Testado); z++) u8g2.print(Testado[z]);



      if (mFunciones){                           
       u8g2.setDrawColor(0); // color invertido
          u8g2.drawBox(25, 18, 85, 40);
          u8g2.setDrawColor(1); // Color normal
          u8g2.drawRFrame(25, 18, 85, 40,6);
          u8g2.setCursor(33, 35);
          u8g2.print(F("Select ID: "));
          u8g2.setCursor(50, 49);
          for (uint8_t d = 0; d < 3; d++) 
            if (nDigito[d] != nDigitoOld[d])
            u8g2.print(nDigito[d]);
      }
      
    u8g2.sendBuffer();  // transfer internal memory to the display
    PrintScreen = false;  
    } // fin PrintScreen

    char key = Keyboard::ReadKey();
  if (key != 0){
    bool validate = true;
    switch (key) {
      case '*': //
      if (idturnoutold != 0){
        if (idturnout != idturnoutold){
          idturnout =  Oled::GetNumber(nDigito);
          if (idturnout > sizeof(Testado)){
            serialIn = "Error";
            Oled::printSerial();
            validate = false;
          } else {
            idturnoutold = idturnout;
          }
        } 
        if (validate){
          if (idturnout == idturnoutold){
            if (Testado[idturnout] != 0)Testado[idturnout] = 0; 
            else Testado[idturnout] = 1;
            Oled::SendCommand(Oled::CreateTurnoutCommand());
          }
        }
        PrintScreen = true;  mFunciones = false;
      }
        break;
      

      case '#': // Salida
        if (mFunciones) { 
          mFunciones = false; PrintScreen = true; 
          Oled::ClearNumbers();  
        } else Oled::ExitPage();
        break;
      
      default:
      delay (100); // No afecta al bucle
        nDigito[posDig] = key - 48;
        PrintScreen = true; mFunciones = true;
        idturnoutold = -1;
        posDig = posDig + 1;
        if (posDig == 3)posDig = 0;
        break;


      } // fin switch
    } // fin if (key != 0)
    // address is the primary address of the decoder controlling this turnout (0-511)
    // subaddress is the subaddress of the decoder controlling this turnout (0-3)
    // activate is either (0) (Deactivate, Straight, Closed) or (1) (Activate, Turn, Thrown)
  }

void Oled::AccesoryOled(){
  if (PrintScreen){
    u8g2.clearBuffer(); // Limpia el buffer de la pantalla
    Oled:printSerial();
    u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
    u8g2.setCursor(75,13); // 
    u8g2.print(F("Acces"));
    u8g2.setCursor(5,30); // // line 2
    u8g2.print(F("Addr: "));
    u8g2.print(accAddr);
    u8g2.print(F(" Sub: "));
    u8g2.print(accSubAddr);
    u8g2.setCursor(5, 50); // line 3
    if (!accStatus) u8g2.print(F(" Encendido "));
    else u8g2.print(F(" Apagado "));   
    if (mFunciones){                           
      u8g2.setDrawColor(0); // color invertido
      u8g2.drawBox(25, 18, 85, 40);
      u8g2.setDrawColor(1); // Color normal
      u8g2.drawRFrame(20, 18, 95, 40,6);
      u8g2.setCursor(28, 35);
      if (!accSubScreen) u8g2.print(F("Select addr:"));
      else u8g2.print(F("Select sub:"));
      u8g2.setCursor(55, 49);
      for (uint8_t d = 0; d < 3; d++) {
        if (nDigito[d] != nDigitoOld[d]) u8g2.print(nDigito[d]);
      }
    }
  u8g2.sendBuffer();  // transfer internal memory to the display
  PrintScreen = false;  
  } // fin PrintScreen

  char key = Keyboard::ReadKey();
  if (key != 0) {
  bool validate = true;
  switch (key) {
    case '*': //
      if ((accAddr == accAddrOld) && (accSubAddr == accSubAddrOld) ){
        Oled::SendCommand(Oled::CreateAccesoryCommand());
      } else {
      if (!accSubScreen){
        if (accAddrOld != 0){
          if (accAddr != accAddrOld){
            accAddr =  Oled::GetNumber(nDigito);
            accAddrOld = accAddr;
            accSubScreen = true;
          }
        }
      } else {
          Serial.print("sdir: "); Serial.println(accSubAddr);
          if (accSubAddrOld != 0){
            if (accSubAddr != accSubAddrOld){
              accSubAddr =  Oled::GetNumber(nDigito);
              accSubAddrOld = accSubAddr;
              accSubScreen = false;
            }
            if (accStatus != 0) accStatus = 0; 
            else accStatus = 1;
          }  
        }
      }
        PrintScreen = true;  mFunciones = false;
      
        break;
      

    case '#': // Salida
        if (mFunciones) { 
          mFunciones = false; PrintScreen = true; 
          Oled::ClearNumbers();  
          Serial.println(F("Sale dialogo"));
        } else Oled::ExitPage();
        break;
      
      default:
        //delay (100); // No afecta al bucle
        nDigito[posDig] = key - 48;
        PrintScreen = true; mFunciones = true;
        accAddrOld = 255;
        accSubAddrOld = 255;
        posDig = posDig + 1;
        if (posDig == 3) posDig = 0;
        break;


      } // fin switch
    } // fin if (key != 0)
    // address is the primary address of the decoder controlling this turnout (0-511)
    // subaddress is the subaddress of the decoder controlling this turnout (0-3)
    // activate is either (0) (Deactivate, Straight, Closed) or (1) (Activate, Turn, Thrown)
  }

void Oled::ExitPage(){
  Oled::PrintScreen = true;
  Oled::nMenu = -1;          
  Oled::Menu_On_Off = false;
}

uint8_t Oled::GetNumber(char num[]){  // obtiene un número de 3 cifras
  uint8_t number=num[0];              // desde un char array
  for (uint8_t i=1;i<3;i++)
    { 
    if (nDigito[i] != nDigitoOld[i]){  
        number = number*10+num[i];
    }
  }
  Oled::ClearNumbers();
    
  return number;
}

void Oled::ClearNumbers(){
  posDig = 0; // Primera posición del array
  for (uint8_t i=1;i<3;i++){
    nDigitoOld[i] = -1;
    nDigito[i] = -1;
  }   // todo en -1 para que no lo tome en cuenta
}

#endif // USE_KEYBOARD
#ifdef USE_ENCODER
ISR(PCINT0_vect){
  if (Oled::nMenu == MLOC){
  aState = digitalRead(PINCLK); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise 
    if (digitalRead(PINDT) != aState) { 
       positionSpeed++;
    } else {
       positionSpeed--;
    }
    if (positionSpeed <= 0) positionSpeed = 0;
    positionSpeed = constrain(positionSpeed, 0, 126);

   }
  } 
   aLastState = aState; // Updates the previous state of the outputA with the current state

if (!digitalRead(PINSW)){ // Cambio de dirección
  locDireccion = !locDireccion;
}

}
#endif // USE_ENCODER

void Oled::printSerial(){ // Imprime la linea 0
  u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
  u8g2.setCursor(0, 13);  // Linea 0 (amarilla)
  u8g2.print(serialIn);
}

void Oled::Monitor(char *m){
  //Serial.print("Monitor: "); Serial.println(m[0]);
  //if (m[0] == "<"){ 
    if (m[1] == 'a'|| m[1] == 'y') return;  // OLED para que no muestre el consumo o S88 con cada consulta 
    else {
      serialIn = "";
      serialIn += m;    
      PrintScreen = true;
    }
  //}
}
/*
void Oled::updateOled() { //  DCCpp::loop

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (timerpantalla >= 0) { // si el temporizador es mayor que 0 muestra pantallas
        
      timerpantalla--;
      
    } else { // Si el temporizador es = o menor que 0 muestra la pantalla inicial
        if (!Oled::Menu_On_Off)Oled::initScreen();
        #ifdef USE_KEYBOARD
        // else Oled::MenuKeyboard();
        #endif


    }
  }
}
*/
void Oled::OledDCCon(bool onOff) { // DCCpp::powerOn
  on_off = onOff;
  timerpantalla = 0;
}

void Oled::GetThrottle(int tCab, int vel, int direc) { // PacketRegister -> RegisterList::setThrottle 
  cab = tCab;
  speed = vel;
  direccion = direc;
  PrintScreen = true;
}

void Oled::GetAccesories(uint8_t addr, uint8_t subAddr, uint8_t Status) { // Turnout::activate
  address = addr;
  subAddress = subAddr;
  tStatus = Status;
  PrintScreen = true;

}

#ifdef USE_S88
void Oled::GetS88(uint8_t id, uint8_t estado) { // S88::check = 3
  S88Binary = "";
  idsensor = id;
  estadosensor = estado;
  PrintScreen = true;

}
void Oled::GetS88Binary(String dataS) { // S88::check = 0 
  S88Binary = dataS;
  PrintScreen = true;

}
#endif // USE_S88

void Oled::GetSensor(uint8_t num, uint8_t pullUp) { // Sensor::status
  idsensor = num;
  // pinsensor = Pin;
  estadosensor = pullUp;
  PrintScreen = true;
}

void Oled::GetOutput(uint8_t num, uint8_t Pin, uint8_t state) { // Output::activate
  idsalida = num;
  pinsalida = Pin;
  estadosalida = state;
  PrintScreen = true;
}

#ifdef USE_SERIALWIFI
void Oled::printWifiIp(String ip) { // Muestra la IP del Websocket comando "I" TextCommand.cpp
  wifiIp = ip;
  PrintScreen = true;
}
#endif  // USE_SERIALWIFI



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
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_10x20_tr); // Fuente 10x20
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
    case S_88:
      ln1 = "Error S88";
      break;

  }
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_10x20_tr); // Fuente 10x20
  u8g2.setCursor(5,30);
  u8g2.println(ln1);
  u8g2.setCursor(35 , 50);
  if (option != S_88) u8g2.println(F("definidos"));
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
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_10x20_tr); // Fuente 10x20
  u8g2.setCursor(30, 15);
  u8g2.print(F("Exito"));
  u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
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
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_10x20_tr); // Fuente 10x20

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
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
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
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_10x20_tr); // Fuente 10x20
  u8g2.setCursor(15, 30);
  u8g2.print(F("Memoria"));
  u8g2.setCursor(15, 50);
  u8g2.print(F("Eliminada"));
  u8g2.sendBuffer();  // transfer internal memory to the display
  timerpantalla = PAUSA_;
}

void Oled::printSram(uint16_t ram) { // TextCommand::parse -> 'F'

  String ln2 = (String) ram;  // Comando <F> DETERMINA CUANTA SRAM LIBRE DISPONIBLE EN ARDUINO  */
  // timerpantalla = PAUSA_;
  u8g2.clearBuffer(); // Limpia el buffer de la pantalla
  u8g2.setFont(u8g2_font_7x13_tr); // Fuente 7x13
  u8g2.setCursor(1, 30);
  u8g2.println(F("Memoria disponible"));  //2KB para UNO - 8KB para MEGA
  u8g2.setFont(u8g2_font_10x20_tr); // Fuente 10x20
  u8g2.setCursor(15, 50);
  u8g2.print(ln2);
  u8g2.println(F(" bytes"));
  u8g2.sendBuffer();  // transfer internal memory to the display
  delay(1000);
  //timerpantalla = PAUSA_;
}

#ifdef USE_KEYBOARD
String Oled::CreateLocoCommand(){
  String data = "t" + (String)(ActiveAddress + 1) + " " +
     (String)LocoAddress[ActiveAddress] + " " + 
     (String)LocoSpeed[ActiveAddress] + " " + 
     (String)LocoDirection[ActiveAddress];
  return data;
}

String Oled::CreateFunctionCommand(uint8_t grupo){
  String sdata = "f ";

  uint16_t fx = 0;
  switch (grupo) {
    case GRUPO_F04 :
      Serial.print("LocoFN0to4: ");
      Serial.println(LocoFN0to4[ActiveAddress]);
      fx = LocoFN0to4[ActiveAddress] + 128 ; 
      sdata = sdata + (String)LocoAddress[ActiveAddress] + " " + fx;
      break;
    case GRUPO_F58:
      fx = LocoFN5to8[ActiveAddress] + 176;
      sdata = sdata + (String)LocoAddress[ActiveAddress] + " " + fx;
      break;
    case GRUPO_F0912:
      fx = LocoFN9to12[ActiveAddress] + 160;
      sdata = sdata + (String)LocoAddress[ActiveAddress] + " " + fx;
      break;
    case GRUPO_F1320:
      fx = LocoFN13to20[ActiveAddress];
      sdata = sdata + (String)LocoAddress[ActiveAddress] + " 222 " + fx;
      // do 
      break;
    case GRUPO_F2128:
      fx = LocoFN21to28[ActiveAddress];
      sdata = sdata + (String)LocoAddress[ActiveAddress] + " 223 " + fx;
      break;
  }
  Serial.print("create fun commands: "); Serial.println(sdata);
  return sdata;
}

String Oled::CreateTurnoutCommand(){
  String data = "T" + (String)idturnout + " " +
     (String)Testado[idturnout] + ">";
  return data;
}

String Oled::CreateAccesoryCommand(){
  String data = "a " + (String)(accAddr) + " " +
     (String)accSubAddr + " " + 
     (String)accStatus;
  return data;
}

void Oled::SendCommand(String dataS){
  char com[16];
  dataS.toCharArray(com, 16);
  TextCommand::parse(com);
}
#endif  // USE_KEYBOARD


String Oled::SwInput(){
  String data = "";
  #ifdef USE_SERIALWIFI
    data = data + "WiFi";
  #endif  // USE_SERIALWIFI
  #ifdef USE_SERIALBLUETOOTH
    data = data + " BT";
  #endif  // USE_SERIALBLUETOOTH
  #ifdef USE_SERIALAUX
    data = data + " Aux";
  #endif  // USE_SERIALAUX
  //Serial.println(data);
  return data;

}


#endif
