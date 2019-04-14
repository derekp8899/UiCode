/*
Author: Zach Hause
zachary.hause@ndus.edu
UND Robotic Mining Team

This file handles creating a socket connection,
and taking in input from any Xinput device like
an Xbox controller.
*/

#include "client.h"

int Gamepad::GetPort() {
	return cId + 1;
}

XINPUT_GAMEPAD* Gamepad::GetState() {
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

SOCKET setup(int iResult, SOCKET ConnectSocket) {  //function call to setup the connection to the specified server (address and port set in .h file)
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

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

int sendData(int iResult, SOCKET ConnectSocket) {  //function call that passes controller input to the server
	const char* sendbuf = "Requesting Access";
	bool wasConnected = true;
	bool notDone = false;
	char lXBuffer[20];
	char lYBuffer[20];
	char rXBuffer[20];
	char rYBuffer[20];
	char rTBuffer[20];
	char lTBuffer[20];
	std::string message = "";
	lShoulder = "0";
	rShoulder = "0";
	 a = "0";
	 b = "0";
	 x = "0";
	 y = "0";
	 startButton = "0";
	selectButton = "0";
	const char* msg;
	Gamepad gamepad;

	// Send and Receive until the peer closes the connection
	while (1) {
		if (!gamepad.Refresh()) {
			if (wasConnected) {
				wasConnected = false;
				//printf("Please connect a controller.\n");
			}
		}
		else {
			if (!wasConnected) {
				wasConnected = true;
				//printf("Controller connected on port %i\n ", gamepad.GetPort());
			} if (gamepad.leftStickX > .1 || gamepad.leftStickX < -.1) {
				sprintf_s(lXBuffer, "%f", (gamepad.leftStickX));
				lXstick = gamepad.leftStickX;
			} else {
				sprintf_s(lXBuffer, "%f", (0.0));
				lXstick = gamepad.leftStickX;
			}if (gamepad.leftStickY > .1 || gamepad.leftStickY < -.1) {
				sprintf_s(lYBuffer, "%f", gamepad.leftStickY);
				lYstick = gamepad.leftStickY;
			}else {
				sprintf_s(lYBuffer, "%f", (0.0));
				lYstick = gamepad.leftStickY;
			} if (gamepad.rightStickX > .1 || gamepad.rightStickX < -.1) {
				sprintf_s(rXBuffer, "%f", gamepad.rightStickX);
				rXstick = gamepad.rightStickX;
			}else {
				sprintf_s(rXBuffer, "%f", (0.0));
				rXstick = gamepad.rightStickX;
			} if (gamepad.rightStickY > .1 || gamepad.rightStickY < -.1) {
				sprintf_s(rYBuffer, "%f", gamepad.rightStickY);
				rYstick = gamepad.rightStickY;
			}else {
				sprintf_s(rYBuffer, "%f", (0.0));
				rYstick = gamepad.rightStickY;
			} if (gamepad.leftTrigger > .1) {
				sprintf_s(lTBuffer, "%f", gamepad.leftTrigger);
				lTrigger = gamepad.leftTrigger;
			}else {
				sprintf_s(lTBuffer, "%f", (0.0));
				lTrigger = 0.0;
			} if (gamepad.rightTrigger > .1) {
				sprintf_s(rTBuffer, "%f", gamepad.rightTrigger);
				rTrigger = gamepad.rightTrigger;
			}else {
				sprintf_s(rTBuffer, "%f", (0.0));
				rTrigger = 0.0;
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
				lShoulder = "1";
			} else{
				lShoulder = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
				rShoulder = "1";
			} else {
				rShoulder = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_A)) {
				a = "1";
			} else {
				a = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_B)) {
				b = "1";
			} else {
				b = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_X)) {
				x = "1";
			} else {
				x = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_Y)) {
				y = "1";
			} else {
				y = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_START)) {
				startButton = "1";
			} else {
				startButton = "0";
			} if (gamepad.IsPressed(XINPUT_GAMEPAD_BACK)) {
				selectButton = "1";
			} else {
				selectButton = "0";
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
			message.append(a);
			message.append(" ");
			message.append(b);
			message.append(" ");
			message.append(x);
			message.append(" ");
			message.append(y);
			message.append(" ");
			message.append(startButton);
			message.append(" ");
			message.append(selectButton);
			msg = message.c_str();
			printf("\rController Status: %s", message);
			iResult = send(ConnectSocket, msg, 160, 0);
			break;
		}
	}
	return iResult;
}

int recieveDistance(int iResult, SOCKET ConnectSocket) {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	LFspd=0,LRspd=0,RFspd=0,RRspd=0,dist=0;
	sscanf_s(recvbuf,"%f %f %f %f %f",&LFspd,&RFspd ,&LRspd,&RRspd,&dist);
	//printw("\n\nour recv buffer: %s\n", recvbuf);
	//printw("our speeds: %f %f %f %f %f", LFspd, RFspd, LRspd, RRspd, dist);
	//printf(" Distance from Marker: %c", recvbuf);
	return iResult;
}