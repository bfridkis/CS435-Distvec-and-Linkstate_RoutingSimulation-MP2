/****************************************************
** Program name: vecToString.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for vecToString function.
*****************************************************/

#include "../HEADERS/vecToString.hpp"

std::string vecToString(std::vector<int>& vec, const char *sep) {
    if (vec.size() > 0) { 
        std::stringstream pathSS;
        std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(pathSS, sep));
        return pathSS.str();
    }
    else { 
        std::string retStr = std::string("-");
        return retStr; }
}

std::string setToString(std::set<int>& set, const char *sep) {
    if (set.size() > 0) { 
        std::stringstream pathSS;
        std::copy(set.begin(), set.end(), std::ostream_iterator<int>(pathSS, sep));
        return pathSS.str();
    }
    else { 
        std::string retStr = std::string("-");
        return retStr; }
}
