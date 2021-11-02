#include<iostream>
#include<vector>
#include<sstream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include "messageHandlers.h"
#include "communicationHandlers.h"
#define SERVER_ADDR "127.0.0.1"
int SERVER_PORT = 5500; //default PORT number 5500
#pragma comment (lib,"Ws2_32.lib")

/**
* Defines the functionality of the server. Handle the message from clients. 
Evaluate if the input string cointains non-numeric value or not. 
If not count the Sum of all the numeric character in the input string
* @param: message <std::string>[IN]: input string that will be evaluated
* @param: response <char *>[OUT]: the output response that can the be used to send back to the clients. Can contain error message or successful answer
* @return <int> : the length of the response.
**/
int handleMessage(std::string message, char * response) {

	std::string responseMessage;
	bool check = true;
	for (char i : message) {
		if (i > '9' || i < '0') {
			check = false;
			break;
		}
	}
	if (check) {
		int Sum = 0;
		for (char i : message) {
			Sum += i - '0';
		}
		responseMessage.append("+").append(std::to_string(Sum)).append(ENDING_DELIMITER);
	}
	else {
		responseMessage.append("-").append("Input string contains non-numeric characters").append(ENDING_DELIMITER);
	}
	strcpy_s(response, responseMessage.length() + 1, responseMessage.c_str());
	return responseMessage.length();
}


int main(int argc, char * argv[]) {
	//handling commandline args
	if (argc == 2) {
		std::string portString(argv[1]);
		std::stringstream tempStream(portString);
		int tempValue = SERVER_PORT;
		tempStream >> tempValue;
		if (tempValue == 5500 && portString != "5500") {
			std::cout << "> Invalide port number (" << portString << "). The default port 5500 will be used instead;\n\n";
		}
		SERVER_PORT = tempValue;
	}
	else {
		std::cout << "\n> No command-line args detected. \n> Proceed to use default Port value (5500)\n\n";
	}

	//initialize WSA
	WSAData data;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &data)) {
		std::cout << "> Winsock version 2.2 is not supported \n";
	}

	//construct a socket for listening connect requests
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET) {
		std::cout << "> ERROR creating listening server socket (" << WSAGetLastError() << ") \n";
		return 0;
	}

	//bind listening socket to this server adderss;
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	int attempts = 10;
	while (attempts > 0) {
		if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
			std::cout << "> Error: cannot bind to this address - " << WSAGetLastError() << ". Attempts left: " << attempts - 1 << std::endl;
			attempts--;
			if (attempts == 0) {
				return 0;
			}
			else continue;
		}
		else break;
	}

	//place the listening socket into listen state
	if (listen(listenSock, 10)) {
		std::cout << "> ERROR place server socket in state LISTEN.\n";
	}
	std::cout << "> Server started!\n";

	//communication
	while (1) {
		//some necessary variables
		sockaddr_in clientAddr;
		char buff[BUFF_SIZE], clientIP[INET_ADDRSTRLEN];
		int ret, clientAddrLen = sizeof(clientAddr), clientPort;
		std::vector<std::string> messageContainer;
		SOCKET connSock;


		//accept next connection from buffer
		connSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR) {
			std::cout << "> Cannot permit incoming connection (" << WSAGetLastError() << ") \n";
			continue;
		}
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			std::cout << "> Accepting connection from " << clientIP << ":" << clientPort << "!\n";
		}

		//sending/receiving messages.
		while (1) {
			ret = handleResponse(connSock, buff, &messageContainer, ENDING_DELIMITER);
			if (ret == -1) {
				messageContainer.clear();
				break;
			}
			else if (ret == 0) {
				std::cout << "> Connection ended. Proceed to next clients...\n";
				messageContainer.clear();
				break;
			}
			else {
				for (auto i : messageContainer) {
					std::cout << "> Receive from client [" << clientIP << ":" << clientPort << "]: \"" << i << "\" \n";
					char response[BUFF_SIZE];
					int resl = handleMessage(i, response);
					ret = handleSend(connSock, response, resl);
					if (ret == -1) {
						std::cout << "> Attempts limit exceeded\n";
						continue;
					}
					if (ret == 0) {
						continue;
					}
					Sleep(1000);
				}


			}
			//clear buffer
			messageContainer.clear();
		}

		//clear some data for next loop
		memset(&clientAddr, 0, sizeof(clientAddr));
		memset(&buff, 0, BUFF_SIZE);
		closesocket(connSock);
	}
	closesocket(listenSock);

	WSACleanup();

}