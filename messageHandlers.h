#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

#pragma once

#ifndef __message_handlers__
#define __message_handlers__

/**
* utility function that does joining strings into a valid message with correct ending delimiter
* @param: base <std::string *> [IN][OUT]: the string contain the message after modified
* @param: joinString <char *> [IN]: the string that will be formatted (adding delimiter to the end) and join into the base string
* @param: delimiter <char *> [IN]: ending delimiter. 
* @return <int>: 0 if any of the paramters is null. 1 if succeeded.
**/
int joinString(std::string * base, char * joinString, char * delimiter);

/**
* utility function that split one big string into seperate different string using a delimiter.
* @param: container <std::vector<std::string> *>[OUT]: this is the container that will holds all seperated string
* @param: base <char *>[IN]: the string that will be splitted.
* @param: delimiter <char *>[IN]: the delimiter used as seperator
* @return <int>: 0 if the input string is an invalid string (not enough seperators/no seperators at all). 1 if successfully split.
**/
int splitString(std::vector<std::string> * container, char * base, char * delimiter);

#endif