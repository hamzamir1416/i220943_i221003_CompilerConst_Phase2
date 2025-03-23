#include "first_follow.h" 
#include "ll1_table.h"
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cstring>

using namespace std;

map<char, map<char, string>> parsingTable;

void constructParsingTable() {
    for (int i = 0; i < n; i++) {
        char nonTerminal = production[i][0];
        char* firstSet = calc_first[i];
        char* followSet = calc_follow[i];

        for (int j = 0; j < strlen(firstSet); j++) {
            if (firstSet[j] != '$') {
                parsingTable[nonTerminal][firstSet[j]] = production[i];
            }
            else {
                for (int k = 0; k < strlen(followSet); k++) {
                    parsingTable[nonTerminal][followSet[k]] = "$";
                }
            }
        }
    }
}

void writeParsingTableToFile(const char* filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }
    file << "LL(1) Parsing Table:\n";
    for (auto& row : parsingTable) {
        char nonTerminal = row.first;
        for (auto& col : row.second) {
            file << "M[" << nonTerminal << ", " << col.first << "] = " << col.second << "\n";
        }
    }
    file.close();
}

