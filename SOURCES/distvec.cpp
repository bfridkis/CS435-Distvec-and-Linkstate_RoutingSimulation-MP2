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

#include "../HEADERS/include.hpp"

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
	
	//Output file stream
	std::ofstream outFile("output.txt");
	
	std::ifstream topoInputFile(argv[1]);
	std::ifstream messagesInputFile(argv[2]);
	std::ifstream changesInputFile(argv[3]);
    
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
    //while (topoInput >> a >> b >> c) {
	while (topoInputFile >> a >> b >> c) {
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
	
	fileOutFT(FT, outFile);
    
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
	
	messagePrint(FT, messagesInputFile, outFile);
    
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
    
    //processChanges(FT, changeInput);
	
	processChanges(FT, changesInputFile, messagesInputFile, outFile);
    
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