#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Make COUNT a compile-time constant
static const int COUNT = 8;

// Forward declarations
void followfirst(char, int, int);
void follow(char c);
void findfirst(char, int, int);

// Global variables
int n = 0;   // Tracks how many FIRST symbols we have collected
int m = 0;   // Tracks how many FOLLOW symbols we have collected

// Arrays for storing FIRST and FOLLOW sets
char calc_first[COUNT][100];
char calc_follow[COUNT][100];

// Production rules array
char production[COUNT][10];

char f[10], first[10];
int k;
char ck;
int e;

int main(int argc, char** argv)
{
    int jm = 0;
    int km = 0;
    int i;
    char c;

    // Use strcpy_s instead of strcpy
    strcpy_s(production[0], sizeof(production[0]), "X=TnS");
    strcpy_s(production[1], sizeof(production[1]), "X=Rm");
    strcpy_s(production[2], sizeof(production[2]), "T=q");
    strcpy_s(production[3], sizeof(production[3]), "T=#");
    strcpy_s(production[4], sizeof(production[4]), "S=p");
    strcpy_s(production[5], sizeof(production[5]), "S=#");
    strcpy_s(production[6], sizeof(production[6]), "R=om");
    strcpy_s(production[7], sizeof(production[7]), "R=ST");

    // Arrays to track which nonterminals have been processed
    char done[COUNT];
    char donee[COUNT];

    // Initialize calc_first array
    for (k = 0; k < COUNT; k++) {
        for (int kay = 0; kay < 100; kay++) {
            calc_first[k][kay] = '!';
        }
    }

    int ptr = -1;
    int point1 = 0, point2, xxx;

    // Calculate FIRST sets
    for (k = 0; k < COUNT; k++) {
        c = production[k][0];
        point2 = 0;
        xxx = 0;

        // Check if FIRST(c) is already calculated
        for (int kay = 0; kay <= ptr; kay++) {
            if (c == done[kay]) {
                xxx = 1;
                break;
            }
        }
        if (xxx == 1) {
            continue;
        }

        // Calculate FIRST(c)
        findfirst(c, 0, 0);
        ptr += 1;
        done[ptr] = c;

        // Print results
        printf("\n First(%c) = { ", c);
        calc_first[point1][point2++] = c;

        for (i = jm; i < n; i++) {
            int lark, chk = 0;
            for (lark = 0; lark < point2; lark++) {
                if (first[i] == calc_first[point1][lark]) {
                    chk = 1;
                    break;
                }
            }
            if (!chk) {
                printf("%c, ", first[i]);
                calc_first[point1][point2++] = first[i];
            }
        }
        printf("}\n");

        jm = n;
        point1++;
    }

    printf("\n");
    printf("-----------------------------------------------\n\n");

    // Initialize calc_follow array
    for (k = 0; k < COUNT; k++) {
        for (int kay = 0; kay < 100; kay++) {
            calc_follow[k][kay] = '!';
        }
    }

    ptr = -1;
    point1 = 0;
    int land = 0;

    // Calculate FOLLOW sets
    for (e = 0; e < COUNT; e++) {
        ck = production[e][0];
        point2 = 0;
        xxx = 0;

        // Check if FOLLOW(ck) is already calculated
        for (int kay = 0; kay <= ptr; kay++) {
            if (ck == donee[kay]) {
                xxx = 1;
                break;
            }
        }
        if (xxx == 1) {
            continue;
        }
        land += 1;

        // Calculate FOLLOW(ck)
        follow(ck);
        ptr += 1;
        donee[ptr] = ck;

        // Print results
        printf(" Follow(%c) = { ", ck);
        calc_follow[point1][point2++] = ck;

        for (i = km; i < m; i++) {
            int lark, chk = 0;
            for (lark = 0; lark < point2; lark++) {
                if (f[i] == calc_follow[point1][lark]) {
                    chk = 1;
                    break;
                }
            }
            if (!chk) {
                printf("%c, ", f[i]);
                calc_follow[point1][point2++] = f[i];
            }
        }
        printf(" }\n\n");

        km = m;
        point1++;
    }

    return 0;
}

// FOLLOW computation
void follow(char c)
{
    // If c is the start symbol, add '$'
    if (production[0][0] == c) {
        f[m++] = '$';
    }

    // Check each production
    for (int i = 0; i < COUNT; i++) {
        for (int j = 2; j < 10; j++) {
            if (production[i][j] == c) {
                if (production[i][j + 1] != '\0') {
                    // Compute FIRST of the next symbol
                    followfirst(production[i][j + 1], i, j + 2);
                }
                // If c is at the end, compute FOLLOW of LHS
                if (production[i][j + 1] == '\0' && c != production[i][0]) {
                    follow(production[i][0]);
                }
            }
        }
    }
}

// FIRST computation
void findfirst(char c, int q1, int q2)
{
    // If terminal, add to FIRST
    if (!isupper(c)) {
        first[n++] = c;
        return;
    }

    // Otherwise, explore productions of c
    for (int j = 0; j < COUNT; j++) {
        if (production[j][0] == c) {
            // Epsilon
            if (production[j][2] == '#') {
                if (production[q1][q2] == '\0') {
                    first[n++] = '#';
                }
                else if (production[q1][q2] != '\0' && (q1 != 0 || q2 != 0)) {
                    findfirst(production[q1][q2], q1, q2 + 1);
                }
                else {
                    first[n++] = '#';
                }
            }
            // If next symbol is a terminal
            else if (!isupper(production[j][2])) {
                first[n++] = production[j][2];
            }
            else {
                // Recurse on the non-terminal
                findfirst(production[j][2], j, 3);
            }
        }
    }
}

// Helper for FOLLOW to get FIRST of next
void followfirst(char c, int c1, int c2)
{
    // If terminal, add directly
    if (!isupper(c)) {
        f[m++] = c;
    }
    else {
        // Find the FIRST set row for this non-terminal 'c'
        int i, j = 1;
        for (i = 0; i < COUNT; i++) {
            if (calc_first[i][0] == c)
                break;
        }

        // Add each symbol of FIRST(c)
        while (calc_first[i][j] != '!') {
            if (calc_first[i][j] != '#') {
                f[m++] = calc_first[i][j];
            }
            else {
                // If epsilon (#), move to next symbol or do FOLLOW of LHS
                if (production[c1][c2] == '\0') {
                    follow(production[c1][0]);
                }
                else {
                    followfirst(production[c1][c2], c1, c2 + 1);
                }
            }
            j++;
        }
    }
}
