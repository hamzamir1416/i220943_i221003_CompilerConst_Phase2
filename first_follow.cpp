// first_follow.cpp
#include "first_follow.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>

using namespace std;

int n, m;
char calc_first[COUNT][100];
char calc_follow[COUNT][100];
char production[COUNT][10];
char f[10], first[10];
int k;
char ck;
int e;

void loadGrammar(const char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }
    file >> n;
    for (int i = 0; i < n; i++) {
        file >> production[i];
    }
    file.close();
}

void findfirst(char c, int q1, int q2) {
    if (!isupper(c)) {
        first[k++] = c;
        return;
    }
    for (int j = 0; j < n; j++) {
        if (production[j][0] == c) {
            if (production[j][2] == '$') {
                e = 1;
            }
            else {
                findfirst(production[j][2], q1, q2);
            }
        }
    }
}

void followfirst(char c, int c1, int c2);

void follow(char c) {
    if (production[0][0] == c) {
        f[k++] = '$';
    }
    for (int i = 0; i < n; i++) {
        for (int j = 2; j < strlen(production[i]); j++) {
            if (production[i][j] == c) {
                if (production[i][j + 1] != '\0') {
                    followfirst(production[i][j + 1], i, (j + 2));
                }
                if (production[i][j + 1] == '\0' && c != production[i][0]) {
                    follow(production[i][0]);
                }
            }
        }
    }
}

void followfirst(char c, int c1, int c2) {
    if (!isupper(c)) {
        f[k++] = c;
    }
    else {
        findfirst(c, c1, c2);
        if (e) {
            follow(production[c1][0]);
        }
    }
}

void computeFirstFollow() {
    for (int i = 0; i < n; i++) {
        k = 0;
        findfirst(production[i][0], 0, 0);
        strcpy_s(calc_first[i], first);
    }
    for (int i = 0; i < n; i++) {
        k = 0;
        follow(production[i][0]);
        strcpy_s(calc_follow[i], f);
    }
}

void displayFirstFollow() {
    cout << "First and Follow sets:\n";
    for (int i = 0; i < n; i++) {
        cout << "FIRST(" << production[i][0] << ") = { ";
        for (int j = 0; j < strlen(calc_first[i]); j++) {
            cout << calc_first[i][j] << " ";
        }
        cout << "}\n";

        cout << "FOLLOW(" << production[i][0] << ") = { ";
        for (int j = 0; j < strlen(calc_follow[i]); j++) {
            cout << calc_follow[i][j] << " ";
        }
        cout << "}\n";
    }
}