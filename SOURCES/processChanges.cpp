/*******************************************************
** Program name: processChanges.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for processChanges function.
********************************************************/

#include "../HEADERS/processChanges.hpp"

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
                        if(path.size() == 1 && ((sourceNode == changedLinkNode1 && changedLinkNode2 == path[0]) || (sourceNode == changedLinkNode2 && changedLinkNode1 == path[0]))) {
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
                                return ((sourceNode == changedLinkNode1 && nodeA == changedLinkNode2) || (sourceNode == changedLinkNode2 && nodeA == changedLinkNode1) ||
									   (nodeA == changedLinkNode1 && nodeB == changedLinkNode2) || (nodeA == changedLinkNode2 && nodeB == changedLinkNode1)); 
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
        
        //Reconverge for each node after changes applied (Run twice to ensure all lowest paths are found.)
        for (int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
            std::cout << "converging for node: << " << sourceNode << " (after change " << j << ")" << std::endl;
            converge(sourceNode, -1, -1, _FT);
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
		for (int sourceNode = _FT.size()-1; sourceNode > 0; sourceNode--) {
            std::cout << "converging for node: << " << sourceNode << " (after change " << j << ")" << std::endl;
            converge(sourceNode, -1, -1, _FT);
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
        std::cout << std::endl;
        std::cout << "Forwarding tables after change " << j++ << " and subsequent reconvergence applied..." << std::endl;
        consoleOutFT(_FT);
    }
}

void processChanges(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ifstream& _changesInput, std::ifstream& _messagesInput, std::ofstream& _outFile) {

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
                        if(path.size() == 1 && ((sourceNode == changedLinkNode1 && changedLinkNode2 == path[0]) || (sourceNode == changedLinkNode2 && changedLinkNode1 == path[0]))) {
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
                                return ((sourceNode == changedLinkNode1 && nodeA == changedLinkNode2) || (sourceNode == changedLinkNode2 && nodeA == changedLinkNode1) ||
									   (nodeA == changedLinkNode1 && nodeB == changedLinkNode2) || (nodeA == changedLinkNode2 && nodeB == changedLinkNode1)); 
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
                                //In case of a tie, must loop through paths and find the match (so as to not inadvertently delete the wrong entry/path 
								for(std::multimap<int, std::vector<int>>::iterator pathAtCost = _FT[sourceNode].find(destNode)->second.find(cost); pathAtCost != _FT[sourceNode].find(destNode)->second.end(); pathAtCost++) {
									if(pathAtCost->second == path) {
										_FT[sourceNode].find(destNode)->second.erase(pathAtCost);
										break;
									}
								}
								//This won't work in some cases for tie breakers, as it may delete the wrong entry...
								//_FT[sourceNode].find(destNode)->second.erase(_FT[sourceNode].find(destNode)->second.find(cost));
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
        
        //Reconverge for each node after changes applied (Run twice to ensure all lowest paths are found.)
        for (int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
            std::cout << "reconverging for node: " << sourceNode << " (after change " << j << ", first re-convergence)" << std::endl;
            converge(sourceNode, -1, -1, _FT);
			std::cout << std::endl;
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
		for (int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
            std::cout << "reconverging for node: " << sourceNode << " (after change " << j << ", second reconvergence)" << std::endl;
            converge(sourceNode, -1, -1, _FT);
			std::cout << std::endl;
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
		/*for (int sourceNode = _FT.size()-1; sourceNode > 0; sourceNode--) {
            std::cout << "reconverging for node: << " << sourceNode << " (after change " << j << ", second reconvergence)" << std::endl;
            converge(sourceNode, -1, -1, _FT);
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }*/
        std::cout << std::endl;
        std::cout << "Forwarding tables after change " << j++ << " and subsequent reconvergence applied..." << std::endl;
        consoleOutFT(_FT);
		fileOutFT(_FT, _outFile, false);
		
		messagePrint(_FT, _messagesInput, _outFile);
    }
}