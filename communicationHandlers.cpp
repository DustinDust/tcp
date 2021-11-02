#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "messageHandlers.h"
#include "communicationHandlers.h"

#pragma comment(lib, "Ws2_32.lib")


int handleSend(SOCKET connection, char * message, int messageLen) {
	int index = 0, ret = 0, attempts = 0;
	int left = messageLen;
	std::string messageWrapper(message);
	char data[BUFF_SIZE];
	strcpy_s(data, messageWrapper.length() + 2, messageWrapper.c_str());
	while (left > 0) {
		ret = send(connection, data, left, 0);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == 10054) {
				std::cout << "> Cannot locate Server. Please restart the program (10054)\n";
			}
			else std::cout << "> ERROR sending data: " << WSAGetLastError() << std::endl;
			return 0;
		}
		left -= ret;
		index += ret;
		messageWrapper.erase(0, ret);
		strcpy_s(data + index, messageWrapper.length() + 2, messageWrapper.c_str());
		attempts++;
		if (attempts > 10000) {
			return -1;
		}
	}
	return 1;
}


int handleResponse(SOCKET connection, char * message, std::vector<std::string> * container, char * delimiter = ENDING_DELIMITER) {
	std::string messageWrapper("");
	std::string delimiterWrapper(delimiter);
	int ret, delmtrLength = strlen(delimiter);
	while (true)
	{
		ret = recv(connection, message, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				std::cout << "> Timed - out!...";
				return -1;
			}
			if (WSAGetLastError() == WSAECONNRESET) {
				return 0;
			}
			else {
				std::cout << "> ERROR receiving data - " << WSAGetLastError() << std::endl;
				return -1;
			}
		}
		if (ret == 0) {
			return 0;
		}
		message[ret] = 0;
		messageWrapper.append(message);
		if (messageWrapper.substr(messageWrapper.length() - delmtrLength) == delimiter) {
			break;
		}
	}
	splitString(container, &messageWrapper[0], delimiter);
	return messageWrapper.length();
}