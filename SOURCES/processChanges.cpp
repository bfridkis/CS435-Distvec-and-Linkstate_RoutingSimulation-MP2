/*******************************************************
** Program name: processChanges.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for processChanges function.
********************************************************/

#include "../HEADERS/processChanges.hpp"

//For Dist Vec
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
            converge(_FT, sourceNode, sourceNode, nullptr, 0);
			//converge(sourceNode, -1, -1, _FT);
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
		/*for (int sourceNode = _FT.size()-1; sourceNode > 0; sourceNode--) {
            std::cout << "converging for node: << " << sourceNode << " (after change " << j << ")" << std::endl;
            converge(sourceNode, -1, -1, _FT);
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }*/
        std::cout << std::endl;
        std::cout << "Forwarding tables after change " << j++ << " and subsequent reconvergence applied..." << std::endl;
        consoleOutFT(_FT);
    }
}

//For Dist Vec
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
            converge(_FT, sourceNode, sourceNode, nullptr, 0);
			//converge(sourceNode, -1, -1, _FT);
			std::cout << std::endl;
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }
		/*for (int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
            std::cout << "reconverging for node: " << sourceNode << " (after change " << j << ", second reconvergence)" << std::endl;
            converge(sourceNode, -1, -1, _FT);
			std::cout << std::endl;
            //converge(sourceNode, -1, -1, _FT, nullptr);
        }*/
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

