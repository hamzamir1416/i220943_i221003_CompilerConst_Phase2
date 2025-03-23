// first_follow.h
#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

using namespace std;

static const int COUNT = 8;

// Function declarations
void followfirst(char, int, int);
void follow(char c);
void findfirst(char, int, int);
void loadGrammar(const char* filename);
void computeFirstFollow(); 
void displayFirstFollow();

// Global variables
extern int n, m;
extern char calc_first[COUNT][100];
extern char calc_follow[COUNT][100];
extern char production[COUNT][10];
extern char f[10], first[10];
extern int k;
extern char ck;
extern int e;

#endif // FIRST_FOLLOW_H