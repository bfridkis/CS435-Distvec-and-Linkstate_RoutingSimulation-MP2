/***************************************************
** Program name: outputFunctions.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for output functions.
***************************************************/

#include "../HEADERS/outputFunctions.hpp"

//For testing/debugging only - For DistVec
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

//For Printing the Forwarding Table Elements to a File - For DistVec
void fileOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ofstream& _outFile) {
	for (unsigned short sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
        //If self-entry not marked with cost of -1 (which means this node number was never part of the topology)...
		if(_FT[sourceNode].find(sourceNode)->second.find(-1) == _FT[sourceNode].find(sourceNode)->second.end()) {
			for (std::map<int, std::multimap<int, std::vector<int>>>::iterator it=_FT[sourceNode].begin(); it!=_FT[sourceNode].end(); it++) {
				//std::cout << "output file is getting... " << sourceNode << " " << it->first << " " << it->second.begin()->first << " size of path: " << it->second.begin()->second.size() << std::endl;
				//_outFile << sourceNode << " " << it->first << " " << it->second.begin()->first << std::endl;
				if(it->second.begin()->second.size() == 0) {
					//The only entry for which the path size is 0 is a self entry, so it->first should == sourceNode
					//if(sourceNode == _FT.size() - 1) { 
					//	_outFile << it->first << " " << it->first << " " << it->second.begin()->first; 
					//}
					//else { 
						_outFile << it->first << " " << it->first << " " << it->second.begin()->first << std::endl; 
					//}
				}
				else {
					//if(sourceNode == _FT.size() - 1) { 
					//	_outFile << it->first << " " << it->second.begin()->second[0] << " " << it->second.begin()->first << std::endl; 
					//}
					//else { 
						_outFile << it->first << " " << it->second.begin()->second[0] << " " << it->second.begin()->first << std::endl; 
					//}
				}
			}
		}
    }
}

//For Printing the Forwarding Table Elements to a File - For DistVec
void fileOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ofstream& _outFile, bool _initialConverge) {
	if(_FT.size() > 1 && !_initialConverge) { 
		_outFile << std::endl; 
	}
	for (unsigned short sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
        //If self-entry not marked with cost of -1 (which means this node number was never part of the topology)...
		if(_FT[sourceNode].find(sourceNode)->second.find(-1) == _FT[sourceNode].find(sourceNode)->second.end()) {
			for (std::map<int, std::multimap<int, std::vector<int>>>::iterator it=_FT[sourceNode].begin(); it!=_FT[sourceNode].end(); it++) {
				//std::cout << "output file is getting... " << sourceNode << " " << it->first << " " << it->second.begin()->first << " size of path: " << it->second.begin()->second.size() << std::endl;
				//_outFile << sourceNode << " " << it->first << " " << it->second.begin()->first << std::endl;
				if(it->second.begin()->second.size() == 0) {
					//The only entry for which the path size is 0 is a self entry, so it->first should == sourceNode
					if(sourceNode == _FT.size() - 1) { 
						_outFile << it->first << " " << it->first << " " << it->second.begin()->first; 
					}
					else { 
						_outFile << it->first << " " << it->first << " " << it->second.begin()->first << std::endl; 
					}
				}
				else {
					if(sourceNode == _FT.size() - 1) { 
						_outFile << it->first << " " << it->second.begin()->second[0] << " " << it->second.begin()->first << std::endl; 
					}
					else { 
						_outFile << it->first << " " << it->second.begin()->second[0] << " " << it->second.begin()->first << std::endl; 
					}
				}
			}
		}
    }
}

//For testing/debugging only - For Link State
void consoleOutFT(std::vector<std::map<int, std::pair<int,int>>> &_FT) {
    //Print converged FT (testing/troubleshooting only)
    std::cout << std::endl;

    for (unsigned short sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
        //std::cout << "sourceNode: " << sourceNode << " " << _FT.size() << std::endl;
        for (std::vector<std::map<int, std::pair<int,int>>>::iterator it=_FT[sourceNode].begin(); it!=_FT[sourceNode].end(); it++) {
           
			std::cout << "source: " << sourceNode << " Destination: " << it->first << " Next Hop: " << it->second.first << " Cost: " << it->second.second << std::endl;
			
			//std::cout << "source: " << sourceNode << " Destination: " << it->first << " Cost: " << itMM->first << " Path: " << pathToPrint << std::endl;
			//std::cout << "source: " << sourceNode << " Destination: " << it->first << " Cost: " << itMM->first << " First Hop: " << itMM->second.size() << std::endl;
			//std::cout << "-------------------------------------" << std::endl;
		}
	}
}

//For Printing the Forwarding Table Elements to a File - For Link State
void fileOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ofstream& _outFile, bool _initialConverge) {
	if(_FT.size() > 1 && !_initialConverge) { 
		_outFile << std::endl; 
	}
	for (unsigned short sourceNode = 1; sourceNode < _FT.size(); sourceNode++) {
        //std::cout << "sourceNode: " << sourceNode << " " << _FT.size() << std::endl;
        for (std::vector<std::map<int, std::pair<int,int>>>::iterator it=_FT[sourceNode].begin(); it!=_FT[sourceNode].end(); it++) {
           
			_outFile << it->first << " " << it->second.first << " " << it->second.second << std::endl;
			
			//std::cout << "source: " << sourceNode << " Destination: " << it->first << " Cost: " << itMM->first << " Path: " << pathToPrint << std::endl;
			//std::cout << "source: " << sourceNode << " Destination: " << it->first << " Cost: " << itMM->first << " First Hop: " << itMM->second.size() << std::endl;
			//std::cout << "-------------------------------------" << std::endl;
		}
	}
}