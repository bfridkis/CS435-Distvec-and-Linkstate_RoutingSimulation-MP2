/*****************************************************************
** Program name: converge.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for converge function. Builds routing
**              tables by simulating a path vector protocol
**              (messages circulated between nodes conveying route
**              paths and associated costs).
******************************************************************/

#include "../HEADERS/converge.hpp"

//For Dist Vec (Attempt 1)
void converge(int sourceNode, int neighbor, int prevDestNode, std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT) {      
//void converge(int sourceNode, int neighbor, int prevDestNode, std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::vector<int>* nodesAddedOnCurrentPath) {
//void converge(int sourceNode, int neighbor, std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT) {  
    //temporary multimap used for constructors / inserting new forwarding table entries
    std::multimap<int, std::vector<int>> tmpMM;
    
    //If neighbor == -1, this is the top/first level of the recursive call, so traverse the source node's own forwarding table to start. Else, traverse node given by neighbor (for subsequent / lower-level recursive calls)
    int nodeToSearch;
    if (neighbor == -1) { nodeToSearch = sourceNode; }
    else { nodeToSearch = neighbor; }
    
    //Vector to track nodes added on the current path, so they will not be re-searched after the fact 
    //std::vector<int> tmpVec = std::vector<int>();
    //nodesAddedOnCurrentPath = &tmpVec;
    //nodesAddedOnCurrentPath = new std::vector<int>();
    //std::vector<int> nodesAddedOnCurrentPath = std::vector<int>();
    
    //Traverse through each reachable remote node's forwarding table, and add all nodes reachable via neighbors using lowest cost option (simulates route advertisment messages which advertise lowest cost routes to neighbors)
    //NOTE: IDEALLY, SHOULD HAVE COPIED EXISTING FORWARDING TABLE INTO A TEMP OBJECT, THEN LOOPED THROUGH IT, ONLY MODIFYING/ADDING TO THE ORIGINAL. AS WRITTEN, WHEN YOU MODIFY THE SAME LOOP USED DURING 'FOR EACH' ITERATIONS, YOU MAY THEN VISIT ADDED ELEMENTS WHICH IS NOT DESIRED HERE. DELETIONS AREN'T USED HERE BUT IN THE CASE OF DELETING FROM THE SAME LOOP BEING ITERATED THROUGH, MEMORY ACCESS ISSUES OCCUR AS YOU TRY TO ACCESS REMOVED ELEMENTS/ENTRIES.
    for(auto&& [destNode, destNodePaths] : _FT[nodeToSearch]) {
        
        //Vector to track nodes added on the current path, so they will not be re-searched after the fact 
        //std::vector<int> tmpVec = std::vector<int>();
        //nodesAddedOnCurrentPath = &tmpVec;
        //nodesAddedOnCurrentPath = new std::vector<int>();
        //std::vector<int> nodesAddedOnCurrentPath = std::vector<int>();
        
        //Ignore entries back to sourceNode and self entry
        //if(destNode != sourceNode && destNode != nodeToSearch && std::find(nodesAddedOnCurrentPath.begin(), nodesAddedOnCurrentPath.end(), destNode) == nodesAddedOnCurrentPath.end()) {
        if(destNode != sourceNode && destNode != nodeToSearch) {
            std::cout << "Starting converge, line 43... Node to search: " << nodeToSearch << " destNode: " << destNode <<  std::endl;
			int totalPathCost;
            //For any n-level of recursive call greater than 1, need to add entry for destNode here if a route isn't already available, before adding entries for destNode remote/reachable node's below (don't need to for first level of recursion because direct connected nodes already have entries established via intial topology load)
            if (prevDestNode != -1 && _FT[sourceNode].find(destNode) == _FT[sourceNode].end()) {
                //Search through potential paths and find cheapest that won't introduce a loop when path to this hop is added to path from this hop to destNode
                bool pathToDestNodeAdded = false;
				for(auto&&[cost, destNodePath] : _FT[nodeToSearch].find(destNode)->second) {
                    bool loopFound = false;
                    std::vector<int> existingPathToThisHop(_FT[sourceNode].find(nodeToSearch)->second.begin()->second);
                    for (auto node: destNodePath) {
                        if (std::find(existingPathToThisHop.begin(), existingPathToThisHop.end(), node) != existingPathToThisHop.end()) {
                            loopFound = true;
                            //break;
                        }
                    }
                    if(!loopFound) {
                        int costToThisHop = _FT[sourceNode].find(nodeToSearch)->second.begin()->first;
                        totalPathCost = costToThisHop + _FT[nodeToSearch].find(destNode)->second.begin()->first;
                        //Initialize newPath vector with existing path to this hop
                        std::vector<int> newPath(_FT[sourceNode].find(nodeToSearch)->second.begin()->second);
                        //Then add path from this hop to next hop
                        newPath.insert(newPath.end(), _FT[nodeToSearch].find(destNode)->second.begin()->second.begin(), _FT[nodeToSearch].find(destNode)->second.begin()->second.end());
                        //Use temporary multimap to construct final map entry for newly reachable node
                        tmpMM.insert(std::make_pair(totalPathCost, newPath));
                        
                        //bool canFindNewlyAddedNodeInSourceNodesFT = _FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end();
                        //std::cout << "Can find new Destination Node After Adding Node " << nextHopRemoteNode << " now...? " << canFindNewlyAddedNodeInSourceNodesFT << std::endl;
                        _FT[sourceNode].insert(std::make_pair(destNode, std::multimap<int, std::vector<int>>(tmpMM)));
                        
                        //canFindNewlyAddedNodeInSourceNodesFT = _FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end();
                        //std::cout << "Can find new Destination Node After Adding Node " << nextHopRemoteNode << " now...? " << canFindNewlyAddedNodeInSourceNodesFT << std::endl;
                        
                        //nodesAddedOnCurrentPath.push_back(destNode);
                        std::cout << "New next node added! Source Node: " << sourceNode << " Destination Node: " << destNode << " Cost: " << totalPathCost << " Path: " << vecToString(newPath) << std::endl;
                        tmpMM.clear();
                        pathToDestNodeAdded = true;
						break;
                    }
                    //If we get here, it means we can't get to the destination node through the node we are currently searching without a loop, which also means that later on in the recursive call we will find a path to destNode. So, we can skip this iteration of the outer loop.
                    //else {
                        //Initialize newPath vector with existing path to this hop
                        //std::vector<int> newPath(_FT[sourceNode].find(nodeToSearch)->second.begin()->second);
                        //Then add path from this hop to next hop
                        //newPath.insert(newPath.end(), _FT[nodeToSearch].find(destNode)->second.begin()->second.begin(), _FT[nodeToSearch].find(destNode)->second.begin()->second.end());
                        //Use temporary multimap to construct final map entry for newly reachable node
                        //tmpMM.insert(std::make_pair(totalPathCost, newPath));
                        
                        
                        std::cout << "No paths without loops to destination available from " << sourceNode << nodeToSearch << destNode << ". Continuing to next nodeToSearch..." << std::endl;
                    //    continue;
						
						//_FT[sourceNode].insert(std::make_pair(destNode, std::multimap<int, std::vector<int>>(tmpMM)));
                        //std::cout << "New next node added! Source Node: " << sourceNode << " Destination Node: " << destNode << " Cost & Path: N/A (no path yet established)" << std::endl;
                    //}
                }
				//If we couldn't find a valid path to destNode, it means we can't get to the destination node through the node we are currently searching without a loop, but we also know that later on in the recursive call we will find a path to destNode. So, we can skip this iteration of the outer loop.
				if(!pathToDestNodeAdded) {
					continue;
				}
			}
            //For each remote node's map of remote nodes (nextHopRemoteNode) and corresponding multimap of routes (nextHopRemoteNodePaths)...
            for(auto&& [nextHopRemoteNode, nextHopRemoteNodePaths] : _FT[destNode]) {
				std::cout << "Starting converge, line 43... Node to search: " << nodeToSearch << " destNode: " << destNode <<  std::endl;
                //Ignore the remote node's entry for itself and entries for sourceNode and prevDestNode, as these nodes are already part of the path (i.e. would introduce loops if added again), and ignore nodes already added by converge
                if(nextHopRemoteNode != destNode && nextHopRemoteNode != sourceNode && destNode != prevDestNode && nextHopRemoteNode != nodeToSearch) {
                   //std::find(nodesAddedOnCurrentPath.begin(), nodesAddedOnCurrentPath.end(), destNode) == nodesAddedOnCurrentPath.end() &&
                   //std::find(nodesAddedOnCurrentPath.begin(), nodesAddedOnCurrentPath.end(), nextHopRemoteNode) == nodesAddedOnCurrentPath.end()) {
                       /*std::cout << "Nodes Added to Current Path: ";
                       for(std::vector<int>::iterator it = nodesAddedOnCurrentPath.begin(); it != nodesAddedOnCurrentPath.end(); it++) {
                            std::cout << *it;
                       }
                       std::cout << std::endl;*/
                //if(nextHopRemoteNode != destNode && nextHopRemoteNode != sourceNode && destNode != prevDestNode && std::find(nodesAddedOnCurrentPath.begin(), nodesAddedOnCurrentPath.end(), destNode) == nodesAddedOnCurrentPath.end()) {
                //if(nextHopRemoteNode != destNode && nextHopRemoteNode != sourceNode && std::find(nodesAddedOnCurrentPath.begin(), nodesAddedOnCurrentPath.end(), destNode) == nodesAddedOnCurrentPath.end()) {
                    //For each route a remote node has to another given node (that is not the remote node itself or the source node)... [nextHopRemoteNodePathCost is cost of route, nextHopRemoteNodePath is vector of ints representing the route]. (Can't just take lowest cost route because it might have a loop through self. Therefore need to iterate until first valid route is found.)
                    for(auto&& [nextHopRemoteNodePathCost, nextHopRemoteNodePath] : nextHopRemoteNodePaths) {
                        //std::cout << "Source Node: " << sourceNode << " Node to Search: " << nodeToSearch << " Previous Hop: " << prevDestNode << " Next Hop: " << destNode << " Next Hop Remote Node: " << nextHopRemoteNode <<  " Next Hop Remote Node Path Cost: " << nextHopRemoteNodePathCost << " Next Hop Remote Node Path: " << vecToString(nextHopRemoteNodePath) << std::endl;
						//We only look at direct links for each level of recursion. Subsequent levels of recursion should cover subsequent links on each available valid path...
						//if(nextHopRemoteNodePath.size() == 1) {
							std::cout << "Source Node: " << sourceNode << " Node to Search: " << nodeToSearch << " Next Hop: " << destNode << " Next Hop Remote Node: " << nextHopRemoteNode <<  " Next Hop Remote Node Path Cost: " << nextHopRemoteNodePathCost << " Next Hop Remote Node Path: " << vecToString(nextHopRemoteNodePath) << std::endl;
							//If the route does not contain the sourceNode (as this would be an invalid route, or in other words a route with a loop)...
							//Make sure this new route would not introduce any loops nor contain the source node (which would also be a loop)
							bool loopFound = false;
							std::vector<int> existingPathToNextHop(_FT[sourceNode].find(destNode)->second.begin()->second);
							for (auto node: nextHopRemoteNodePath) {
								if (std::find(existingPathToNextHop.begin(), existingPathToNextHop.end(), node) != existingPathToNextHop.end()) {
									loopFound = true;
									//break;
								}
							}
							if(loopFound){
								std::cout << "Loop Found. Existing Path To Next Node: " << vecToString(existingPathToNextHop) << " Remote node path from " << nextHopRemoteNode << ": " << vecToString(nextHopRemoteNodePath) << std::endl;
							}
							else {
								std::cout << "Loop Not Found. Existing Path To Next Node: " << vecToString(existingPathToNextHop) << " Remote node path from " << nextHopRemoteNode << ": " << vecToString(nextHopRemoteNodePath) << std::endl;
							}
							if(std::find(nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end(), sourceNode) == nextHopRemoteNodePath.end() && !loopFound) {  //    destNodePaths.begin()->first > ) {
								//std::cout << "Next Hop: " << destNode << std::endl;
								//If the sourceNode has no current entry for the remote node's reachable node, add a new entry as this new node can now be reached from the source via neighbor chain.
								if(_FT[sourceNode].find(nextHopRemoteNode) == _FT[sourceNode].end()) {
									//If top-level / first call of recursion, calculate cost and update path according to directly connected neighbors
									//if (nodeToSearch == sourceNode) { 
									int costToNextHop = _FT[sourceNode].find(destNode)->second.begin()->first;
									totalPathCost = costToNextHop + nextHopRemoteNodePathCost;
									//Initialize newPath vector with existing path to next hop
									std::vector<int> newPath(existingPathToNextHop);
									//Then add path from next hop to remote node
									newPath.insert(newPath.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
									//Use temporary multimap to construct final map entry for newly reachable node
									tmpMM.insert(std::make_pair(totalPathCost, newPath));
									
									//bool canFindNewlyAddedNodeInSourceNodesFT = _FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end();
									//std::cout << "Can find new Destination Node After Adding Node " << nextHopRemoteNode << " now...? " << canFindNewlyAddedNodeInSourceNodesFT << std::endl;
									
									//nodesAddedOnCurrentPath.push_back(nextHopRemoteNode);
									_FT[sourceNode].insert(std::make_pair(nextHopRemoteNode, std::multimap<int, std::vector<int>>(tmpMM)));
									
									//canFindNewlyAddedNodeInSourceNodesFT = _FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end();
									//std::cout << "Can find new Destination Node After Adding Node " << nextHopRemoteNode << " now...? " << canFindNewlyAddedNodeInSourceNodesFT << std::endl;
									
									//nodesAddedOnCurrentPath.push_back(nextHopRemoteNode);
									std::cout << "New remote node added! Source Node: " << sourceNode << " Destination Node: " << nextHopRemoteNode << " Cost: " << totalPathCost << " Path: " << vecToString(newPath) << std::endl;
									tmpMM.clear();
										
									//}
									//If not top-level call, calculate cost and update path according to what was just previously calculated / updated for prior node (added in previous call)
									//else { 
									//    totalPathCost = _FT[sourceNode].find(neighbor)->second.begin()->first + nextHopRemoteNodePathCost;
									//    std::vector<int> newPath(_FT[sourceNode].find(neighbor)->second.begin()->second);
									//    newPath.push_back(nextHopRemoteNode);
									//}
									//std::cout << "Next Hop: " << destNode << std::endl;
									//Because this is a newly reachable node, need to recursively check it in turn for any newly reachable nodes [from this newly added node, that is]
									converge(sourceNode, nextHopRemoteNode, destNode, _FT);
									//converge(sourceNode, nextHopRemoteNode, _FT);
									
									//After returning from each level of recursion, remove the last node added to the newly added nodes list
									//nodesAddedOnCurrentPath.pop_back();
									
									//Can break because multimap is sorted, so this should be lowest cost option that doesn't introduce a loop (i.e. lowest cost valid route). No need to check additional routes to target node.
									//However... removing this break statement may result in a slower initial convergence but provide alternate routes initially, resulting in faster reconvergences after topology changes are introduced.
									break;
								}
								//If there already is an entry for the remote node's reachable node (but not one that has already been added by converge previously 
								else {
								//else if (_FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end()) {
								//else if (_FT[sourceNode].find(destNode) != _FT[sourceNode].end()) {
								//else if (_FT[sourceNode].find(destNode) != _FT[sourceNode].end() && _FT[sourceNode].find(nextHopRemoteNode)->second.begin()->first > _FT[sourceNode].find(destNode)->second.begin()->first + nextHopRemoteNodePathCost) {
									totalPathCost = _FT[sourceNode].find(destNode)->second.begin()->first + nextHopRemoteNodePathCost;
									//Compare remote node's cost to lowest existing cost route for target node. If new path cost is less than the existing path cost in sourceNode table, add new entry for target node. 
									if (_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->first > totalPathCost) {
										//Initialize the vector [that represents the path] for the new entry with the existing path to the next hop, then add the path from the next hop to the remote node to it.
										_FT[sourceNode].find(nextHopRemoteNode)->second.insert(std::make_pair(totalPathCost, std::vector<int>(existingPathToNextHop)));
										_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.insert(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
										
										std::cout << "New path added for existing node. Source Node: " << sourceNode << " Destination Node: " << nextHopRemoteNode << " Cost: " << totalPathCost << " Path: " << vecToString(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second) << std::endl;
										std::cout << "Path Cost Breakdown for New Path Added to Existing Node: " << "Existing path cost to destNode: " << _FT[sourceNode].find(destNode)->second.begin()->first << " Path cost from nextHopRemoteNode to Destination: " << nextHopRemoteNodePathCost << std::endl;
										
										//Can break because multimap is sorted, so this should be lowest cost option that doesn't introduce a loop (i.e. lowest cost valid route). No need to check additional routes to target node.
										//However... removing this break statement may result in a slower initial convergence but provide alternate routes initially, resulting in faster reconvergences after topology changes are introduced.
										break;
									}
									//Else if new path cost is equal to the existing path cost in sourceNode table, break tie based on route's first hop node number (lowest first hop = highest priority) . 
									else if (_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->first == totalPathCost) {
										
										//Initialize newPath vector with existing path to next hop
										std::vector<int> newPath(existingPathToNextHop);
										//Then add path from next hop to remote node
										newPath.insert(newPath.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
										
										//Make sure this path is not already in the table (need to search all paths to this node, as there may be multiple paths with the same lowest cost (i.e. tied paths, don't want to add duplicates of tied lowest cost paths that may or may not be highest priority)
										//Could maybe just use == between two vectors here instead... :/
										bool pathAlreadyInTable = false;
										for(auto&& [existingNextHopRemoteNodePathCost, existingNextHopRemoteNodePath] : _FT[sourceNode].find(nextHopRemoteNode)->second) {
											if(existingNextHopRemoteNodePath.size() == newPath.size()) { 
												std::vector<int>::iterator it_nextHopRemoteNodePath;
												std::vector<int>::iterator it_newPath;
												for(it_nextHopRemoteNodePath = existingNextHopRemoteNodePath.begin(), it_newPath = newPath.begin();
													it_nextHopRemoteNodePath != existingNextHopRemoteNodePath.end() && it_newPath != newPath.end();
													it_nextHopRemoteNodePath++, it_newPath++) {
														if(*it_nextHopRemoteNodePath != *it_newPath) {
															break;
														}
														if(it_nextHopRemoteNodePath+1 == existingNextHopRemoteNodePath.end() && it_newPath+1 == newPath.end()) {
															pathAlreadyInTable = true;
														}
													}	
											}
										}
										if(!pathAlreadyInTable) {				
										int highestPriorityIsNewPath = tieBreaker(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second, newPath);
											/*std::vector<int>::iterator npIt, epIt;
											for(epIt = _FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.begin(), npIt = newPath.begin() ;
												epIt != _FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.end() && npIt != newPath.end();
												epIt++, npIt++) {
													std::cout << "new path node: " << *npIt << " existing path node: " << *epIt << std::endl;
												if (*npIt < *epIt) {
													highestPriorityIsNewPath = 1;
													std::cout << "Did we get to npIt < epIt...??" << std::endl;
													break;
												}
												else if (*npIt > *epIt) {
													highestPriorityIsNewPath = 0;
													std::cout << "Did we get to npIt < epIt...??" << std::endl;
													break;
												}
											}*/
											//if(epIt == _FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.end() && npIt == newPath.end() && highestPriorityIsNewPath == -1) {
											if(highestPriorityIsNewPath == -1) {
												//std::cout << "epIt == existing path end?: " << npIt == 
												//If we get here, paths are identical. Do nothing... (i.e. do not add duplicate paths)
												std::cout << "Are we here skipping over identical paths...?" << std::endl;
												break;
											}
											else if (highestPriorityIsNewPath) {
												//New path gets added straight in if it has the highest priority, as multimap ordering will be according to insertion when keys are duplicated
												//Initialize the vector [that represents the path] for the new entry with the existing path to the next hop, then add the path from the next hop to the remote node to it.
												
												//If top-level / first call of recursion, calculate cost and update path according to directly connected neighbors
												//if (nodeToSearch == sourceNode) { 
												int costToNextHop = _FT[sourceNode].find(destNode)->second.begin()->first;
												totalPathCost = costToNextHop + nextHopRemoteNodePathCost;
												//Initialize newPath vector with existing path to next hop
												//std::vector<int> newPath(existingPathToNextHop);
												//Then add path from next hop to remote node
												//newPath.insert(newPath.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
												
												//bool canFindNewlyAddedNodeInSourceNodesFT = _FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end();
												//std::cout << "Can find new Destination Node After Adding Node " << nextHopRemoteNode << " now...? " << canFindNewlyAddedNodeInSourceNodesFT << std::endl;
												
												//nodesAddedOnCurrentPath.push_back(nextHopRemoteNode);
												_FT[sourceNode].find(nextHopRemoteNode)->second.insert(std::make_pair(totalPathCost, std::vector<int>(newPath)));
												
												//_FT[sourceNode].find(nextHopRemoteNode)->second.insert(std::make_pair(totalPathCost, std::vector<int>(_FT[sourceNode].find(destNode)->second.begin()->second)));
												//_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.insert(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
												
												//std::cout << "!!!!Existing path to next hop added: " << vecToString(_FT[sourceNode].find(destNode)->second.begin()->second) << " Next hop remote node path added to it: " << vecToString(nextHopRemoteNodePath) << std::endl;
												std::cout << "!!!!New path to next hop added: " << vecToString(newPath) << std::endl;
												
												std::cout << "New path added for existing node (tie breaker: new path highest priority). Source Node: " << sourceNode << " Destination Node: " << nextHopRemoteNode << " Cost: " << totalPathCost << " Path: " << vecToString(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second) << std::endl;
												std::cout << "Path Cost Breakdown for New Path Added to Existing Node: " << "Existing path cost to destNode: " << _FT[sourceNode].find(destNode)->second.begin()->first << " Path cost from nextHopRemoteNode to Destination: " << nextHopRemoteNodePathCost << std::endl;
												
												//Can break because multimap is sorted, so this should be lowest cost option that doesn't introduce a loop (i.e. lowest cost valid route). No need to check additional routes to target node.
												//However... removing this break statement may result in a slower initial convergence but provide alternate routes initially, resulting in faster reconvergences after topology changes are introduced.
												break;
											}
											else {
												//Getting here means the existing path is higher priority. The new path is still added [in case it is needed later after a network change] but is placed lower in the ordering of the multimap...
												//Save temp copy of highest priorty entry then remove...
												std::pair<int, std::vector<int>> tmpMMEntry(*(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()));
												//Erase the highest priority entry from the source node's FT...
												_FT[sourceNode].find(nextHopRemoteNode)->second.erase(_FT[sourceNode].find(nextHopRemoteNode)->second.begin());
												
												//Add the new path with equivalent cost but lower priority to the source node's FT...
												//_FT[sourceNode].find(nextHopRemoteNode)->second.insert(std::make_pair(totalPathCost, std::vector<int>(_FT[sourceNode].find(destNode)->second.begin()->second)));
												//_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.insert(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
												int costToNextHop = _FT[sourceNode].find(destNode)->second.begin()->first;
												totalPathCost = costToNextHop + nextHopRemoteNodePathCost;
												//Initialize newPath vector with existing path to next hop
												//std::vector<int> newPath(existingPathToNextHop);
												//Then add path from next hop to remote node
												//newPath.insert(newPath.end(), nextHopRemoteNodePath.begin(), nextHopRemoteNodePath.end());
												
												//bool canFindNewlyAddedNodeInSourceNodesFT = _FT[sourceNode].find(nextHopRemoteNode) != _FT[sourceNode].end();
												//std::cout << "Can find new Destination Node After Adding Node " << nextHopRemoteNode << " now...? " << canFindNewlyAddedNodeInSourceNodesFT << std::endl;
												
												//nodesAddedOnCurrentPath.push_back(nextHopRemoteNode);
												_FT[sourceNode].find(nextHopRemoteNode)->second.insert(std::make_pair(totalPathCost, std::vector<int>(newPath)));
												
												//Add back in the highest priority entry, which will retain highest priority by virtue of being added last, despite duplicate key...
												_FT[sourceNode].find(nextHopRemoteNode)->second.insert(tmpMMEntry);
												
												std::cout << "New path added for existing node (tie breaker: existing path has highest priority). Source Node: " << sourceNode << " Destination Node: " << nextHopRemoteNode << " Cost: " << totalPathCost << " Path: " << vecToString(_FT[sourceNode].find(nextHopRemoteNode)->second.begin()->second) << std::endl;
												std::cout << "Path Cost Breakdown for New Path Added to Existing Node: " << "Existing path cost to destNode: " << _FT[sourceNode].find(destNode)->second.begin()->first << " Path cost from nextHopRemoteNode to Destination: " << nextHopRemoteNodePathCost << std::endl;
												
												//Can break because multimap is sorted, so this should be lowest cost option that doesn't introduce a loop (i.e. lowest cost valid route). No need to check additional routes to target node.
												//However... removing this break statement may result in a slower initial convergence but provide alternate routes initially, resulting in faster reconvergences after topology changes are introduced.
												break;
											}
										}
									}
									else {
										std::cout << "Existing path cheaper than this path. Continuing..." << std::endl;
										//Could decide to add higher cost valid path here anyway in case it's needed later but reconvergences should pick it up again if needed after a change...
									}
								}
							}
						}
                    }
                }
            }
        //}
        
        //delete nodesAddedOnCurrentPath;
    }
}

//For Dist Vec (Attempt 2)
void converge(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, int sourceNode, int prevNode, std::vector<int>* newPath, int newPathCost) {      
//void converge(int sourceNode, int neighbor, int prevDestNode, std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::vector<int>* newPath) {
//void converge(int sourceNode, int neighbor, std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT) {  
	
	//Initialize newPath vector with existing path to this prevNode
	//std::vector<int> _newPath;
	
	std::vector<int>* _newPath = new std::vector<int>();
	
	//On first level of recursion, establish an empty path vector
	if(sourceNode == prevNode) {
		newPath = _newPath;
	}
	else {
		newPath->push_back(prevNode);
	}
	//For node directly connected to the source node, recursively find paths to all other reachable nodes. (sourceNode == prevNode on first level of recursion)
	//for (auto it_m = _FT[prevNode].begin(), next_it = it_m; it_m != _FT[prevNode].end(); it_m = next_it) {
	//	next_it++;
	 //	int reachableNode = it_m->first;
	//	std::multimap<int, std::vector<int>> reachableNodePaths = it_m->second;
	for(auto&& [reachableNode, reachableNodePaths] : _FT[prevNode]) {
		//Ignore source self entry
		if(reachableNode != sourceNode) {
			for(auto&& [reachableNodeCost, reachableNodePath] : reachableNodePaths) {
				//if node is neighbor (i.e. directly connected) and this path is lowest cost option of direct links (in case multiple direct links of different costs)
				if(reachableNodePath.size() == 1 && reachableNode == reachableNodePath[0] && std::find(newPath->begin(), newPath->end(), reachableNode) == newPath->end()) {
					newPathCost += reachableNodeCost;
					newPath->push_back(reachableNode);
					std::cout << "New Converge... sourceNode: " << sourceNode << " prevNode: " << prevNode << " reachableNode: " << reachableNode << " reachableNodeCost: " << reachableNodeCost << " reachableNodePathSize: " << reachableNodePath.size() << " reachableNodePath: " << vecToString(reachableNodePath) << " reachableNodeFirstHop: " << reachableNodePath[0] << " newPath: " << vecToString(*newPath) << " newPathCost: " << newPathCost << std::endl;
					//If there is not yet a path established from source to this node...(will never have to do this for a node's first level of recursion because initial topology loads all direct links into FT)
					if(_FT[sourceNode].find(reachableNode) == _FT[sourceNode].end() || _FT[sourceNode].find(reachableNode)->second.begin()->first > newPathCost) {
						//Add newly discovered path for this new node. 
						//std::cout << "Got here: Line 370..." << std::endl;
						if(_FT[sourceNode].find(reachableNode) == _FT[sourceNode].end()) {
							std::multimap<int, std::vector<int>> tmpMM;
							//newPath->push_back(reachableNode);
							tmpMM.insert(std::make_pair(newPathCost, *newPath));
							_FT[sourceNode].insert(std::make_pair(reachableNode, std::multimap<int, std::vector<int>>(tmpMM)));
							//newPath->pop_back();
							std::cout << "New Converge - New node added!... sourceNode: " << sourceNode << " prevNode: " << prevNode << " reachableNode: " << reachableNode << " reachableNodeCost: " << reachableNodeCost << " newPath: " << vecToString(*newPath) << " newPathCost: " << newPathCost << std::endl;
						}
						else {
							//newPath->push_back(reachableNode);
							_FT[sourceNode].find(reachableNode)->second.insert(std::make_pair(newPathCost, *newPath));
							//newPath->pop_back();
							std::cout << "New Converge - New cheaper path added to existing node!... sourceNode: " << sourceNode << " prevNode: " << prevNode << " reachableNode: " << reachableNode << " reachableNodeCost: " << reachableNodeCost << " newPath: " << vecToString(*newPath) << " newPathCost: " << newPathCost << std::endl;
						}
					}
					else if(_FT[sourceNode].find(reachableNode)->second.begin()->first == newPathCost && newPath->size() > 0) {
						//std::cout << "Got here: Line 376..." << " new path size: " << newPath->size() << std::endl;
						//tmpMM.insert(std::make_pair(newPathCost, *newPath));
						
						//Can add straight in if tie breaker goes to the newly sought/established path
						int tieBreakGoesToNewPath = tieBreaker(*newPath, _FT[sourceNode].find(reachableNode)->second.begin()->second);
						//std::cout << "Got here: Line 381..." << std::endl;
						if(!tieBreakGoesToNewPath) {
							//newPath->push_back(reachableNode);
							_FT[sourceNode].find(reachableNode)->second.insert(std::make_pair(newPathCost, *newPath));
							//newPath->pop_back();
							std::cout << "New Converge - New Path Added to Existing Node - Tie: New Path = Highest Priority... sourceNode: " << sourceNode << " prevNode: " << prevNode << " reachableNode: " << reachableNode << " reachableNodeCost: " << reachableNodeCost << " newPath: " << vecToString(*newPath) << " newPathCost: " << newPathCost << std::endl;
						}
						//Else need to remove existing highest priority path, add new path, then restore/re-add highest priority to maintain proper ordering in multimap
						else if (tieBreakGoesToNewPath != -1) {
							std::cout << "tieBreakGoesToNewPath: " << tieBreakGoesToNewPath << std::endl;
							//Save current entry in temp pair...
							std::cout << "saving current highest priority... " << vecToString(_FT[sourceNode].find(reachableNode)->second.begin()->second) << std::endl;
							std::pair tmpMMEntry = std::make_pair(newPathCost, _FT[sourceNode].find(reachableNode)->second.begin()->second);
							//Erase current entry...
							//_FT[sourceNode].find(reachableNode)->second.erase(_FT[sourceNode].find(reachableNode)->second.find(newPathCost));
							_FT[sourceNode].find(reachableNode)->second.erase(_FT[sourceNode].find(reachableNode)->second.begin());
							//Add new direct link with updated cost..
							//newPath->push_back(reachableNode);
							_FT[sourceNode].find(reachableNode)->second.insert(std::make_pair(newPathCost, std::vector<int>(*newPath)));
							//newPath->pop_back();
							//Add back existing entry back in so it gets loaded into the multimap for destNode at this cost (==change) top priority
							_FT[sourceNode].find(reachableNode)->second.insert(tmpMMEntry);
							std::cout << "New Converge - New Path Added to Existing Node - Tie: Existing Path = Highest Priority... sourceNode: " << sourceNode << " prevNode: " << prevNode << " reachableNode: " << reachableNode << " reachableNodeCost: " << reachableNodeCost << " newPath: " << vecToString(*newPath) << " newPathCost: " << newPathCost << std::endl;
						}
					}
					else {
						//Could add newly discovered higher cost paths here if wanted...
						std::cout << "Cheaper path already available for and/or this direct link already exists in table" << sourceNode << "->" << reachableNode << ". Skipping this path..." << std::endl;
					}
					newPath->pop_back();
					converge(_FT, sourceNode, reachableNode, newPath, newPathCost);
					newPath->pop_back();
					newPathCost -= reachableNodeCost;
				}
			}
		}
	}
	delete _newPath;
}

//For Link State
void converge(int sourceNode, std::vector<std::map<int, int>> &_TT, std::vector<std::map<int, std::pair<int, int>>> &_FT) {
	
	//Initialize container for Dijkstras. Map key is reachable node, first element of pair (int) is the "previous" node (or, alternatively from the view of the destination node itself, next hop to source), while the first element of the pair is the reachable node shortest path cost, 
	std::map<int, std::pair<int,int>> dijk;
	
	//Initialize helper containers and min distance node tracker
	std::set<int> visitedNodes, unvisitedNodes;
	int minDistNode, minDist = std::numeric_limits<int>::max();
	
	//Initialize shortest distance values, with sourceNode = 0 and all other nodes set to infinity
	for (int i = 1; i < _FT.size(); i++) {
		//Do not add node numbers with no established links to dijkstras (if, for example, some node numbers were skipped in topology input file)
		if (_FT[i].find(i)->second.second != -1) {
			if(i == sourceNode) {
				dijk.insert(std::make_pair(sourceNode, std::make_pair(i,0)));
				visitedNodes.insert(sourceNode);
			}
			else {			
				dijk.insert(std::make_pair(i, std::make_pair(-1, std::numeric_limits<int>::max())));
				unvisitedNodes.insert(i);
			}
		}
	}
	
	std::cout << std::endl;
	std::cout << "dijkstras table after initialization with infinite values for all non-source nodes (and 0 for source):" << std::endl;
	for(auto&& [destNode, prevNode_cost] : dijk) {
		std::cout << "Source Node: " << sourceNode << " Dest Node: " << destNode << " Shortest Distance: " << prevNode_cost.second << " prev node: " << prevNode_cost.first << std::endl;
	}
	std::cout << "visitedNodes: " << setToString(visitedNodes) << " unvisitedNodes: " << setToString(unvisitedNodes) << std::endl;
	std::cout << std::endl;
	
	//First iteration, for all of sourceNode's directly connected nodes
	for(std::map<int, int>::iterator it = _TT[sourceNode].begin(); it != _TT[sourceNode].end(); it++) {
		int reachableNode = it->first;
		if(reachableNode != sourceNode) {
			int reachableNodeCost = it->second;
			//Add cost to dijkstras entry from source node to this reachable node
			dijk.find(reachableNode)->second.first = sourceNode;
			dijk.find(reachableNode)->second.second = reachableNodeCost;
			if(reachableNodeCost < minDist) {
				minDist = reachableNodeCost;
				minDistNode = reachableNode;
			}
			//Break tie here if needed
			else if(reachableNodeCost == minDist) {
				if(dijk.find(reachableNode)->second.first < minDistNode) {
					minDistNode = reachableNode;
				}
			}
		}
	}
	
	std::cout << std::endl;
	std::cout << "dijkstras table after first initialization with source direct links:" << std::endl;
	for(auto&& [destNode, prevNode_cost] : dijk) {
		std::cout << "Source Node: " << sourceNode << " Dest Node: " << destNode << " Shortest Distance: " << prevNode_cost.second << " prev node: " << prevNode_cost.first << std::endl;
	}
	std::cout << "visitedNodes: " << setToString(visitedNodes) << " unvisitedNodes: " << setToString(unvisitedNodes) << std::endl;
	std::cout << std::endl;
	
	std::cout << "dijkstras main run..." << std::endl;
	int unvisitedNodesInitialSize = unvisitedNodes.size();
	for (int i = 0; i < unvisitedNodesInitialSize; i++) {
		//Find next unvisited node with minimum distance from source node
		std::set<int>::iterator minDistIt = unvisitedNodes.find(minDistNode);
		//Used to track min distance update needed for next loop iteration...
		//int nextMinDistNode, nextMinDist = std::numeric_limits<int>::max();
		//Used to find adjust nextMinDistNode in case of tie breaker
		//std::vector<int> tiedForLowestNextMinDistance;
		std::cout << "minDistNode: " << minDistNode << " ";
		for(std::map<int, int>::iterator it = _TT[*minDistIt].begin(); it != _TT[*minDistIt].end(); it++) {
			int reachableNode = it->first, reachableNodeCost = it->second;
			std::cout << "next node connected to minDistNode (" << reachableNode << ") ... cost = reachableNodeCost " << "(" << reachableNodeCost << ")." << " So therefore, cost to reachableNode = minDist + reachableNodeCost = " << minDist+reachableNodeCost << std::endl;
			if(unvisitedNodes.find(reachableNode) != unvisitedNodes.end() && 
			  ((reachableNodeCost + minDist < dijk.find(reachableNode)->second.second) ||
			  (dijk.find(reachableNode)->second.second == reachableNodeCost + minDist && dijk.find(reachableNode)->second.first > minDistNode))) {
				dijk.find(reachableNode)->second.first = minDistNode;
				dijk.find(reachableNode)->second.second = reachableNodeCost + minDist;
				std::cout << "Adding previous node " << minDistNode << " and cost of " << reachableNodeCost + minDist << " for reachableNode " << reachableNode << " to sourceNode " << sourceNode << std::endl;
			}
			//if(unvisitedNodes.find(reachableNode) != unvisitedNodes.end() && reachableNodeCost + minDist < nextMinDist) {
				//nextMinDist = reachableNodeCost + minDist;
				//nextMinDistNode = reachableNode;
				//tiedForLowestNextMinDistance.clear();
				//tiedForLowestNextMinDistance.push_back(reachableNode);
			//	std::cout << "nextMinDist updated here to " << nextMinDist << " and nextMinDistNode updated to " << nextMinDistNode << std::endl;
			//}
			//if(unvisitedNodes.find(reachableNode) != unvisitedNodes.end() && reachableNodeCost + minDist == nextMinDist) {
			//	tiedForLowestNextMinDistance.push_back(reachableNode);
			//}
			if(unvisitedNodes.find(reachableNode) == unvisitedNodes.end()) {
				std::cout << "node " << reachableNode << " already visited. Skipping over here..." << std::endl;
			}
		}
		//Break tie for next min distance if needed
		/* if(tiedForLowestNextMinDistance.size() > 1) {
			int winner = std::numeric_limits<int>::max();
			for(auto node : tiedForLowestNextMinDistance) {
				if(dijk.find(node)->second.first < winner) {
					winner = node;
				}
			}
			nextMinDistNode = winner;
			std::cout << "Tie encountered for nextMinDistNode update. nextMinDist now " << nextMinDist << " (should be same as just printed) and nextMinDistNode updated to " << nextMinDistNode << std::endl;
		} */
		
		std::cout << std::endl;
		std::cout << "dijkstras table after main loop for reachable node " << minDistNode << " (loop number: " << i+1 << ")" << std::endl;
		
		//Add node to visited Nodes
		visitedNodes.insert(*minDistIt);
		//Remove node from unvisitedNodes
		std::cout << "Trying to erase " << *minDistIt << " here from univistedNodes... unvisitedNodes before erasure attempt: " << setToString(unvisitedNodes) << std::endl;
		unvisitedNodes.erase(*minDistIt);
		std::cout << "UnvisitedNodes after erasure attempt: " << setToString(unvisitedNodes) << std::endl;
		//Update minDistNode and minDist
		//if(minDistNode != nextMinDistNode) {
		//	minDistNode = nextMinDistNode;
		//	minDist = nextMinDist;
		//}
		//If we get here it means we got to the end of a path with no loop back to the source, and we are out of remaining reachable unvisited nodes but not done with the algorithm. Therefore, we need to search back through our dijkstras table so far to find the next lowest cost path (that is not the source, of course)
		//else {
			minDist = std::numeric_limits<int>::max();
			for(std::set<int>::iterator it = unvisitedNodes.begin(); it != unvisitedNodes.end(); it++ ) { 
				//Ignore self-entry, where cost == 0
				if(dijk.find(*it)->second.second > 0 && dijk.find(*it)->second.second < minDist) {
					minDist = dijk.find(*it)->second.second;
					minDistNode = *it;
				}
				//In case of tie (that is not max int limit)
				if(dijk.find(*it)->second.second > 0 && dijk.find(*it)->second.second != std::numeric_limits<int>::max() && dijk.find(*it)->second.second == minDist) {
					if(dijk.find(*it)->second.first < dijk.find(minDistNode)->second.first) {
						minDist = dijk.find(*it)->second.second;
						minDistNode = *it;
					}
				}
			}
		//}
		
		std::cout << "nextMinDistNode: " << minDistNode << " nextMinDist: " << minDist << std::endl;
		for(auto&& [destNode, prevNode_cost] : dijk) {
			std::cout << "Source Node: " << sourceNode << " Dest Node: " << destNode << " Shortest Distance: " << prevNode_cost.second << " prev node: " << prevNode_cost.first << std::endl;
		}
		std::cout << "visitedNodes: " << setToString(visitedNodes) << " unvisitedNodes: " << setToString(unvisitedNodes) << std::endl;

		std::cout << std::endl;
	}
	
	std::cout << std::endl;
	std::cout << "Final dijkstras table:" << std::endl;
	for(auto&& [destNode, prevNode_cost] : dijk) {
		std::cout << "Source Node: " << sourceNode << " Dest Node: " << destNode << " Shortest Distance: " << prevNode_cost.second << " prev node: " << prevNode_cost.first << std::endl;
	}
	std::cout << "visitedNodes: " << setToString(visitedNodes) << " unvisitedNodes: " << setToString(unvisitedNodes) << std::endl;
	std::cout << std::endl;
	
	//Dijkstras Table is now built for this sourceNode. Update the forwarding table (_FT) accordingly
	//for(auto&& [reachableNode, nextHop_cost] : dijk) {
	for(std::map<int, std::pair<int,int>>::iterator it = dijk.begin(); it != dijk.end(); it++) {
		//int nextHop = nextHop_cost.first, cost = nextHop_cost.second;
		int reachableNode = it->first, nextHop = it->second.first, cost = it->second.second;
		std::cout << "Should be adding entry for reachableNode " << reachableNode << " to source node: " << sourceNode << " with next hop of " << nextHop << " and cost of " << cost << std::endl;
		//Note: if cost does equal std::numeric_limits<int>::max(), a path was not discovered to the node in question and it is therefore unreachable from source
		if(cost != std::numeric_limits<int>::max() && reachableNode != sourceNode) {
			if(_FT[reachableNode].find(sourceNode) != _FT[reachableNode].end()) {
				_FT[reachableNode].find(sourceNode)->second.first = nextHop;
				_FT[reachableNode].find(sourceNode)->second.second = cost;
				//_FT[reachableNode].find(sourceNode)->insert(std::make_pair(nextHop, cost);
				//std::cout << "Just added to FT here, existing entry for source node " << sourceNode << " already present. FT[reachableNode].find(sourceNode)->second.first = " << _FT[reachableNode].find(sourceNode)->second.first << " _FT[reachableNode].find(sourceNode)->second.second = " << _FT[reachableNode].find(sourceNode)->second.second << std::endl;
				//consoleOutFT(_FT);
			}
			else {
				_FT[reachableNode].insert(std::make_pair(sourceNode, std::make_pair(std::move(nextHop), cost)));
				//std::cout << "Just added to FT here, new entry for source node " << sourceNode << ". FT[reachableNode].find(sourceNode)->second.first = " << _FT[reachableNode].find(sourceNode)->second.first << " _FT[reachableNode].find(sourceNode)->second.second = " << _FT[reachableNode].find(sourceNode)->second.second << std::endl;
			}
		}
	}
	std::cout << std::endl;
	std::cout << "FT after final dijkstras inside converge... just ran for source node: " << sourceNode << std::endl;
	std::cout << std::endl;
	consoleOutFT(_FT);
	
	//Print converged FT (testing/troubleshooting only)
    std::cout << std::endl;
}