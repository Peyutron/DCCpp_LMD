/**********************************************************************

Turnout.h renamed from Accessories.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef Turnout_h
#define Turnout_h

#include "Arduino.h"

#include "DCCpp.h"

#ifdef USE_TURNOUT
	

/** Datos asociados a cada desvío.*/
struct TurnoutData {
  byte tStatus;				//	Estado actual del desvío: 0 para Off, 1 para On.
  byte subAddress;		//	La subdirección del decodificador que controla este desvío (0-3).
  int id;							//  El ID numérico (0-32767) del desvío a controlar.
  int address;  			//	La dirección principal del decodificador que controla este desvío (0-511).
};

/**	\addtogroup commandsGroup
 	CREA UN NUEVO DESVIO
	----------------------

	<b>
	\verbatim
	<T ID ADDRESS SUBADDRESS>
	\endverbatim
	</b>

	crea una nueva ID de participación, con DIRECCIÓN y SUBDIRECCIÓN especificadas
	Si la ID del existe, esta se actualizara con la DIRECCIÓN y SUBDIRECCIÓN especificadas
	

	- <b>ID</b>: El ID numérico (0-32767) del desvío a controlar.
	- <b> ADDRESS</b>:  La dirección principal del decodificador que controla este desvío (0-511).
	- <b> SUBADDRESS</b>: La subdirección del decodificador que controla este desvío (0-3).

	devuelve: <b>\<O\></b> si es correcto y <b>\<X\></b> si no (ej. Fuera de memoria)
*/

/**	\addtogroup commandsGroup
	DELETES AN EXISTING TURNOU%T
	----------------------------

	<b>
	\verbatim
	<T ID>
	\endverbatim
	</b>

	Elimina la ID de un desvío definido
	

	- <b>ID</b>: El ID numérico (0-32767) del desvío a controlar.

	devuelve: <b>\<O\></b> si es correcto y <b>\<X\></b> si no (ej. Fuera de memoria)
*/

/**	\addtogroup commandsGroup
 	LISTA DE TODOS LOS DESVÍOS
	------------------

	<b>
	\verbatim
	<T>
	\endverbatim
	</b>

	devuelve: <b>\<H ID ADDRESS SUBADDRESS THROW\></b>p ara cada desvío definido o <b>\<X\></b> Si no hay desvío definido

	- <b>ID</b>: El ID numérico (0-32767) del desvío a controlar.
	- <b>ADDRESS</b>:  La dirección principal del decodificador que controla este desvío (0-511).
	- <b>SUBADDRESS</b>: La subdirección del decodificador que controla este desvío (0-3).
*/

/**	\addtogroup commandsGroup
	THROWS AN EXISTING TURNOU%T
	----------------------------

	<b>
	\verbatim
	<T ID THROW>
	\endverbatim
	</b>

	Establece el ID de desvío en la posición "cruzado" o "recto"

	- <b>ID</b>: El ID numérico (0-32767) del desvío a controlar.
	- <b>THROW</b>:  0 (recto) or 1 (cruzado)

	devuelve: <b>\<H ID THROW\></b>, o <b>\<X\></b> si la ID del desvío no existe
*/

/** DCC++ BASE STATION puede realizar un seguimiento de la dirección de cualquier desvío que esté controlado
por un decodificador accesorio estacionario DCC.

Si se utiliza TextCommand, todos los desvíos, así como cualquier otro accesorio DCC
conectado, siempre se puedrá operar usando el comando de desvío DCC BASE STATION.

Cuando se controla como tal, Arduino actualiza y almacena la dirección de cada desvío en la memoria EEPROM para 
que se mantengan incluso sin energía.  ASe genera una lista de las direcciones actuales de cada desvío en la forma <b>\<H ID THROW\></b>
por este boceto cada vez que se invoca el comando de estado <b>\<s\></b>.  Esto proporciona una manera eficiente de inicializar
las direcciones de los desvíos que están siendo monitoreados o controlados por una interfaz separada o un programa GUI.

Una vez que todos los desvíos se hayan definido correctamente, use el comando <b>\<E\></b> para almacenar sus definiciones en EEPROM.
Si luego realiza ediciones/adiciones/eliminaciones a las definiciones de desvío, debe invocar el comando <b>\<E\></b> si desea que esas
nuevas definiciones sean actualizadas en la EEPROM. También puede borrar todo lo almacenado en la EEPROM invocando el comando <b>\<e\></b>.

Sin TextCommand, este es un uso clásico de C++. AEl usuario crea una instancia de la estructura,
y funciones como begin() y activar() se pueden aplicar en esta instancia.
También se pueden usar load() y store() de la clase (si USE_EEPROM está activado).
*/
struct Turnout{
	struct TurnoutData data;	/**< Datos asociados a este desvío.*/

