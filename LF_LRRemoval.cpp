#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define MAX_ALTS 50


FILE * inputFile, *outputFile;
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
void openFiles() {
    inputFile = fopen("CFG.txt", "r");
    if (!inputFile) {
        perror("Error opening file CFG.txt");
        exit(1);
    }

    outputFile = fopen("CFG_LF.txt", "w");
    if (!outputFile) {
        perror("Error opening file CFG_LF.txt");
        fclose(inputFile);
        exit(1);
    }
}

void processFile_LF() {
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
    printf("Left-factored grammar has been saved in CFG_LF.txt\n");
    
}

// Trim leading and trailing whitespace from a string.


// Check if the alternative is left recursive (i.e. starts with the nonterminal).
// We use a simple check: if alternative begins with lhs, then it is considered left recursive.
int isLeftRecursive(const char *lhs, const char *alternative) {
    size_t lhsLen = strlen(lhs);
    // Compare the first lhsLen characters of alternative with lhs.
    if (strncmp(alternative, lhs, lhsLen) == 0) {
        // Ensure that the alternative either ends or the next character is a delimiter (space)
        // so that we don't confuse, e.g., "A" and "Ab".
        char next = alternative[lhsLen];
        if (next == '\0' || isspace((unsigned char)next))
            return 1;
    }
    return 0;
}

// Process a single production rule line, remove immediate left recursion if present,
// and write the result to outputFile.
void processProductionLineLR(char *line, FILE *outputFile) {
    char *trimmedLine = trim(line);
    if (strlen(trimmedLine) == 0)
        return;  // Skip empty lines.

    // Look for the "->" separator.
    char *arrow = strstr(trimmedLine, "->");
    if (!arrow)
        return;  // Skip invalid production rules.
    
    // Split the production into LHS and RHS.
    *arrow = '\0';
    char *lhs = trim(trimmedLine);
    char *rhs = trim(arrow + 2);

    // Tokenize the right-hand side alternatives using '|'.
    char *alternatives[MAX_ALTS];
    int altCount = 0;
    char *token = strtok(rhs, "|");
    while (token && altCount < MAX_ALTS) {
        alternatives[altCount++] = trim(token);
        token = strtok(NULL, "|");
    }
    if (altCount == 0)
        return;
    
    // Partition alternatives into left-recursive and non-left-recursive groups.
    char *leftRecAlts[MAX_ALTS];
    int leftRecCount = 0;
    char *nonLeftRecAlts[MAX_ALTS];
    int nonLeftRecCount = 0;

    for (int i = 0; i < altCount; i++) {
        if (isLeftRecursive(lhs, alternatives[i]))
            leftRecAlts[leftRecCount++] = alternatives[i];
        else
            nonLeftRecAlts[nonLeftRecCount++] = alternatives[i];
    }

    // If no left recursion is found, write the production unchanged.
    if (leftRecCount == 0) {
        fprintf(outputFile, "%s -> ", lhs);
        for (int i = 0; i < altCount; i++) {
            fprintf(outputFile, "%s", alternatives[i]);
            if (i < altCount - 1)
                fprintf(outputFile, " | ");
        }
        fprintf(outputFile, "\n\n");
        return;
    }

    // Create a new nonterminal by appending a prime (') to the original LHS.
    char newNonTerminal[50];
    snprintf(newNonTerminal, sizeof(newNonTerminal), "%s'", lhs);

    // Remove immediate left recursion.
    // For each non-left-recursive alternative: A -> β becomes: β newNonTerminal
    // For each left-recursive alternative: A -> Aα becomes: α newNonTerminal
    // and then add an alternative for newNonTerminal: ε (epsilon).
    
    // Write the new production for the original nonterminal.
    // Check if there is at least one non-left-recursive alternative.
    if (nonLeftRecCount == 0) {
        // In the rare case that all alternatives are left recursive,
        // we cannot remove left recursion by the standard method.
        // Here, we choose to output an error message or skip the rule.
        fprintf(outputFile, "// Cannot remove left recursion from %s as all alternatives are left recursive.\n\n", lhs);
        return;
    }
    
    fprintf(outputFile, "%s -> ", lhs);
    for (int i = 0; i < nonLeftRecCount; i++) {
        // Append the new nonterminal after each non-left-recursive alternative.
        fprintf(outputFile, "%s %s", nonLeftRecAlts[i], newNonTerminal);
        if (i < nonLeftRecCount - 1)
            fprintf(outputFile, " | ");
    }
    fprintf(outputFile, "\n");

    // Write the new production for the new nonterminal.
    fprintf(outputFile, "%s -> ", newNonTerminal);
    for (int i = 0; i < leftRecCount; i++) {
        // For left recursive alternatives of the form A -> A α,
        // remove the lhs (including any extra whitespace) from the beginning.
        size_t lhsLen = strlen(lhs);
        // Copy the remainder after lhs.
        char *alpha = alternatives[i] + lhsLen;
        alpha = trim(alpha);
        fprintf(outputFile, "%s %s", alpha, newNonTerminal);
        if (i < leftRecCount - 1)
            fprintf(outputFile, " | ");
    }
    // Add the epsilon alternative.
    fprintf(outputFile, " | ε\n\n");
}

// Process the CFG from inputFile and write the final CFG to outputFile.
void processCFG(FILE *inputFile, FILE *outputFile) {
    char line[MAX_INPUT];
    while (fgets(line, sizeof(line), inputFile)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
            line[len-1] = '\0'; // Remove the newline.
        processProductionLineLR(line, outputFile);
    }
}

void leftRecursionDriver()
{
    FILE *inputFile = fopen("CFG_LF.txt", "r");
    if (!inputFile) {
        perror("Error opening file CFG_LF.txt");
        return ;
    }

    FILE *outputFile = fopen("CFG_LR.txt", "w");
    if (!outputFile) {
        perror("Error opening file CFG_LR.txt");
        fclose(inputFile);
        return ;
    }

    processCFG(inputFile, outputFile);
    
    fclose(inputFile);
    fclose(outputFile);
    
    printf("CFG with left recursion removed has been saved in CFG_LR.txt\n");

}
int main(void) {
   
    //opens the Files
    openFiles();
    // Process each production rule line in the file
    processFile_LF();//Fixed Grammer saved into CFG_LF.txt
    leftRecursionDriver();//Left Recursion Removed Grammer saved into CFG_LR.txt


    
    
    return 0;
}
