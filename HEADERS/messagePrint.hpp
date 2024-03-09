/************************************************
** Program name: messagePrint.hpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Header file for messagePrint.cpp.
*************************************************/

#ifndef messagePrint_hpp
#define messagesPrint_hpp

#include "include.hpp"

void messagePrint(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::istringstream& messagesStr);
void messagePrint(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::istringstream& messagesStr, std::ofstream& _outFile);
void messagePrint(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ifstream& messagesStr, std::ofstream& _outFile);

#endif