//For Link State
void processChanges(std::vector<std::map<int, std::pair<int, int>>> &_FT, std::vector<std::map<int, std::pair<int, int>>> &_FT_invert, std::vector<std::map<int, int>> &_TT,  std::ifstream& _changesInput, std::ifstream& _messagesInput, std::ofstream& _outFile) {

    int changedLinkNode1, changedLinkNode2, change, j=1;
    while (_changesInput >> changedLinkNode1 >> changedLinkNode2 >> change) {    
        std::cout << "\nChanges to be applied: changedLinkNode1 - " << changedLinkNode1 << " changedLinkNode2 - " << changedLinkNode2 << " change - " << change << std::endl;
        std::cout << std::endl;
		
		if(changedLinkNode1 > _FT.size() - 1) {
			int prevSize = _TT.size();
			_TT.resize(changedLinkNode1+1);
			_FT.resize(changedLinkNode1+1);
			_FT_invert.resize(changedLinkNode1+1);
			//Resize forwarding table and initialize added node with self link cost of 0 (and if any nodes are skipped in between previous highest node number and newest node, initialize those node number placeholders with cost of -1, to denote not part of topology)
			for(int i = prevSize ; i < _FT.size(); i++) {
				if(i == changedLinkNode1) {
					_FT[i].insert(std::make_pair(i, std::make_pair(i,0)));
					_FT_invert[i].insert(std::make_pair(i, std::make_pair(i,0)));
				}
				else {
					_FT[i].insert(std::make_pair(i, std::make_pair(i,-1)));
					_FT[i]_invert.insert(std::make_pair(i, std::make_pair(i,-1)));
				}
			}
		}
		if(changedLinkNode2 > _FT.size() - 1) {
			int prevSize = _TT.size();
			_TT.resize(changedLinkNode2+1);
			_FT.resize(changedLinkNode2+1);
			_FT_invert.resize(changedLinkNode2+1);
			//Resize forwarding table and initialize added node with self link cost of 0 (and if any nodes are skipped in between previous highest node number and newest node, initialize those node number placeholders with cost of -1, to denote not part of topology)
			for(int i = prevSize ; i < _FT.size(); i++) {
				if(i == changedLinkNode2) {
					_FT[i].insert(std::make_pair(i, std::make_pair(i,0)));
					_FT[i]_invert.insert(std::make_pair(i, std::make_pair(i,0)));
				}
				else {
					_FT[i].insert(std::make_pair(i, std::make_pair(i,-1)));
					_FT_invert[i].insert(std::make_pair(i, std::make_pair(i,-1)));
				}
			}
		}
		
		//Update topology table with link cost change
		//If link is to be removed, erase it from topology
		if(change == -999) {
			if(_TT[changedLinkNode1].find(changedLinkNode2) != _TT[changedLinkNode1].end()) {
				_TT[changedLinkNode1].erase(_TT[changedLinkNode1].find(changedLinkNode2));
			}
			if(_TT[changedLinkNode2].find(changedLinkNode1) != _TT[changedLinkNode2].end()) {
				_TT[changedLinkNode2].erase(_TT[changedLinkNode2].find(changedLinkNode1));
			}
		}
		else {
			//If link is already present, update it's cost
			if(_TT[changedLinkNode1].find(changedLinkNode2) != _TT[changedLinkNode1].end()) {
				_TT[changedLinkNode1].find(changedLinkNode2)->second = change;
			}
			//If link is not yet present, add it
			else {
				_TT[changedLinkNode1].insert(std::make_pair(changedLinkNode2, change));
			}
			//If link is already present, update it's cost
			//std::cout << "_TT.size() : " << _TT.size() << std::endl;
			if(_TT[changedLinkNode2].find(changedLinkNode1) != _TT[changedLinkNode2].end()) {
				_TT[changedLinkNode2].find(changedLinkNode1)->second = change;
			}
			//If link is not yet present, add it
			else {
				_TT[changedLinkNode2].insert(std::make_pair(changedLinkNode1, change));
			}
		}
		
		//Print topology table after changes
		std::cout << std::endl;
		std::cout << "New Topology after change " << j << std::endl;
		std::cout << "---------" << std::endl;
		for (unsigned short i = 1; i < _TT.size(); i++) {
			//std::cout << "i: " << i << " " << FT.size() << std::endl;
			for (std::map<int, int>::iterator it=_TT[i].begin(); it!=_TT[i].end(); it++) {
				std::cout << i << it->first << it->second << std::endl;
			}
		}
		std::cout << std::endl;
		
		//Clear forwarding tables except for self entries
		for(int sourceNode = 1; sourceNode < _FT_invert.size(); sourceNode++) {
			//for(auto&& [reachableNode, nextHop_cost] : _FT[sourceNode]) {
			for(auto it_m = _FT_invert[sourceNode].begin(), next_it=it_m; it_m != _FT_invert[sourceNode].end(); it_m = next_it){
				next_it++;
				int reachableNode = it_m->first;
				if(reachableNode != sourceNode) {
					_FT_invert[sourceNode].erase(_FT_invert[sourceNode].find(reachableNode));
				}
			}
		}
		for(int sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
			//for(auto&& [reachableNode, nextHop_cost] : _FT[sourceNode]) {
			for(auto it_m = _FT[sourceNode].begin(), next_it=it_m; it_m != _FT[sourceNode].end(); it_m = next_it){
				next_it++;
				int reachableNode = it_m->first;
				if(reachableNode != sourceNode) {
					_FT[sourceNode].erase(_FT[sourceNode].find(reachableNode));
				}
			}
		}
		
		//Should be empty except for self-entries here...
		std::cout << "Stripped FT before recovergence... " << std::endl;
		consoleOutFT(_FT);
		
		//Reconverge
		for(int sourceNode = 1; sourceNode < _TT.size(); sourceNode++) {
			if(_FT[sourceNode].find(sourceNode)->second.second != -1) {
				converge(sourceNode, _TT, _FT_invert);
			}
		}
		
		//After before after the initial converge (linkstate.cpp), need to swap routes between each source and destination to ensure tie breaking rule of lowest last hop node number is followed (due to the fact that dijkstra's algo builds the routes in reverse order, i.e. from destination to source... see converge.cpp)
		//Do this by building a new FT with swapped values from the dijkstra inverted output
		//(If this makes program run too slow, for this assignment/exercise just need to swap the final source / destination next hops for consoleOutFT and print dest path as source's and vis versa in messagePrint...)
		for (int sourceNode = 1; sourceNode < FT_.size(); sourceNode++) {
			for(auto&& [reachableNode, nextHop_cost_invert] : FT_invert[sourceNode]) {
				if(FT[reachableNode].find(sourceNode) == FT[reachableNode].end()) {
					FT[reachableNode].insert(sourceNode, std::make_pair(nextHop_cost_invert.first, nextHop_cost_invert.second));
				}
				else {
					FT[reachableNode].find(sourceNode)->second.first = nextHop_cost_invert.first;
					FT[reachableNode].find(sourceNode)->second.second = nextHop_cost_invert.second;
				}
			}
		}
		
        //std::cout << std::endl;
        //std::cout << "Forwarding Table After Change " << j << " Applied, Before Reconverge..." << std::endl;
        //consoleOutFT(_FT);
		
        std::cout << std::endl;
        
        std::cout << "Forwarding tables after change " << j++ << " and subsequent reconvergence applied..." << std::endl;
        consoleOutFT(_FT);
		fileOutFT(_FT, _outFile, false);
		
		messagePrint(_FT, _messagesInput, _outFile);
    }
}