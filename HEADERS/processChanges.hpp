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
void processChanges(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ifstream& _changesInput, std::ifstream& _messagesInput, std::ofstream& _outFile);
void processChanges(std::vector<std::map<int, std::pair<int, int>>> &_FT, std::vector<std::map<int, int>> &_TT,  std::ifstream& _changesInput, std::ifstream& _messagesInput, std::ofstream& _outFile);
//void processChanges(std::vector<std::map<int, std::pair<int, int>>> &_FT, std::vector<std::map<int, std::pair<int, int>>> &_FT_invert, std::vector<std::map<int, int>> &_TT,  std::ifstream& _changesInput, std::ifstream& _messagesInput, std::ofstream& _outFile);
void processChanges(std::vector<std::map<int, std::pair<std::vector<int>, int>>> &_FT, std::vector<std::map<int, int>> &_TT,  std::ifstream& _changesInput, std::ifstream& _messagesInput, std::ofstream& _outFile);

#endif