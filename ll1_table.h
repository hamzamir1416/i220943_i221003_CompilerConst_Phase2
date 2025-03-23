// ll1_table.h
#ifndef LL1_TABLE_H
#define LL1_TABLE_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <cstring>
using namespace std;

extern map<char, map<char, string>> parsingTable;

void constructParsingTable();
void writeParsingTableToFile(const char* filename);

#endif // LL1_TABLE_H
