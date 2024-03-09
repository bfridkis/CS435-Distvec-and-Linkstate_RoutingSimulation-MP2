/**********************************************************************************
** Program name: distvec.cpp
** Class name: CS435
** Author: Ben Fridkis
** Date: 2/28/2024
** Description: Distance vector (more specifically, Path Vector) routing protocol
**              simulation for CS435 MP2.
***********************************************************************************/

/*
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <fstream>
#include <vector>
#include <algorithm>
#include <locale>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
*/

#include "converge.hpp"

std::string vecToString(std::vector<int>& vec) {
    if (vec.size() > 0) { 
        std::stringstream pathSS;
        std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(pathSS, ""));
        return pathSS.str();
    }
    else { 
        std::string retStr = std::string("-");
        return retStr; }
}

//Tie breaker function. Next hop lowest node number wins the tie. Returns 1 if pathA is winner, 0 if pathB is winner, and -1 if paths are identical. Will not handle paths that are identical for the entirety of one path but of dissimilar lenghts. (However, these should never be passed as they should never have the same cost.)
int tieBreaker(std::vector<int> pathA, std::vector<int> pathB) {
	std::vector<int>::iterator aIt, bIt;
	for(aIt = pathA.begin(), bIt = pathB.begin(); aIt != pathA.end() && bIt != pathB.end(); aIt++, bIt++) {
			//std::cout << "new path node: " << *aIt << " existing path node: " << *bIt << std::endl;
		if (*aIt < *bIt) {
			std::cout << "Did we get to npIt < epIt...??" << std::endl;
			return 1;
		}
		else if (*aIt > *bIt) {
			return 0;
		}
	}
	return -1;
}

//For testing/debugging only
void consoleOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT) {
    //Print converged FT (testing/troubleshooting only)
    std::cout << std::endl;

    for (unsigned short i = 1; i < _FT.size(); i++) {
        //std::cout << "i: " << i << " " << _FT.size() << std::endl;
        for (std::map<int, std::multimap<int, std::vector<int>>>::iterator it=_FT[i].begin(); it!=_FT[i].end(); it++) {
            //std::cout << "i: " << i << " " << _FT.size() << std::endl;
            for (std::multimap<int, std::vector<int>>::iterator itMM=it->second.begin() ; itMM != it->second.end() ; itMM++) {
                //std::vector<int> tmpVec();
                std::stringstream path;
                std::copy(itMM->second.begin(), itMM->second.end(), std::ostream_iterator<int>(path, ""));
                //std::string path(itMM->second.begin(), itMM->second.end());
                std::string pathToPrint;
                if (itMM->second.size() > 0) { 
                    std::stringstream pathSS;
                    std::copy(itMM->second.begin(), itMM->second.end(), std::ostream_iterator<int>(pathSS, ""));
                    std::string pathString = pathSS.str();
                    std::cout << "source: " << i << " Destination: " << it->first << " Cost: " << itMM->first << " Path: " << pathString << std::endl;
                }
                else { 
                    std::cout << "source: " << i << " Destination: " << it->first << " Cost: " << itMM->first << " Path: " << "-" << std::endl;
                }
                //std::cout << "source: " << i << " Destination: " << it->first << " Cost: " << itMM->first << " Path: " << pathToPrint << std::endl;
                //std::cout << "source: " << i << " Destination: " << it->first << " Cost: " << itMM->first << " First Hop: " << itMM->second.size() << std::endl;
                //std::cout << "-------------------------------------" << std::endl;
            }
        }
    }
}

