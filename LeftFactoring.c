#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define MAX_ALTS 50

// Trim leading and trailing whitespace
char* trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0')
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
    return str;
}

// Compute the longest common prefix length between two strings
int longest_common_prefix_length(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] && s2[i] && s1[i] == s2[i])
        i++;
    return i;
}

// Process a single production rule line and write factored results to file
void processProductionLine(char *line, FILE *outputFile) {
    char *trimmedLine = trim(line);
    if (strlen(trimmedLine) == 0)
        return; // Skip empty lines

    char *arrow = strstr(trimmedLine, "->");
    if (!arrow)
        return; // Skip invalid production rules

    *arrow = '\0';
    char *lhs = trim(trimmedLine);
    char *rhs = trim(arrow + 2);

    // Tokenize the right-hand side alternatives using '|'
    char *alternatives[MAX_ALTS];
    int altCount = 0;
    char *token = strtok(rhs, "|");
    while (token && altCount < MAX_ALTS) {
        alternatives[altCount++] = trim(token);
        token = strtok(NULL, "|");
    }
    if (altCount == 0)
        return;

    // Determine the longest common prefix among alternatives
    int prefixLength = strlen(alternatives[0]);
    for (int i = 1; i < altCount; i++) {
        int currentPrefix = longest_common_prefix_length(alternatives[0], alternatives[i]);
        if (currentPrefix < prefixLength)
            prefixLength = currentPrefix;
    }
    
    if (prefixLength == 0)
        return; // Skip this production if no common prefix is found

    char newNonTerminal[50];
    snprintf(newNonTerminal, sizeof(newNonTerminal), "%s'", lhs);

    // Write the left-factored production to the output file
    fprintf(outputFile, "%s -> ", lhs);
    for (int i = 0; i < prefixLength; i++)
        fputc(alternatives[0][i], outputFile);
    fprintf(outputFile, " %s\n", newNonTerminal);

    // Write the new nonterminal's production
    fprintf(outputFile, "%s -> ", newNonTerminal);
    for (int i = 0; i < altCount; i++) {
        int altLen = strlen(alternatives[i]);
        if (altLen == prefixLength) {
            fprintf(outputFile, "ε"); // If alternative exactly matches prefix, use epsilon
        } else {
            fprintf(outputFile, "%s", alternatives[i] + prefixLength);
        }
        if (i < altCount - 1)
            fprintf(outputFile, " | ");
    }
    fprintf(outputFile, "\n\n");
}

int main(void) {
    FILE *inputFile = fopen("CFG.txt", "r");
    if (!inputFile) {
        perror("Error opening file CGF.txt");
        return 1;
    }

    FILE *outputFile = fopen("CFG_LF.txt", "w");
    if (!outputFile) {
        perror("Error opening file CFGNew.txt");
        fclose(inputFile);
        return 1;
    }

    char line[MAX_INPUT];

    // Process each production rule line in the file
    while (fgets(line, sizeof(line), inputFile)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0'; // Remove newline character
        processProductionLine(line, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
    printf("Left-factored grammar has been saved in CFGNew.txt\n");
    
    return 0;
}
