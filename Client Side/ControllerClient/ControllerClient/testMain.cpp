#include "client.h"

int main(int argc, char **argv) {

	SOCKET ConnectSocket = INVALID_SOCKET;
	int iResult = 0;
	int loop = 1, loop2 = 1;
	ConnectSocket = setup(iResult, ConnectSocket);
	while (ConnectSocket == 1) {
		ConnectSocket = setup(iResult, ConnectSocket);
	}
	while (true) {										       //loop to keep the client active
		loop = sendData(iResult, ConnectSocket);
		loop2 = recieveDistance(iResult, ConnectSocket);
		if (loop == 0 || loop2 == 0) {						 //If sendData returns 1 to loop variable, that means the server shutdown, so if statement will be triggered to reestablish connection
			ConnectSocket = INVALID_SOCKET;					//reinitialize ConnectSocket to default
			ConnectSocket = setup(iResult, ConnectSocket); //Connect to the new server
			loop = sendData(iResult, ConnectSocket);	  //reset counters so we maintain connection
			loop2 = recieveDistance(iResult, ConnectSocket);
		}
	}
}