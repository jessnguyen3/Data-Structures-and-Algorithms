// COMP2521 20T2 Assignment 1
// Dict.c ... implementsation of Dictionary ADT
// Written by z5311696

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Dict.h"
#include "WFreq.h"

typedef struct _DictNode *Link;

typedef struct  _DictNode {
   WFreq  data;
   Link   left;
   Link   right;
   int height;
} DictNode;

struct _DictRep {
   Link tree;
};

// My functions:
Link balancedInsertion(Dict d, Link n, char *w);
Link rotateLeft(Dict d, Link curr);
Link rotateRight(Dict d, Link curr);
int height(Link curr);
int max(int a, int b);
Link searchWord(Link curr, char *w);
int insertIntoArray(WFreq *wfs, WFreq inputPair, int n, int numInserted);
int cmpFreq(const void *pair1, const void *pair2);
int inorderTraversal(Link curr, WFreq *wfs, int n, int index);

// create new empty Dictionary
Dict newDict()
{
   struct _DictRep *d = malloc(sizeof(*d));
   d->tree = NULL;
   return d;
}

// creates new word to insert into the dictionary
Link newWord(char *w) 
{
    Link new = malloc(sizeof(*new));
    new->data.word = malloc(strlen(w) + 1);
    strcpy(new->data.word, w);
    new->data.freq = 0;
    new->left = NULL;
    new->right = NULL;
    new->height = 0;
    return new;
}

// insert new word into Dictionary
// return pointer to the (word,freq) pair for that word
WFreq *DictInsert(Dict d, char *w)
{
    d->tree = balancedInsertion(d, d->tree, w);
    return DictFind(d, w);
}

// Code adapted from lecture slides pseudo code & COMP2521 20T2 Lab 04
// Inserts new word into dictionary and returns
// a pointer to the new word inserted.
Link balancedInsertion(Dict d, Link curr, char *w) 
{
    // Base case
    if (curr == NULL) return newWord(w);
    // Find and Insert into dictionary - recursively.
    int cmp = strcmp(w, curr->data.word);
    if (cmp < 0) {
        curr->left = balancedInsertion(d, curr->left, w);
    } else if (cmp > 0) {
        curr->right = balancedInsertion(d, curr->right, w);
    }
    
    // Insertion completed.
    // Now required to correct the height of the current subtree
    curr->height = max(height(curr->left), height(curr->right)) + 1;
    int leftHeight = height(curr->left);
    int rightHeight = height(curr->right);
    // Rebalancing the given subtree.
    if ((leftHeight - rightHeight) > 1) {
        if (strcmp(w, curr->left->data.word) > 0) {
            curr->left = rotateLeft(d, curr->left);
        }
        curr = rotateRight(d, curr);
    } 
    else if ((rightHeight - leftHeight) > 1) {
        if (strcmp(w, curr->right->data.word) < 0) {
            curr->right = rotateRight(d, curr->right);
            }
        curr = rotateLeft(d, curr);
    }
    return curr;
}

// Code adapted from lecture slides pseudo code & COMP2521 20T2 Lab 04
// Rotates  the  given  subtree left and returns the root of the updated
// subtree.
Link rotateLeft(Dict d, Link curr) 
{
    // Rotation cannot be performed
    if (curr == NULL || curr->right == NULL) return curr;
    
    Link oldRoot = curr;
    curr = curr->right; 
    // Make the right node the new root node (if oldRoot was d->tree)
    if (d->tree == oldRoot) d->tree = curr;
    oldRoot->right = curr->left;
    curr->left = oldRoot;
    
    // Updating the heights
    curr->left->height = max(height(curr->left->left), height(curr->left->right)) + 1;
    curr->height = max(height(curr->left), height(curr->right)) + 1;

    return curr;
}