	/** Inicializa un desvío creado por una instacia regular de C++.
	@param id El ID numérico (0-32767) del desvío a controlar.
	@param add	La dirección principal del decodificador que controla este desvío (0-511).
	@param subAdd	La subdirección del decodificador que controla este desvío (0-3).
	@remark Should not be used in command line mode.
	*/
	void begin(int id, int add, int subAdd);
	/** Fuerza la creación de un desvío por linea de comando.
	@param El ID numérico (0-32767) del desvío a controlar.
	@param add	La dirección principal del decodificador que controla este desvío (0-511).
	@param subAdd	La subdirección del decodificador que controla este desvío (0-3).
	@remark Should not be used in C++ mode.
	*/
	void set(int id, int add, int subAdd);
	/** Cambia el estaddo de activacion del desvíoChange the activation state of the turnout.
	@param s new state : 0 for off, 1 for on. Default is 1.
	*/
	void activate(int s = 1);
	/** Inactivate the turnout.
	*/
	inline void inactivate() { activate(0); }
	/** Comprueba si la salida está activada o no.
	@devuelve True si la salida está activada. De lo contrario falso (por supuesto...).
	*/
	inline bool isActivated() { return this->data.tStatus > 0; }

#if defined(USE_EEPROM)	|| defined(USE_TEXTCOMMAND)
	static Turnout *firstTurnout;	/**< Address of the first object of this class. NULL means empty list of turnouts. Do not change it !*/
	Turnout *nextTurnout;			/**< Address of the next object of this class. NULL means end of the list of turnouts. Do not change it !*/
	/** Obtiene un desvío.
	@param id	El ID numérico (0-32767) del desvío a controlar.
	@return The found turnout or NULL if not found.
	@remark Only available when USE_EEPROM or USE_TEXTCOMMAND is defined.
	*/
	static Turnout* get(int id);
	/** Elimina un desvío.
	@param id	El ID numérico (0-32767) del desvío a controlar.
	@remark Solo disponible cuando USE_EEPROM o USE_TEXTCOMMAND esta definido.
	*/
	static void remove(int id);
	/** Obtiene el múmero total de desvíos.
	@devuelve el número de desvíos.
	@remark Solo disponible cuando USE_EEPROM o USE_TEXTCOMMAND esta definido.
	*/
	static int count();

#ifdef DCCPP_PRINT_DCCPP
	/** Muestra el contenido de todos los desvíos.
	@remark Solo disponible cuando DCCPP_PRINT_DCCPP esta definido.
	*/
	static void show();
#endif

#if defined(USE_EEPROM)
	int eepromPos;	/**< EEPROM starting address for this Turnout storage.*/

	/** Carga el contenido de todos los desvíos desde la EEPROM.
	@remark Solo disponible cuando USE_EEPROM esta definido.
	*/
	static void load();
	/** Store the content of all turnouts in EEPROM.
	@remark Solo disponible cuando USE_EEPROM esta definido.
	*/
	static void store();
#endif

#endif

#if defined(USE_TEXTCOMMAND)
	/** Ejecuta el comando de acuerdo con la cadena de comando dada.
	@param cadena c para interpretar... Consulte la sintaxis en la descripción de la clase.
	@remark Solo disponible cuando USE_TEXTCOMMAND esta definido.
	*/
	static bool parse(char *c);
	/** Creates a new turnout in the list..
	@param id El ID numérico (0-32767) del desvío a controlar.
	@param add	La dirección principal del decodificador que controla este desvío (0-511).
	@param subAdd	La subdirección del decodificador que controla este desvío (0-3).
	@remark No debe usarse en modo C++.
	@remark Solo disponible cuando USE_TEXTCOMMAND esta definido.
	*/
	static Turnout *create(int id, int add, int subAdd);
#endif

}; // Turnout
  
#endif
#endif
