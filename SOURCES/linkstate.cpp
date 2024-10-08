/*********************************************************************
** Program name: linkstate.cpp
** Class name: CS435
** Author: Ben Fridkis
** Date: 2/28/2024
** Description: Link State routing protocol simulation for CS435 MP2.
**********************************************************************/

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
    printf("Number of arguments: %d\n", argc);
    if (argc != 4) {
        printf("Usage: ./distvec topofile messagefile changesfile\n");
        return -1;
    }
    
    std::cout << "\nBeginning of Program..." << std::endl;
    
	//Topology stored as a vector of maps, with each vector element corresponding to a given node's map of direct links, indexed by destination with value equal to link cost
	std::vector<std::map<int, int>> TT; 
	
    //Forwarding tables stored as vector of maps, with each vector element corresponding to a forwarding table for a given node, and each (map) entry of that table is that node's forwarding entry, indexed by destination and with the resulting pair first element == to the next hop, and second element == path cost
	//std::vector<std::map<int, std::pair<int, int>>> FT, FT_invert; 		//FT_invert is used for the converge function's dijkstra results, since dijkstra essentially builds paths /resolves ties in reverse order. The results of invert will be swapped/inverted to produce the final FT
    std::vector<std::map<int, std::pair<std::vector<int>, int>>> FT;
	
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
    //iterator for topology maps
    std::map<int, int>::iterator it;
	
	std::set<int> nodesAdded;
	
    //std::cout << "Line64..." << std::endl;
    //int i = 0;
    //while (topoInput >> a >> b >> c) {
	//Padded with one extra element so index number can equal node number :)
	//TT.resize(2);
	while (topoInputFile >> a >> b >> c) {
        //std::cout << "i: " << ++i << std::endl;
        //std::cout << "Line68..." << "a: " << a << " b: " << b << " c: " << c << std::endl;
		
		//Add additional entries up to largest number node found in topology.
        ////Padding by one in the front so index number = node number for semantic convenience. :)
        if (a >= FT.size()) { 
			TT.resize(a+1);
			FT.resize(a+1);
			//FT_invert.resize(a+1);
		}
        if (b >= FT.size()) { 
			TT.resize(b+1);
			FT.resize(b+1); 
			//FT_invert.resize(b+1); 
		}
		
        //std::cout << "Line75..." << "FT Size: " << FT.size() << std::endl;
        
        //Load forwarding tables according to imported topology
        
        ////For a's map...
        it = TT[a].find(b);
        //std::cout << "Line81..." << std::endl;
        if (it == TT[a].end()) {
            TT[a].insert(std::make_pair(b,c));
			nodesAdded.insert(a);
        }
        ////For b's map...
        //std::cout << "Line92..." << std::endl;
        it = TT[b].find(a);
        //std::cout << "Line94..." << std::endl;
        if (it == TT[b].end()) {
			TT[b].insert(std::make_pair(a,c));
			nodesAdded.insert(b);
        }
    }
    
    //std::cout << "Line100..." << std::endl;
    
    //Initialize fowarding tables with self-path cost = 0, and node numbers not added with a self-path cost of -1.
    for (int i = 1; i < FT.size(); i++) {
        
        //If node number added/part of topology, add path to self with cost of 0
        if(nodesAdded.find(i) != nodesAdded.end()) {
			FT[i].insert(std::make_pair(i, std::make_pair(std::vector<int>(1,i),0)));
			//FT_invert[i].insert(std::make_pair(i, std::make_pair(i,0)));
		}
		//If a node is not part of the topology, add a self-entry with a cost of -1 (used to omit printing these place holder node numbers to output file)
		else {
			std::cout << "adding -1 entry for this unused node's self cost: " << i << std::endl;
			FT[i].insert(std::make_pair(i, std::make_pair(std::vector<int>(1,i),-1)));
			//FT_invert[i].insert(std::make_pair(i, std::make_pair(i,-1)));
		}
    }
    
    //Print initial loaded TT (testing/troubleshooting only)
    std::cout << std::endl;
    std::cout << "Intial TT" << std::endl;
    std::cout << "---------" << std::endl;
    for (unsigned short i = 1; i < TT.size(); i++) {
        //std::cout << "i: " << i << " " << FT.size() << std::endl;
        for (it=TT[i].begin(); it!=TT[i].end(); it++) {
            std::cout << i << it->first << it->second << std::endl;
        }
    }
    std::cout << std::endl;
    
    //Initial Routing Table Convergences. 
	std::vector<std::pair<int,int>> tiesTracker;	//Used for tracking tied path decision that may need to be swapped/inverted after convergence, to handle the way in which dijkstra's builds paths and assignemt tie breaker rule of last hop node with lowest node number... ex: node 1->4 may have a different path than 4->1 in the event of a tie, and these will need to be swapped to accomodate assignment priority rule of lowest last hop node number.
    for (int sourceNode = 1; sourceNode < FT.size(); sourceNode++) {
        std::cout << std::endl;
        std::cout << "Initial convergence for node: " << sourceNode << std::endl;
        if(FT[sourceNode].find(sourceNode)->second.second != -1) {
			converge(sourceNode, TT, FT, tiesTracker);
		}
	}
	//After converge, need to swap routes between each source and destination to ensure tie breaking rule of lowest last hop node number is followed (due to the fact that dijkstra's algo builds the routes in reverse order, i.e. from destination to source... see converge.cpp)
	//Do this by building a new FT with swapped values from the dijkstra inverted output
	//(If this makes program run too slow, for this assignment/exercise just need to swap the final source / destination next hops for consoleOutFT and print dest path as source's and vis versa in messagePrint...)
	//This doesn't work... need more thought...
	/* for (int sourceNode = 1; sourceNode < FT_invert.size(); sourceNode++) {
		for(auto&& [reachableNode, nextHop_cost_invert] : FT_invert[sourceNode]) {
			if(FT[sourceNode].find(reachableNode) == FT[reachableNode].end()) {
				FT[sourceNode].insert(std::make_pair(reachableNode, std::make_pair(FT_invert[reachableNode].find(sourceNode)->first, nextHop_cost_invert.second)));
			}
			else {
				FT[sourceNode].find(reachableNode)->second.first = FT_invert[reachableNode].find(sourceNode)->first;
				FT[sourceNode].find(reachableNode)->second.second = nextHop_cost_invert.second;
			}
		}
	} */
	
    //Print converged FT (testing/troubleshooting only)
    std::cout << std::endl;
    std::cout << "Forwarding tables after initial converge..." << std::endl;
    //consoleOutFT(FT);
	consoleOutFT(FT);
	//std::cout << "Got past consoleOutFT?" << std::endl;
	
	//fileOutFT(FT, outFile);
	fileOutFT(FT, outFile, true);
    
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
    
    //auto iss = std::istringstream{"2 1 here is a message from 2 to 1\n3 5 this one gets sent from 3 to 5!"};
    //messagePrint(FT, iss);
	
	messagePrint(FT, messagesInputFile, outFile);
    //messagePrint(FT, messagesInputFile, outFile);
	
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
	
	//std::cout << "Did we get to processChanges?" << std::endl;
	
	processChanges(FT, TT, changesInputFile, messagesInputFile, outFile);
    
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
//https://stackoverflow.com/questions/28331017/rewind-an-ifstream-object-after-hitting-the-end-of-file
//https://stackoverflow.com/questions/20583531/lvalue-to-rvalue-reference-binding
//https://stackoverflow.com/questions/14623958/breaking-change-in-c11-with-make-pair-ty1-val1-const-ty2-val2