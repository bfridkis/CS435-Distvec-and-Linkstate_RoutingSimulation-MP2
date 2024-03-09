/***************************************************
** Program name: outputFunctions.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for output functions.
***************************************************/

#include "../HEADERS/outputFunctions.hpp"

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

//For Printing the Forwarding Table Elements to a File
void fileOutFT(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ofstream& _outFile) {
    for (unsigned short i = 1; i < _FT.size(); i++) {
        for (std::map<int, std::multimap<int, std::vector<int>>>::iterator it=_FT[i].begin(); it!=_FT[i].end(); it++) {
            //std::cout << "output file is getting... " << i << " " << it->first << " " << it->second.begin()->first << std::endl;
			//_outFile << i << " " << it->first << " " << it->second.begin()->first << std::endl;
			_outFile << it->first << " " << it->second.begin()->second[0] <<  it->second.begin()->first << std::endl;
			
        }
    }
}