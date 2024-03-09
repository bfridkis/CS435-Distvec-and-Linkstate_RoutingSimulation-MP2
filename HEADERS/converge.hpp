/**********************************************
** Program name: converge.hpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Header file for converge.cpp.
***********************************************/

#ifndef converge_hpp
#define converge_hpp

#include "include.hpp"

void converge(int sourceNode, int neighbor, int prevHop, std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT);

#endif