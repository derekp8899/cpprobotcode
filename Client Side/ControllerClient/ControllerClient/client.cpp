#include "client.h"

int Gamepad::GetPort() {
	return cId + 1;
}

XINPUT_GAMEPAD *Gamepad::GetState() {
	return &state.Gamepad;
}

bool Gamepad::CheckConnection() {
	int controllerId = -1;
	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
			controllerId = i;
	}
	cId = controllerId;
	return controllerId != -1;
}

bool Gamepad::Refresh() {
	if (cId == -1) {
		CheckConnection();
	}
	if (cId != -1) {
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(cId, &state) != ERROR_SUCCESS) {
			cId = -1;
			return false;
		}
		float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);
		float normRX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);
		leftStickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
		leftStickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));
		rightStickX = (abs(normRX) < deadzoneX ? 0 : (abs(normRX) - deadzoneX) * (normRX / abs(normRX)));
		rightStickY = (abs(normRY) < deadzoneY ? 0 : (abs(normRY) - deadzoneY) * (normRY / abs(normRY)));
		leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
		rightTrigger = (float)state.Gamepad.bRightTrigger / 255;
		if (deadzoneX > 0) leftStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) leftStickY *= 1 / (1 - deadzoneY);
		if (deadzoneX > 0) rightStickX *= 1 / (1 - deadzoneX);
		if (deadzoneY > 0) rightStickY *= 1 / (1 - deadzoneY);
		return true;
	} return false;
}

bool Gamepad::IsPressed(WORD button) {
	return (state.Gamepad.wButtons & button) != 0;
}

SOCKET setup(int iResult, SOCKET ConnectSocket){  //function call to setup the connection to the specified server (address and port set in .h file)
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(DEFAULT_ADDRESS, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	return ConnectSocket;
}

int sendData(int iResult, SOCKET ConnectSocket){  //function call that passes controller input to the server
	const char *sendbuf = "Requesting Access";
	bool wasConnected = true;
	bool notDone = false;
	char lXBuffer[20];
	char lYBuffer[20];
	char rXBuffer[20];
	char rYBuffer[20];
	char rTBuffer[20];
	char lTBuffer[20];
	std::string message = "";
	std::string lShoulder = "0";
	std::string rShoulder = "0";
	std::string aButton = "0";
	std::string bButton = "0";
	std::string xButton = "0";
	std::string yButton = "0";
	std::string startButton = "0";
	std::string selectButton = "0";
	const char * msg;

	Gamepad gamepad;

	// Send and Receive until the peer closes the connection
		if (!gamepad.Refresh()) {
			if (wasConnected) {
				wasConnected = false;
				printf("Please connect a controller.\n");
			}
		} else {
			if (!wasConnected) {
				wasConnected = true;
				printf("Controller connected on port %i\n ", gamepad.GetPort());
			} if (gamepad.leftStickX > .1 || gamepad.leftStickX < -.1) {
				sprintf_s(lXBuffer, "%f", (gamepad.leftStickX));
			} if (gamepad.leftStickY > .1 || gamepad.leftStickY < -.1) {
				sprintf_s(lYBuffer, "%f", gamepad.leftStickY);
			} if (gamepad.rightStickX > .1 || gamepad.rightStickX < -.1) {
				sprintf_s(rXBuffer, "%f", gamepad.rightStickX);	
			} if (gamepad.rightStickY > .1 || gamepad.rightStickY < -.1) {
				sprintf_s(rYBuffer, "%f", gamepad.rightStickY);
			} if (gamepad.leftTrigger > .1) {
				sprintf_s(lTBuffer, "%f", gamepad.leftTrigger);
			} if (gamepad.rightTrigger > .1) {
				sprintf_s(rTBuffer, "%f", gamepad.rightTrigger);
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				lShoulder = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				rShoulder = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
				aButton = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_B)) {
				bButton = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_X)) {
				xButton = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_Y)) {
				yButton = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_START)) {
				startButton = "1";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
				selectButton = "1";
			}
			message.append(lXBuffer);
			message.append(" ");
			message.append(lYBuffer);
			message.append(" ");
			message.append(rXBuffer);
			message.append(" ");
			message.append(rYBuffer);
			message.append(" ");
			message.append(rTBuffer);
			message.append(" ");
			message.append(lTBuffer);
			message.append(" ");
			message.append(rShoulder);
			message.append(" ");
			message.append(lShoulder);
			message.append(" ");
			message.append(aButton);
			message.append(" ");
			message.append(bButton);
			message.append(" ");
			message.append(xButton);
			message.append(" ");
			message.append(yButton);
			message.append(" ");
			message.append(startButton);
			message.append(" ");
			message.append(selectButton);
			msg = message.c_str();
			printf("Controller Status: %S\n", message);
			iResult = send(ConnectSocket, msg, 160, 0);
		}
	return iResult;
}

int recieveDistance(int iResult, SOCKET ConnectSocket) {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	printf("Distance from Marker: %c\n\n", recvbuf);
	return iResult;
}