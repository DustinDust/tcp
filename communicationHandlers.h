#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "messageHandlers.h"
#pragma comment(lib, "Ws2_32.lib")

#ifndef communication_handler
#define communication_handler
#define BUFF_SIZE 1024
#define ENDING_DELIMITER "\r\n"

/**
* this functions handle the sending procedures int TCP connection.
* Will keep sending until everything is sent, or some errors occur.
* @param: connection<SOCKET> [IN]: the connection socket;
* @param: message<char *>[IN]: the message to send
* @param: messageLen<int> [IN]: size of the message
* @return <int>: will return 0 if error occurs. returns -1 if the data cannot be send after 10000 attempts, return 1 if succeed
**/
int handleSend(SOCKET connection, char * message, int messageLen);

/**
* this function handles the receiving procedures. Will keep trying to receive until a valid formatted string is received (message ending with a delimiter).
* this function will try to seperate received messages into individuals of std::string using the delimiter as the seperator;
* @param: connection<SOCKET>[IN]: the socket represents the TCP connection.
* @param: message<char *>[IN]: message buffer. Fixed size 1024 (can't be changed. Sorry)
* @param: container<std::vector<std::string> *> [OUT]: the container for all the message received after seperating.
* @param: delimiter<char *>[IN]: the ending delimiter that will be used to validate message and to seperate messages.
* @return <int>: returns -1 if failed to receive message due to errors. returns 0 if the connection is lost (client/server disconnected). Else return the size of the message string;
**/
int handleResponse(SOCKET connection, char * message, std::vector<std::string> * container, char * delimiter);

#endif // !communication_handler
