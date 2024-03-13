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
void converge(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, int sourceNode, int prevNode, std::vector<int>* newPath, int newPathCost);
//void converge(int sourceNode, std::vector<std::map<int, int>> &_TT, std::vector<std::map<int, std::pair<int, int>>> &_FT);
void converge(int sourceNode, std::vector<std::map<int, int>> &_TT, std::vector<std::map<int, std::pair<vector<int>, int>>> &_FT, std::vector<std::pair<int,int>> &tiesTracker) {	

#endif