// Code adapted from lecture slides pseudo code & COMP2521 20T2 Lab 04
// Rotates the given subtree right and returns the root of  the  updated
// subtree.
Link rotateRight(Dict d, Link curr) 
{
    if (curr == NULL || curr->left == NULL) return curr;
    
    Link oldRoot = curr;
    curr = curr->left; 
    // Make the left node the new root node(if oldRoot was d->tree)
    if (d->tree == oldRoot) d->tree =curr;
    oldRoot->left = curr->right;
    curr->right = oldRoot;
    
    // Update the heights
    curr->right->height = max(height(curr->right->left), height(curr->right->right)) + 1;
    curr->height = max(height(curr->left), height(curr->right)) + 1;
    
    return curr;
}

// Height function used to ensure program is not accessing NULL memory.
int height(Link curr) 
{
    if (curr == NULL) {
        return -1;
    } else {
        return curr->height;
    }
}

// Used to determine maximum between two integers.
int max(int a, int b) 
{   
    if (a > b) return a;
    return b;
}

// find Word in Dictionary
// return pointer to (word,freq) pair, if found
// otherwise return NULL
WFreq *DictFind(Dict d, char *w)
{
    if (d->tree == NULL) return NULL;
    Link n = searchWord(d->tree, w);
    if (n == NULL) return NULL; // Word not found in the dictionary.
    return &(n->data);
}

// Gets a node, traverses through BSTree to find given word
// Returns pointer to node with (word, freq) pair if found
// Return NULL if not found
Link searchWord(Link curr, char *w) 
{
    // Base case
    if (curr == NULL) return NULL;
    // Recursively search tree.
    if (strcmp(w, curr->data.word) == 0) return curr;
    else if (strcmp(w, curr->data.word) > 0) return searchWord(curr->right, w);
    return searchWord(curr->left, w);
}

// find top N frequently occurring words in Dict
// input: Dictionary, array of WFreqs, size of array
// returns: #WFreqs in array, modified array
int findTopN(Dict d, WFreq *wfs, int n)
{
    int numInserted = 0; // number of elements inserted into array
    numInserted = inorderTraversal(d->tree, wfs, n, numInserted);
    
    if (numInserted < n) return numInserted; // total no# of dictionary words less than n 
    return n;
}

// Insert word pair into array in the correct position. 
int insertIntoArray(WFreq *wfs, WFreq inputPair, int n, int numInserted) 
{
    if (numInserted < n) {
        wfs[numInserted] = inputPair; // insert at the last available spot in array
        numInserted++;
        if (numInserted > 1) qsort(wfs, numInserted, sizeof(WFreq), cmpFreq);
    } else if (wfs[n - 1].freq < inputPair.freq) { // min frequency at end of array
            wfs[n - 1] = inputPair; // insert at the end of the array
            qsort(wfs, numInserted, sizeof(WFreq), cmpFreq); // insert new-input into correct position
    }   
    return numInserted;
}

// Compare the frequencies of two WFreq(pair)s
// If pair1 is larger than pair2, pair1 will be in front of pair2
// If pair2 is larger than pair1, pair2 will be in front of pair1
// Compare function ensures qsort will arrange array such that
//larger frequencies are moved to the front of the array.
int cmpFreq(const void *pair1, const void *pair2) 
{
    return ((WFreq *)pair2)->freq - ((WFreq *)pair1)->freq;
}

// Traverse tree inorder so WFreq pairs in array
// will be in lexicographic order and hence, 
// required to only compare frequencies.
// numInserted only affected by the insertIntoArray function
int inorderTraversal(Link curr, WFreq *wfs, int n, int numInserted) 
{
    if (curr == NULL) return numInserted;
    numInserted = inorderTraversal(curr->left, wfs, n, numInserted);
    numInserted = insertIntoArray(wfs, curr->data, n, numInserted);
    numInserted = inorderTraversal(curr->right, wfs, n, numInserted);
    return numInserted;
}


// print a dictionary
void showDict(Dict d) {

}

