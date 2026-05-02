#ifndef COMMINTERFACE_H_
#define COMMINTERFACE_H_


class CommManager
{
	public:
	
		static void printf(const char *fmt, ...);
	
		// Nueva función para obtener el último mensaje formateado
    	static const char* getLastOutput();
    
    	// Opcional: limpiar el buffer si es necesario
    	static void clearLastOutput();
	
	private:
    	static char lastOutputBuffer[128];  // Almacena el último mensaje
};

#endif /* COMMINTERFACE_H_ */