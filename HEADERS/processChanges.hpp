/**************************************************
** Program name: processChanges.hpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Header file for processChanges.cpp.
***************************************************/

#ifndef processChanges_hpp
#define processChanges_hpp

#include "include.hpp"

void processChanges(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::stringstream& _changesInput);

#endif