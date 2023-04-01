#ifndef COMMINTERFACE_H_
#define COMMINTERFACE_H_


class CommManager {
public:
	// static void update();
	// static void registerInterface(CommInterface *interface);
	// static void showConfiguration();
	// static void showInitInfo();
	static void printf(const char *fmt, ...);
private:
	// static CommInterface *interfaces[10];
	// static int nextInterface;
};

#endif /* COMMINTERFACE_H_ */