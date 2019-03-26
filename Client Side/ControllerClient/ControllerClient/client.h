#ifndef CLIENT_H_
#define CLIENT_H_
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <Xinput.h>
#include <iostream>
#include <string>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define DEFAULT_ADDRESS "192.168.1.1"


SOCKET setup(int, SOCKET);
int sendData(int, SOCKET);
int recieveDistance(int, SOCKET);

class Gamepad {
private:
	int cId;
	XINPUT_STATE state;
	float deadzoneX;
	float deadzoneY;

public:
	Gamepad() : deadzoneX(0.05f), deadzoneY(0.02f) {}
	Gamepad(float dzX, float dzY) : deadzoneX(dzX), deadzoneY(dzY) {}

	XINPUT_GAMEPAD *GetState();
	int GetPort();
	float leftStickX;
	float leftStickY;
	float rightStickX;
	float rightStickY;
	float leftTrigger;
	float rightTrigger;
	bool CheckConnection();
	bool Refresh();
	bool IsPressed(WORD);
};

#endif // !CLIENT_H_

