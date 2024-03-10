/******************************************************
** Program name: tieBreaker.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for messagePrint functions.
*******************************************************/

#include "../HEADERS/tieBreaker.hpp"

//Tie breaker function. Next hop lowest node number wins the tie. Returns 1 if pathA is winner, 0 if pathB is winner, and -1 if paths are identical. Will not handle paths that are identical for the entirety of one path but of dissimilar lenghts. (However, these should never be passed as they should never have the same cost.)
int tieBreaker(std::vector<int> pathA, std::vector<int> pathB) {
	if(pathA.length() == 1) { return 1 }
	if(pathB.length() == 1) { return 0 }
	std::vector<int>::iterator aIt, bIt;
	for(aIt = pathA.begin(), bIt = pathB.begin(); aIt != pathA.end() && bIt != pathB.end(); aIt++, bIt++) {
			//std::cout << "new path node: " << *aIt << " existing path node: " << *bIt << std::endl;
		if (*aIt < *bIt) {
			std::cout << "Did we get to aIt < bIt...?? *aIt: " << *aIt << " *bIt: " << *bIt << std::endl;
			return 1;
		}
		else if (*aIt > *bIt) {
			return 0;
		}
	}
	return -1;
}