# DCCpp_LMD

DCCpp LMD Librería DCC++ para Arduino

Que es y para que sirve DCCpp LMD?

DCCpp LMD es una adapatación de la librería DCCpp. Diseñada para placa Arduino MEGA cuenta con diferentes añadidos como pantalla Oled, sonido y multiples puertos seriales. Con esta versión podemos utilizar todos los puertos al mismo tiempo para mandar o recibir los comandos de la central.



<strong>
  
*- Pantalla Oled con librería U8G2lib 

*- Retromodulos S88

*- Serial → PC

*- Serial1 → Wifi

*- Serial2 → Bluetooth

*- Serial3 → Serial Auxiliar

*- Sonido 
  
</strong>

En la pantalla Oled obtendremos información tales como desvíos y sensores almacenados en la EEPROM, comando recibido, estado del ultimo desvío/salida activadas. También muestra los datos de la última locomotora como ID, velocidad y dirección. Igualmente, cuando almacenemos o borremos algun elemento de la memoria EEPROM

Para instalar la central DCCpp LMD solo hay que tener la librería DCCpp LMD en la carpeta <em>libraries</em> de Arduino. Luego abrimos el IDE de Arduino y en <em>Ejemplos -> DCCppLMD -> SerialBluetoothDCCpp</em>


![alt text](https://github.com/Peyutron/DCCpp_LMD/blob/main/DCCpp_LMD/extras/Images/ardumoto-l298p_pines.jpg?raw=true "Shield Ardumoto")



![alt text](https://github.com/Peyutron/DCCpp_LMD/blob/main/DCCpp_LMD/extras/Images/DCCpp_Mega_oledESP8266Buzzmax471.jpg?raw=true "DCCpp Wifi" )


Nos podemos ahorrar el soldar componentes ya que para el módulo <strong>ESP-01</strong> existe un adaptador para tener una conexión Rx/Tx con alimentación a 5V 

![alt text](https://github.com/Peyutron/DCCpp_LMD/blob/main/DCCpp_LMD/extras/Images/Modulo_serial_ESP8266.jpg?raw=true "Adaptador ESP-01")

![alt text](https://github.com/Peyutron/DCCpp_LMD/blob/main/DCCpp_LMD/extras/Images/DCCOledSSD1306_03.png?raw=true "Pantalla Oled I2c")



Con el pequeño parlante podremos percibir de una manera mas intuitiva si algo va bien o mal en la cental, cuenta con sonidos para <strong>encendido/apagado</strong> y <strong>confirmación o fallo</strong>
Estos se pueden modificar en el archivo <em>Sound.cpp</em>

*-LMD 23/04/2023 V2.0.4
  - Añadido teclado 4x3 y encoder rotativo en Keyboard.h y Keyboard.cpp. Los pines
    definidos para el teclado y encoder se encuentran en Keyboard.h
  - Con Keyboard se añaden varias funciones en Oled.cpp. LocomotiveOled, TurnoutOled.
  - Pantalla para uso de locomotoras (1). Necesita Keyboard
  - Pantalla para uso de desvíos (2). Necesita Keyboard
  - como usar el teclado en: URL) 
  - Oled.cpp se actualiza solo cuando cambian los datos que se muestran.
  - Imagen de inicio, opcional.
  - Algunas partes han sido traducidas a Español ES

*-LMD 23/04/2023 V2.0.3
  - Retromodulos S88 añadido (por Philippe -> http://lormedy.free.fr/)
  - Arreglos sonido
  - Comando '#' que muestra el número de locomotoras que puede administrar la central
  - Comando 'I' (solo WiFi) Muestra la IP del dispositivo Wifi WebSocket (https://github.com/Peyutron/DCC-ESP8266-Websocket-Server)

*-LMD 05/04/2023 V2.0.2
  - Arreglos en TextCommand.cpp ya que algunos comandos no funcionaban correctamente (comandos 's', 'E' y 'F')

*-LMD 01/04/2023 V2.0.1
- Datos seriales unificados en CommInterface.cpp
- Implementación del puerto Serial1 como SerialWifi (SerialWifi.h) con WebSocket "ws://"
- Implementación del puerto Serial2 como SerialBluetooth (SerialBluetooth.h)
- Implementación del puerto Serial3 como SerialAUX (SerialAux.h)
- Nueva librería U8G2lib probado en pantalla 
  *- SSD1603 128x64
- Monitor Serial en pantalla
- Monitor de consumo 


DCCpp modificado por Peyutron. Todos los cambios y montajes estan basados en un circuito general. Es recomendable mierar el datasheet y a los pines de conexión ya que dependeran del fabricante. No nos hacemos responsables de posibles fallos. Todas las modificaciones son bajo tu propia responsabilidad.




License
Copyright (c) 2017/2020 Locoduino.org. All right reserved. Copyright (c) 2017/2020 Thierry Paris. All right reserved.

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
