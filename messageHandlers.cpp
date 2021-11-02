#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include "messageHandlers.h"

int joinString(std::string * output, char * toJoin, char * delimiter) {
	if (toJoin == NULL || delimiter == NULL || output == NULL) {
		return 0;
	}
	output->append(toJoin);
	output->append(delimiter);
	return 1;
}

int splitString(std::vector<std::string>  *output, char * str, char * delimiter) {
	std::string wrapper(str);
	std::string delimiterWrapper(delimiter);
	while (wrapper.length() > 0) {
		int index = wrapper.find(delimiterWrapper);
		if (index != std::string::npos) {
			output->push_back(wrapper.substr(0, index));
			wrapper.erase(0, index + delimiterWrapper.length());
		}
		else return 0;
	}
	return 1;
}