/******************************************************
** Program name: messagePrint.cpp
** CS435 Cloud Networking: MP2
** Date: 3/4/2024
** Description: Source file for messagePrint functions.
*******************************************************/

#include "../HEADERS/messagePrint.hpp"

void messagePrint(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::istringstream& messagesStr) {
    
    std::cout << std::endl;
    
    auto str = std::string{};
    while (!messagesStr.eof()) {
        std::string line, message;
        getline(messagesStr, line);
        //std::cout << str << std::endl;
        int sourceNode, destNode, cost;
        std::istringstream messageStr(line);
        messageStr >> sourceNode >> destNode;
        getline(messageStr, message);
        //std::cout << "Source Node: " << sourceNode << " Destination Node: " << destNode << " Message :" << message << std::endl;
        if(_FT[sourceNode].find(destNode) != _FT[sourceNode].end()) {
			cost = _FT[sourceNode].find(destNode)->second.begin()->first;
			std::vector<int> lowestCostPath(_FT[sourceNode].find(destNode)->second.begin()->second);
			if (lowestCostPath.size() == 1) {
				std::cout << "from " << sourceNode << " to " << destNode << " cost " << cost << " hops " << sourceNode;
			}
			else {
				std::cout << "from " << sourceNode << " to " << destNode << " cost " << cost << " hops " << sourceNode << " ";
				std::vector<int> lowestCostPath(_FT[sourceNode].find(destNode)->second.begin()->second);
				if(lowestCostPath.size() > 1) {
					for(std::vector<int>::iterator it = lowestCostPath.begin(); it != lowestCostPath.end()-1; it++) {
						if(it == lowestCostPath.end()-2) {
							std::cout << *it;
						}
						else {
							std::cout << *it << " ";
						}
					}
				}
				//std::cout << std::endl;
			}
			std::cout << message << std::endl;
		}
		else {
			std::cout << "from " << sourceNode << " to " << destNode << " cost infinite hops unreachable message" << message << std::endl;
		}
    }   
    std::cout << std::endl;
}

void messagePrint(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::istringstream& messagesStr, std::ofstream& _outFile) {
    
    std::cout << std::endl;
    
    auto str = std::string{};
    while (!messagesStr.eof()) {
        std::string line, message;
        getline(messagesStr, line);
        int sourceNode, destNode, cost;
        std::istringstream messageStr(line);
        messageStr >> sourceNode >> destNode;
        getline(messageStr, message);
        if(_FT[sourceNode].find(destNode) != _FT[sourceNode].end()) {
			cost = _FT[sourceNode].find(destNode)->second.begin()->first;
			std::vector<int> lowestCostPath(_FT[sourceNode].find(destNode)->second.begin()->second);
			if (lowestCostPath.size() == 1) {
				_outFile << "from " << sourceNode << " to " << destNode << " cost " << cost << " hops " << sourceNode;
			}
			else {
				_outFile << "from " << sourceNode << " to " << destNode << " cost " << cost << " hops " << sourceNode << " ";
				std::vector<int> lowestCostPath(_FT[sourceNode].find(destNode)->second.begin()->second);
				if(lowestCostPath.size() > 1) {
					for(std::vector<int>::iterator it = lowestCostPath.begin(); it != lowestCostPath.end()-1; it++) {
						if(it == lowestCostPath.end()-2) {
							_outFile << *it;
						}
						else {
							_outFile << *it << " ";
						}
					}
				}
			}
			_outFile << message << std::endl;
		}
		else {
			_outFile << "from " << sourceNode << " to " << destNode << " cost infinite hops unreachable message" << message << std::endl;
		}
    }   
    _outFile << std::endl;
}

void messagePrint(std::vector<std::map<int, std::multimap<int, std::vector<int>>>> &_FT, std::ifstream& messages, std::ofstream& _outFile) {
    
    std::cout << std::endl;
    
    auto str = std::string{};
    while (!messages.eof()) {
        std::string line, message;
        getline(messages, line);
		//std::cout << "Line in messagePrint: " << line << "Line is Empty Check: " << line.empty() << std::endl;
        if((!line.empty()) && (std::find_if_not(line.begin(),line.end(),std::isspace) != line.end())
		int sourceNode, destNode, cost;
        std::istringstream messageStr(line);
        messageStr >> sourceNode >> destNode;
        getline(messageStr, message);
        if(_FT[sourceNode].find(destNode) != _FT[sourceNode].end()) {
			cost = _FT[sourceNode].find(destNode)->second.begin()->first;
			std::vector<int> lowestCostPath(_FT[sourceNode].find(destNode)->second.begin()->second);
			if (lowestCostPath.size() == 1) {
				_outFile << "from " << sourceNode << " to " << destNode << " cost " << cost << " hops " << sourceNode;
			}
			else if (lowestCostPath.size() > 1) {
				_outFile << "from " << sourceNode << " to " << destNode << " cost " << cost << " hops " << sourceNode << " ";
				std::vector<int> lowestCostPath(_FT[sourceNode].find(destNode)->second.begin()->second);
				for(std::vector<int>::iterator it = lowestCostPath.begin(); it != lowestCostPath.end()-1; it++) {
					if(it == lowestCostPath.end()-2) {
						_outFile << *it;
					}
					else {
						_outFile << *it << " ";
					}
				}
			}
			_outFile << " message" << message << std::endl;
		}
		else {
			_outFile << "from " << sourceNode << " to " << destNode << " cost infinite hops unreachable message" << message << std::endl;
		}
    }   
    _outFile << std::endl;
	
	//Rewind input stream to beginning for next print job...
	messages.clear();
	messages.seekg(0);
}