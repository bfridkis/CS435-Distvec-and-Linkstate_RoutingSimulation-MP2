/****************************************************
** Program name: vecToSring.hpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Header file for vecToString.cpp.
****************************************************/

#ifndef vecToSring_hpp
#define vecToSring_hpp

#include "include.hpp"

std::string vecToString(std::vector<int>& vec, const char *sep = "");
std::string setToString(std::set<int>& set, const char *sep = "");
#endif