void processChanges(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::stringstream& _changesInput) {
    int changedLinkNode1, changedLinkNode2, change, j=1;
    while (_changesInput >> changedLinkNode1 >> changedLinkNode2 >> change) {    
        std::cout << "\nChanges to be applied: changedLinkNode1 - " << changedLinkNode1 << " changedLinkNode2 - " << changedLinkNode2 << " change - " << change << std::endl;
        std::cout << std::endl;
        if(_FT.size() > 1) {
            for(int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
                //for(auto&& [destNode, destNodePaths] : _FT[i]) {
                for (auto it_m = _FT[sourceNode].begin(), next_it = it_m; it_m != _FT[sourceNode].end(); it_m = next_it) {
                    next_it++;
                    int destNode = it_m->first;
                    std::multimap<int, std::vector<int>> destNodePaths = it_m->second;
                    for(auto&& [cost, path] : destNodePaths) {
                        //If this is the direct link affected by the change, update accordingly
                        if(path.size() == 1 && (sourceNode == changedLinkNode1 && changedLinkNode2 == path[0] || sourceNode == changedLinkNode2 && changedLinkNode1 == path[0])) {
                            std::cout << "Current Path: " << vecToString(path) << std::endl;
                            std::cout << "Erasing Map Entry..." << " sourceNode: " << sourceNode << " changedLinkNode1: " << changedLinkNode1 << " destNode: " << destNode << " changedLinkNode2: " << changedLinkNode2 << std::endl;
                            //If link is broken and this is the only path from source to destination, remove destNode map from source node's forwaring table (destination no longer reachable)
                            if(change == -999 && destNodePaths.size() == 1) {
                              _FT[sourceNode].erase(_FT[sourceNode].find(destNode));
                            }
                            //If the link is broken but there is another path to the destintation node, remove only the one multimap entry for the direct link
                            else if(change == -999 && destNodePaths.size() > 1) {
                              _FT[sourceNode].find(destNode)->second.erase(_FT[sourceNode].find(destNode)->second.find(cost));
                            }
                            //If the link is not broken but cost is changing, update cost accordingly.
                            else {
                              //Remove existing multimap entry for direct link
                              _FT[sourceNode].find(destNode)->second.erase(_FT[sourceNode].find(destNode)->second.find(cost));
                              //Add new entry representing new cost for link. 
							  
							  std::vector<int> newLinkVec = std::vector<int>(1,destNode);
							  
							  //If the new cost introduces a tie with another path, handle according to tie breaker rule (lowest next node number in path)
							  if(destNodePaths.find(change) != destNodePaths.end()) {
								int existingNonDirectLinkPathIsHighestPriority = tieBreaker(destNodePaths.find(change)->second, newLinkVec);
								if(existingNonDirectLinkPathIsHighestPriority == 1) {
									//Save current entry in temp pair...
									std::pair tmpMMEntry = std::make_pair(change, destNodePaths.find(change)->second);
									//Erase current entry...
									_FT[sourceNode].find(destNode)->second.erase(_FT[sourceNode].find(destNode)->second.find(change));
									//Add new direct link with updated cost..
									_FT[sourceNode].find(destNode)->second.insert(std::make_pair(change, std::vector<int>(1,destNode)));
									//Add back existing entry back in so it gets loaded into the multimap for destNode at this cost (==change) top priority
									_FT[sourceNode].find(destNode)->second.insert(tmpMMEntry);
								}
								else {
									//If updated/changed direct link cost will have the higher priority, add straight in so it will have the first/highest priority in the multimap
									_FT[sourceNode].find(destNode)->second.insert(std::make_pair(change, std::vector<int>(1,destNode)));
								}
							  }
							  else {
								_FT[sourceNode].find(destNode)->second.insert(std::make_pair(change, std::vector<int>(1,destNode)));
							  }
                            }
                        }
                        //Else, if any other given path contains the source and destination link, remove this path (if the link is still viable but cost changed, the new path cost will be calculated/assessed during next converge call).
                        else {
                            std::cout << "Current Path: " << vecToString(path) << std::endl;
                            //Function lambda used for std::adjacent_find
                            auto findLink = [changedLinkNode1, changedLinkNode2, sourceNode](int nodeA, int nodeB) { 
                                //std::cout << "In Lambda... sourceNode: " << sourceNode << " nodeA: " << nodeA << " changedLinkNode1: " << changedLinkNode1 << " nodeB: " << nodeB << " changedLinkNode2: " << changedLinkNode2 << std::endl;
                                //std::cout << "Lambda Return Value : " << ((nodeA == changedLinkNode1 && nodeB == changedLinkNode2) || (nodeA == changedLinkNode2 && nodeB == changedLinkNode1)) << std::endl;
                                return (sourceNode == changedLinkNode1 && nodeA == changedLinkNode2) || (sourceNode == changedLinkNode2 && nodeA == changedLinkNode1) ||
									   (nodeA == changedLinkNode1 && nodeB == changedLinkNode2) || (nodeA == changedLinkNode2 && nodeB == changedLinkNode1); 
                            };
                            std::vector<int>::iterator it_findLink = std::adjacent_find(path.begin(), path.end(), findLink);
                            //std::cout << "it_findLink == path.end()?: " << (it_findLink == path.end()) << " it_findLink: " << *it_findLink << std::endl;
                            //std::cout << "it_findLink == path.end()?: " << (it_findLink == path.end()) << " it_findLink != path.end()?: " << (it_findLink != path.end()) << std::endl;
                            //If this is the only path to Node2 (for source Node 1) or Node 1 (for source Node 2), remove the map entry for previously linked node (will be added back via subsequent call to converge if link still exists).
                            if(it_findLink != path.end() && _FT[sourceNode].find(destNode)->second.size() == 1) {
                                std::cout << "Erasing map entry here for node " << sourceNode << std::endl;
                                _FT[sourceNode].erase(_FT[sourceNode].find(destNode));
                            }
                            //Else if multiple paths exist for linked node, just remove single path entry
                            //else if(it_findLink != path.end() && destNodePaths.size() > 1) {
                            else if(it_findLink != path.end() && _FT[sourceNode].find(destNode)->second.size() > 1) {
                                std::cout << "destNodesPaths.size(): " << _FT[sourceNode].find(destNode)->second.size() << std::endl;
                                std::cout << "Erasing multi-map entry here for node " << sourceNode << std::endl;
                                _FT[sourceNode].find(destNode)->second.erase(_FT[sourceNode].find(destNode)->second.find(cost));
                            }
                        }
                    }
                }
            }
        }
        std::cout << std::endl;
        std::cout << "Forwarding Table After Change " << j << " Applied, Before Reconverge..." << std::endl;
        consoleOutFT(_FT);
        std::cout << std::endl;
        
        //Reconverge for each node after changes applied...
        for (int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
            std::cout << "converging for node: << " << sourceNode << " (after change " << j << ")" << std::endl;
            converge(sourceNode, -1, -1, _FT);
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
        std::cout << std::endl;
        std::cout << "Forwarding tables after change " << j++ << " and subsequent reconvergence applied..." << std::endl;
        consoleOutFT(_FT);
    }
}

int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    //if (argc != 4) {
    //    printf("Usage: ./distvec topofile messagefile changesfile\n");
    //    return -1;
    //}
    
    std::cout << "Beginning of Program..." << std::endl;
    
    //Forwarding tables stored as  vector of maps. Each map represents the forwarding table for a given node. Map key is remote node, map value is a multimap with keys representing costs (stored as int), and values representing paths (stored as std::vector<int>).
    ////Multimap used so multiple routes can be stored for a given destination, for reconvergence upon link failure.
    std::vector<std::map<int, std::multimap<int, std::vector<int>>>> FT; 
    
    //Build initial forwarding table from topology input file
    //std::ifstream topoInput(argv[1]);
    
    //For testing only...
    std::stringstream topoInput;
    //topoInput << 1 << ' ' << 2 << ' ' << 6 << std::endl;
    topoInput << 1 << ' ' << 2 << ' ' << 8 << std::endl;
    topoInput << 2 << ' ' << 3 << ' ' << 3 << std::endl;
    topoInput << 2 << ' ' << 5 << ' ' << 4 << std::endl;
    topoInput << 4 << ' ' << 1 << ' ' << 1 << std::endl;
    topoInput << 4 << ' ' << 5 << ' ' << 1 << std::endl;
    
    //std::cout << "Line53..." << std::endl;
    
    //int variables to hold topology interger values
    int a, b, c;
    //Temporary multimap used for initial multimap constructors
    std::multimap<int, std::vector<int>> tempMM;
    //iterator to traverse and access entries of each's nodes forwarding table
    std::map<int, std::multimap<int, std::vector<int>>>::iterator it;
    //iterator to traverse and access elements of multimaps (forwarding tables / routes for a given destination)
    std::multimap<int, std::vector<int>>::iterator itMM;
    
    //std::cout << "Line64..." << std::endl;
    //int i = 0;
    while (topoInput >> a >> b >> c) {
        //std::cout << "i: " << ++i << std::endl;
        //std::cout << "Line68..." << "a: " << a << " b: " << b << " c: " << c << std::endl;
        
        //Add additional entries up to largest number node found in topology.
        ////Padding by one in the front so index number = node number for semantic convenience. :)
        if (a >= FT.size()) { FT.resize(a+1); }
        if (b >= FT.size()) { FT.resize(b+1); }
        
        //std::cout << "Line75..." << "FT Size: " << FT.size() << std::endl;
        
        //Load forwarding tables according to imported topology
        
        ////For a's map...
        it = FT[a].find(b);
        //std::cout << "Line81..." << std::endl;
        if (it == FT[a].end()) {
            tempMM.insert(std::make_pair(c, std::vector<int>(1,b)));
            FT[a].insert(std::make_pair(b, std::multimap<int, std::vector<int>>(tempMM)));
            tempMM.clear();
            //std::cout << "Line86..." << std::endl;
            //std::cout << FT[a].find(b)->first << " " << FT[a].find(b)->second. std::endl;
            //std::cout << "Destination Node: " << b << " Cost: " << FT[a].find(b)->second.begin()->first << " Path: " << FT[a].find(b)->second.begin()->second[0] << std::endl;
        }
        else {
            it->second.insert(std::make_pair(c, std::vector<int>(1,b)));
        }
        ////For b's map...
        //std::cout << "Line92..." << std::endl;
        it = FT[b].find(a);
        //std::cout << "Line94..." << std::endl;
        if (it == FT[b].end()) {
            tempMM.insert(std::make_pair(c, std::vector<int>(1,a)));
            FT[b].insert(std::make_pair(a, std::multimap<int, std::vector<int>>(tempMM)));
            tempMM.clear();
            //std::cout << "Line97..." << std::endl;
            std::cout << "Destination Node: " << a << " Cost: " << FT[b].find(a)->second.begin()->first << " Path: " << FT[b].find(a)->second.begin()->second[0] << std::endl;
        }
        else {
            it->second.insert(std::make_pair(c, std::vector<int>(1,a)));
        }
        
    }
    
    //std::cout << "Line100..." << std::endl;
    
    //Initialize fowarding tables with self-path cost = 0 and paths not yet discovered with placeholder value of -1.
    for (int i = 1; i < FT.size(); i++) {
        
        //Add path to self with cost of 0
        tempMM.insert(std::make_pair(0, std::vector<int>(0)));
        FT[i].insert(std::make_pair(i, std::multimap<int, std::vector<int>>(tempMM)));
        tempMM.clear();
        
        //std::cout << "Source Node: " << i << " Destination Node: " << FT[i].find(i)->first << " Cost: " << FT[i].find(i)->second.begin()->first << " Path: " << FT[i].find(i)->second.begin()->second.size() << std::endl;
        
        //For all other unregistered paths, establish an entry with a placeholder value of -1
        //for(unsigned short j = 1; j < FT.size(), j++) {
            //std::map<int, int>::iterator it = ft.find(j);
        //    if (ft.find(j) == ft.end()) { ft.insert(std::pair<int, int>(j,-1); }
            
        //}
    }
    
    //Print initial loaded FT (testing/troubleshooting only)
    std::cout << std::endl;
    std::cout << "Intial FT" << std::endl;
    std::cout << "---------" << std::endl;
    for (unsigned short i = 1; i < FT.size(); i++) {
        //std::cout << "i: " << i << " " << FT.size() << std::endl;
        for (it=FT[i].begin(); it!=FT[i].end(); it++) {
            //std::cout << "i: " << i << " " << FT.size() << std::endl;
            for (itMM=it->second.begin() ; itMM != it->second.end() ; itMM++) {
                std::string tmpPath = itMM->second.size() ? std::to_string(itMM->second[0]) : "-";
                std::cout << i << it->first << itMM->first << tmpPath << std::endl;
                
            }
        }
    }
    std::cout << std::endl;
    
    //Initial Routing Table Convergences
    for (int i = 1; i < FT.size(); i++) {
        std::cout << std::endl;
        std::cout << "converging for node: " << i << std::endl;
        converge(i, -1, -1, FT);
        //converge(i, -1, -1, FT, nullptr);
        //converge(i, -1, FT);
    }
    
    //Print converged FT (testing/troubleshooting only)
    std::cout << std::endl;
    std::cout << "Forwarding tables after initial converge..." << std::endl;
    consoleOutFT(FT);
    
    /*std::cout << "\n2nd Convergence..." << std::endl;
    for (int i = 1; i < FT.size(); i++) {
        std::cout << std::endl;
        std::cout << "converging for node: " << i << std::endl;
        converge(i, -1, -1, FT);
        //converge(i, -1, -1, FT, nullptr);
        //converge(i, -1, FT);
    }
    
    //Print converged FT (testing/troubleshooting only)
    std::cout << std::endl;
    std::cout << "Forwarding tables after 2nd converge (but before any changes applied)..." << std::endl;
    consoleOutFT(FT);
    */
    std::cout << std::endl;
    
    auto iss = std::istringstream{"2 1 here is a message from 2 to 1\n3 5 this one gets sent from 3 to 5!"};
    messagePrint(FT, iss);
    
    std::cout << std::endl;
    
    //Changes simulation...
    std::stringstream changeInput;
    //changeInput << 2 << ' ' << 4 << ' ' << 1 << std::endl;
    //changeInput << 2 << ' ' << 4 << ' ' << -999 << std::endl;
    changeInput << 2 << ' ' << 3 << ' ' << 1 << std::endl;
    changeInput << 2 << ' ' << 3 << ' ' << -999 << std::endl;
    //int i = 0;
    //while(changeInput >> a >> b >> c) {
    //    std::cout << "After change " << i << ":" << std::endl;
    //    std::cout << "a: " << a << " b: " << b << " c: " << c << std::endl;
     //   i++;
    //}
    
    processChanges(FT, changeInput);
    
    std::cout << std::endl;
    
    std::cout << "End of Program..." << std::endl;
        
    //Initialize fowarding tables with paths not yet discovered with placeholder value (in this case -1)
    

    //FILE *fpOut;
    //fpOut = fopen("output.txt", "w");
    //fclose(fpOut);
    //std::ofstream fpOut("output.txt");
    
    
    
    
    
    
    
    //fpOut.close();


    // You may choose to use std::fstream instead
    // std::ofstream fpOut("output.txt");

    return 0;
}

