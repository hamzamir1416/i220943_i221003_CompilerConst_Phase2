//// main.cpp
#include "first_follow.h"
#include "ll1_table.h"
#include <iostream>

using namespace std;

int main() {
    // Load the grammar from file
    loadGrammar("grammar.txt");

    // Compute First and Follow sets
    computeFirstFollow();

    // Display First and Follow sets
    displayFirstFollow();

    // Construct and write LL(1) Parsing Table
    constructParsingTable();
    writeParsingTableToFile("output.txt");

    cout << "Parsing table has been written to output.txt" << endl;

    return 0;
}