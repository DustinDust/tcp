#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "messageHandlers.h"
#include "communicationHandlers.h"
#define BUFF_SIZE 1024
#pragma comment(lib, "Ws2_32.lib")


int SERVER_PORT = 5500;
char SERVER_ADDR[INET_ADDRSTRLEN] = "127.0.0.1";

/**
* this function gets userinput to a buffer with fixed size (1024 as defined in BUFF_SIZE) and automatically append an ending delimiter to it
* @param: buff<char *>[IN][OUT]: the will-be-modified input buffer that will be used to store readied message to send to the server
* @return <int>: 0 if user enter an empty string or a string too big for the BUFF_SIZE. Else modify the @param buff and return the length of the final buffer;
**/
int getUserInput(char * buff) {
	char inputBuff[BUFF_SIZE * 10];
	std::cout << "> Send to server: ";
	std::string wrapper("");
	gets_s(inputBuff, BUFF_SIZE * 10);
	if (strlen(inputBuff) > BUFF_SIZE) {
		std::cout << "> Input too long. Exiting...\n";
		return 0;
	}
	if (strlen(inputBuff) == 0) {
		std::cout << "> Empty input. Exiting...\n";
		return 0;
	}
	else {
		joinString(&wrapper, inputBuff, ENDING_DELIMITER);
		strcpy_s(buff, wrapper.length() + 2, wrapper.c_str());
		return wrapper.length();
	}
	
	
	
	
}

/**
* this function handle the message received from the server. Typically responses.
* @param: message<std::string *> [IN][OUT]: the message received from server after communicating. Can contain success/error message. Will be modifed
* @return: <int>: 1 if the message is a positive(success) answer. 0 if the message is a negative(error) answer. -1 if the answer from server is not recognized/invalid
*/
int handleMessage(std::string * message) {
	if (message->at(0) == '+') {
		message->erase(0, 1);
		return 1;
	}
	else if (message->at(0) == '-') {
		message->erase(0, 1);
		return 0;
	}
	else return -1;
}

int main(int argc, char * argv[]) {

	//handling commandline arguments
	if (argc == 3) {
		strcpy_s(SERVER_ADDR, argv[1]);
		std::string portString(argv[2]);
		std::stringstream tempStream(portString);
		int tempValue = SERVER_PORT;
		tempStream >> tempValue;
		if (tempValue == 5500 && portString != "5500") {
			std::cout << "> Invalide port number (" << portString << "). The default port 5500 will be used instead;\n\n";
		}
		SERVER_PORT = tempValue;
	}
	else {
		std::cout << "\n> Not enough command-line args detected. \n> Proceed to use default value of server IP (127.0.0.1) and Port (5500)\n\n";
	}

	//initialize WSA
	WSAData data;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &data)) {
		std::cout << "> Winsock version 2.2 is not supported \n";
	}

	//client socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		std::cout << "> ERROR creating client socket (" << WSAGetLastError() << ") \n";
		return 0;
	}

	//init server info with data from user input
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	int IPValidation = inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	if (IPValidation <= 0) {
		std::cout << "> Invalid IP address. Exitting...\n";
	}

	//attempt to connect to the server
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		std::cout << "> Error connecting to the server (" << WSAGetLastError() << ") \n";
		return 0;
	}
	std::cout << "> Connected!\n";
	
	while (1) {
		//gettin user's input;
		int ret = INT_MIN;
		char buff[BUFF_SIZE];
		int inputLength = getUserInput(buff);
		if (inputLength == 0) {
			break;
		}

		//sending and handling rejections
		ret = handleSend(client, buff, inputLength);
		if (ret == 0) {
			memset(buff, BUFF_SIZE, 0);
			continue;
		}
		else if (ret == -1) {
			std::cout << "> Cannot send data: attempts exceeded\n";
			memset(buff, BUFF_SIZE, 0);
			continue;
		}

		
		//handling responses and/or rejections;
		char rBuff[BUFF_SIZE];
		std::vector<std::string> responsesContainer;
		ret = handleResponse(client, rBuff, &responsesContainer, ENDING_DELIMITER);
		if (ret == -1) {
			responsesContainer.erase(responsesContainer.begin(), responsesContainer.end());
			memset(buff, 0, BUFF_SIZE);
			memset(rBuff, 0, BUFF_SIZE);
			continue;
		}
		for (auto i : responsesContainer) {
			ret = handleMessage(&i);
			if (ret == 1) {
				std::cout << "> Succeeded: " << i << std::endl;
			}
			else if (ret == 0) {
				std::cout << "> Failed: " << i << std::endl;
			}
			else {
				std::cout << "> Error: message from server isn't in the correct format." << std::endl;
			}
		}
		

		//clear some data for the next loop;
		responsesContainer.clear();
		memset(buff, 0, BUFF_SIZE);
		memset(rBuff, 0, BUFF_SIZE);
	}
	closesocket(client);

	WSACleanup();
}