// References
//https://stackoverflow.com/questions/7868936/read-file-line-by-line-using-ifstream-in-c
//https://stackoverflow.com/questions/23637765/does-c-provide-a-triple-template-comparable-to-pairt1-t2
//https://cplusplus.com/reference/map/map/
//https://stackoverflow.com/questions/20899775/memory-allocation-and-working-of-stdmap-in-c#:~:text=A%20map%20is%20an%20object,difference%20with%20std%3A%3Amultimap%20).
//https://stackoverflow.com/questions/1939953/how-to-find-if-a-given-key-exists-in-a-stdmap
//https://cplusplus.com/reference/map/map/find/
//https://stackoverflow.com/questions/15027282/c-for-each-pulling-from-vector-elements
//https://stackoverflow.com/questions/15149354/get-index-of-current-element-in-c-range-based-for-loop
//https://cplusplus.com/reference/utility/pair/pair/
//https://stackoverflow.com/questions/5590381/how-to-convert-int-to-string-in-c
//https://cplusplus.com/reference/utility/pair/tuple_element/
//https://cplusplus.com/reference/map/multimap/multimap/
//https://cplusplus.com/reference/string/to_string/
//https://cplusplus.com/reference/sstream/stringstream/stringstream/
//https://en.cppreference.com/w/cpp/container/multimap/multimap
//https://stackoverflow.com/questions/39906504/how-to-count-duplicates-in-a-vector-c
//https://www.geeksforgeeks.org/std-count-cpp-stl/
//https://stackoverflow.com/questions/3450860/check-if-a-stdvector-contains-a-certain-object
//https://stackoverflow.com/questions/2518979/how-to-transform-a-vectorint-into-a-string
//https://stackoverflow.com/questions/571394/how-to-find-out-if-an-item-is-present-in-a-stdvector
//https://stackoverflow.com/questions/8046357/how-do-i-check-if-a-stringstream-variable-is-empty-null
//https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
//https://stackoverflow.com/questions/16867372/splitting-strings-by-newline-character-in-c
//https://stackoverflow.com/questions/4324763/can-we-have-functions-inside-functions-in-c
//https://stackoverflow.com/questions/7407383/cant-send-null-to-a-function-getting-vector
//https://stackoverflow.com/questions/8660025/why-doesnt-c-require-a-new-statement-to-initialize-stdvector
//https://stackoverflow.com/questions/49441588/c-how-to-check-if-contents-of-vector-exist-in-another-vector
//https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
//https://cplusplus.com/reference/algorithm/adjacent_find/#google_vignette