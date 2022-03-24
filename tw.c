// COMP2521 20T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File
// Program written by z5311696

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Dict.h"
#include "WFreq.h"
#include "stemmer.h"

#define STOPWORDS "stopwords"
#define MAXLINE 1000
#define MAXWORD 100

#define STARTING "*** START OF"
#define ENDING   "*** END OF"

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

// My #defines
#define YES 1
#define NO -1

int main( int argc, char *argv[])
{
    // TODO ... add any variables you need
    int startFound = NO;

    FILE  *in;         // currently open file
    Dict   stopwords;  // dictionary of stopwords
    Dict   wfreqs;     // dictionary of words from book
    WFreq *results;    // array of top N (word,freq) pairs
                      // (dynamically allocated)

    char *fileName;    // name of file containing book text
    int   nWords;      // number of top frequency words to show

    char   line[MAXLINE];  // current input line
    char   word[MAXWORD];  // current word

    // process command-line args
    switch (argc) {
    case 2:
       nWords = 10;
       fileName = argv[1];
       break;
    case 3:
       nWords = atoi(argv[1]);
       if (nWords < 10) nWords = 10;
       fileName = argv[2];
       break;
    default:
       fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
       exit(EXIT_FAILURE);
    }
    
    // Check if stopwords File exists and can be open
    in = fopen(STOPWORDS, "r");
    if (in == NULL) {
        fprintf(stderr, "Can't open stopwords\n");
        exit(EXIT_FAILURE);
    }
    
    // build stopword dictionary -- via tree
    // i.e. insert, find etc.
    stopwords = newDict();
    while (fgets(line, MAXLINE, in) != NULL) {
        int j = 0;
        for (int i = 0; line[i] != '\0'; i++, j++) {
            if (line[i] == '\n') word[i] = '\0'; // read until end of word
            else word[i] = tolower(line[j]); // add character to array
        }
        struct _WFreq *inputWord = DictInsert(stopwords, word);
        inputWord->freq++;
    }
    fclose(in);
    
    // Check if textfile exists and can be open
    in = fopen(fileName, "r");
    if (in == NULL) {
        fprintf(stderr, "Can't open %s\n",fileName);
        exit(EXIT_FAILURE);
    }
    
    // Scan File, up to start of text
    while (fgets(line, MAXLINE, in) != NULL && strncmp(line, ENDING, 10) != 0) {
        if (strncmp(line, STARTING, 12) == 0) {
            startFound = YES;
            break;
        }  
    }
    // if "*** START OF" does not exist 
    if (startFound == NO) {
        fprintf(stderr, "Not a Project Gutenberg book\n");
        exit(EXIT_FAILURE);
    }
    
    wfreqs = newDict();
    while (fgets(line, MAXLINE, in) != NULL && strncmp(line, ENDING, 10) != 0) {
        int j = 0;
        if (line[0] == '\n') continue; // Line read is only a newline.
        for (int i = 0; line[j] != '\0'; i++, j++) {  
            if (isWordChar(line[j])) {
                word[i] = tolower(line[j]);
            } else { // Read until end of word OR invalid character read in.
                word[i] = '\0'; // Terminate word array so strlen can be  
                                // used to find length of the word.
                if (strlen(word) < 2) { // a single character or less read in.
                    i = -1;
                    continue; 
                }
                if (DictFind(stopwords, word) == NULL) { // Not a stopword, apply stemmer
                    int k = stem(word, 0, strlen(word) - 1);
                    word[k + 1] = '\0';
                } else { // Is a stopword, cannot be inserted into dictionary.
                    i = -1;
                    continue; 
                }
                struct _WFreq *inputWord = DictFind(wfreqs, word);
                // Guarantees DictInsert given a word NOT already in dictionary
                if (inputWord == NULL) {
                    // New word added to dictionary.
                    inputWord = DictInsert(wfreqs, word);
                }
                inputWord->freq++; // Increment frequency of word.
                i = -1; // To overwrite current array with new word 
            } 
        } 
    }
    fclose(in);
    
    results = malloc(nWords * sizeof(WFreq));
    // Compute top N most frequent words in dictionary
    int nFound = findTopN(wfreqs, results, nWords);
    // Print the top N most frequent words
    for (int index = 0; index < nFound; index++) {
        printf("%7d %s\n", results[index].freq, results[index].word);
    }
    
    return EXIT_SUCCESS;
} 
