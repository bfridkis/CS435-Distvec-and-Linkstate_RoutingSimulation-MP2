/****************************************************
** Program name: outputFunctions.hpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Header file for outputFunctions.cpp.
****************************************************/

#ifndef outputFunctions_hpp
#define outputFunctions_hpp

#include "include.hpp"

void consoleOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT);
void fileOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ofstream _outFile);

#endif