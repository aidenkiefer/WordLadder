//Aiden Kiefer
//Fall 2023
//CS211


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct WordNode_struct {
    char* myWord;
    struct WordNode_struct* next; 
} WordNode;

typedef struct LadderNode_struct {
    WordNode* topWord;
    struct LadderNode_struct* next; 
} LadderNode;

int countWordsOfLength(char* filename, int wordSize) { 
    // opens file with filename, returns count of 
    //words with size wordSize 

    int count = 0;
    char line[100];

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char* token = strtok(line, " \t\n");  //split line into words
        while (token != NULL) {
            if (strlen(token) == wordSize) {
                count++;  //increment count if word size matches
            }
            token = strtok(NULL, " \t\n");  //get next word
        }
    }

    fclose(file);

    return count;
}

bool buildWordArray(char* filename, char** words, int numWords, int wordSize) { 
    char line[100];
    int wordsFound = 0;

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char* token = strtok(line, " \t\n");  
        while (token != NULL) {
            if (strlen(token) == wordSize) {
                words[wordsFound] = strdup(token);  //allocate memory + copy word
                wordsFound++;  //update words found
            }
            token = strtok(NULL, " \t\n");  //get next word
        }
    }

    fclose(file);

    return wordsFound == numWords;
}

int findWord(char** words, char* aWord, int loInd, int hiInd) { 
    int lo = loInd;
    int hi = hiInd;
    int mid;

    while (lo <= hi) {
        mid = lo + (hi - lo) / 2;

        int comp = strcmp(words[mid], aWord);

        if (comp == 0) {
            return mid;
        }
        else if (comp < 0) {
            lo = mid + 1;
        }
        else if (comp > 0) {
            hi = mid - 1;
        }
    }
    return -1; 
}

void freeWords(char** words, int numWords) {
    for (int i = 0; i < numWords; ++i) {
        free(words[i]);
    }

    free(words);
}

void insertWordAtFront(WordNode** ladder, char* newWord) {
    WordNode* front = (WordNode*)malloc(sizeof(WordNode));

    front->myWord = newWord;
    front->next = *ladder;
    *ladder = front;
}

int getLadderHeight(WordNode* ladder) {
    int height = 0;
    WordNode* curr = ladder;

    while (curr != NULL){
        height++;
        curr = curr->next;
    }

    return height;
}

WordNode* copyLadder(WordNode* ladder) {
    WordNode* new = NULL;
    WordNode* tail = NULL;
    WordNode* curr = ladder;

    while(curr != NULL) {
        WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));
        newNode->myWord = curr->myWord;
        newNode->next = NULL;

        if (new == NULL) {
            new = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
        curr = curr->next;
    }
    return new; 
}

void freeLadder(WordNode* ladder) {
    while (ladder != NULL){
        WordNode* temp = ladder;
        ladder = ladder->next;
        free(temp);
    }
}

void insertLadderAtBack(LadderNode** list, WordNode* newLadder) {
    LadderNode* new = (LadderNode*)malloc(sizeof(LadderNode));
    new->topWord = newLadder;
    new->next = NULL;
    
    if (*list == NULL) {
        *list = new;
    }
    else {
        LadderNode* curr = *list;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new;
    }
}

WordNode* popLadderFromFront(LadderNode** list) {
    WordNode* popped = NULL;

    if (*list == NULL) {
        return NULL;
    }
    else {
        LadderNode* pop = *list;
        *list = (*list)->next;
        if (pop != NULL) {
            popped = pop->topWord;
            free(pop);
        }
    }   
    return popped; 
}

void freeLadderList(LadderNode* myList) {
    while (myList != NULL) {
        LadderNode* next = myList->next;
        freeLadder(myList->topWord);
        free(myList);
        myList = next;
    }
}

bool areNeighbors(const char *word1, const char *word2) {
    int diffCount = 0;
    
    if (strlen(word1) != strlen(word2)) {
        return false;
    }

    for (int i = 0; i < strlen(word1); i++) {
        if (word1[i] != word2[i]) {
            diffCount++;
        }

        if (diffCount > 1) {
            return false;
        }
    }

    return diffCount == 1;
}

WordNode* findShortestWordLadder(   char** words, bool* usedWord, int numWords, int wordSize, char* startWord, char* finalWord ) {
    LadderNode* myList = NULL; //create empty linked list of ladders
    WordNode* myLadder = NULL; //create empty ladder

    insertWordAtFront(&myLadder, startWord); //insert start word at front of ladder
    insertLadderAtBack(&myList, myLadder); //insert myLadder list at back of myList

    while (myList != NULL) {
        //pop head LadderNode off front of myList
        myLadder = popLadderFromFront(&myList);

        //get top word
        char* topWord = myLadder->myWord;

        //iterate through all words in array
        for (int i = 0; i < numWords; i++) {
            char* currWord = words[i];

            //check if currWord is neighbor of topWord
            if (areNeighbors(topWord, currWord)) {
                //check if it has been used
                if (!usedWord[i]) {
                    //check if currWord is final word
                    if (strcmp(currWord, finalWord) == 0) {
                        //shortest ladder achieved, prepend finalWord to myLadder
                        insertWordAtFront(&myLadder, finalWord);

                        return myLadder;
                    }
                    else {
                        //create copy of myLadder and add currWord to it
                        WordNode* newLadder = copyLadder(myLadder);
                        insertWordAtFront(&newLadder, currWord);

                        //append new ladder to back of myList
                        insertLadderAtBack(&myList, newLadder);
                        usedWord[i] = true;
                    }
                }
            }
        }
    }
    //no ladder was found
    freeLadder(myLadder);
    freeLadderList(myList);
    return NULL;
}

// interactive user-input to set a word; 
//  ensures sure the word is in the dictionary word array
void setWord(char** words, int numWords, int wordSize, char* aWord) {
    bool valid = false;
    printf("  Enter a %d-letter word: ", wordSize);
    int count = 0;
    while (!valid) {
        scanf("%s",aWord);
        count++;
        valid = (strlen(aWord) == wordSize);
        if (valid) {
            int wordInd = findWord(words, aWord, 0, numWords-1);
            if (wordInd < 0) {
                valid = false;
                printf("    Entered word %s is not in the dictionary.\n",aWord);
                printf("  Enter a %d-letter word: ", wordSize); 
            }
        } else {
            printf("    Entered word %s is not a valid %d-letter word.\n",aWord,wordSize);
            printf("  Enter a %d-letter word: ", wordSize);
        }
        if (!valid && count >= 5) { //too many tries, picking random word
            printf("\n");
            printf("  Picking a random word for you...\n");
            strcpy(aWord,words[rand()%numWords]);
            printf("  Your word is: %s\n",aWord);
            valid = true;
        }
    }
}

// helpful debugging function to print a single Ladder
void printLadder(WordNode* ladder) {
    WordNode* currNode = ladder;
    while (currNode != NULL) {
        printf("\t\t\t%s\n",currNode->myWord);
        currNode = currNode->next;
    }
}

// helpful debugging function to print the entire list of Ladders
void printList(LadderNode* list) {
    printf("\n");
    printf("Printing the full list of ladders:\n");
    LadderNode* currList = list;
    while (currList != NULL) {
        printf("  Printing a ladder:\n");
        printLadder(currList->topWord);
        currList = currList->next;
    }
    printf("\n");
}


//-----------------------------------------------------
// The primary application is fully provided in main(); 
//  no changes should be made to main()
//-----------------------------------------------------
int main() {
    srand((int)time(0));
    
    printf("\nWelcome to the CS 211 Word Ladder Generator!\n\n");

    // set word length using interactive user-input
    int wordSize;
    printf("Enter the word size for the ladder: ");
    scanf("%d",&wordSize);
    printf("\n");

    printf("This program will make the shortest possible\n"); 
    printf("word ladder between two %d-letter words.\n\n",wordSize);
    
    // interactive user-input sets the dictionary file;
    //  check that file exists; if not, user enters another file
    //  if file exists, count #words of desired length [wordSize]
    char dict[100];
    printf("Enter filename for dictionary: ");
    scanf("%s", dict);
    printf("\n");
    int numWords = countWordsOfLength(dict,wordSize);
    while (numWords < 0) {
        printf("  Dictionary %s not found...\n",dict);
        printf("Enter filename for dictionary: ");
        scanf("%s", dict);
        printf("\n");
        numWords = countWordsOfLength(dict,wordSize);
    }
    
    // allocate heap memory for the word array; only words with desired length
    char** words = (char**)malloc(numWords*sizeof(char*));
    for (int i = 0; i < numWords; ++i) {
        words[i] = (char*)malloc((wordSize+1)*sizeof(char));
    }

    // end program if file does not have at least two words of desired length
    if (numWords < 2) {
        printf("  Dictionary %s contains insufficient %d-letter words...\n",dict,wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    
    // [usedWord] bool array has same size as word array [words];
    //  all elements initialized to [false];
    //  later, usedWord[i] will be set to [true] whenever 
    //      words[i] is added to ANY partial word ladder;
    //      before adding words[i] to another word ladder,
    //      check for previous usage with usedWord[i] 
    bool* usedWord = (bool*)malloc(numWords*sizeof(bool));
    for (int i = 0; i < numWords; ++i) {
        usedWord[i] = false;
    }
    
    // build word array (only words with desired length) from dictionary file
    printf("Building array of %d-letter words... ", wordSize); 
    bool status = buildWordArray(dict,words,numWords,wordSize);
    if (!status) {
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of %d-letter words.\n",wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    printf("Done!\n"); 

    // set the two ends of the word ladder using interactive user-input
    //  make sure start and final words are in the word array, 
    //  have the correct length (implicit by checking word array), AND
    //  that the two words are not the same
    char startWord[30];
    char finalWord[30];
    printf("Setting the start %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, startWord);
    printf("\n");
    printf("Setting the final %d-letter word... \n", wordSize);
    setWord(words, numWords, wordSize, finalWord);
    while (strcmp(finalWord,startWord) == 0) {
        printf("  The final word cannot be the same as the start word (%s).\n",startWord);
        printf("Setting the final %d-letter word... \n", wordSize);
        setWord(words, numWords, wordSize, finalWord);
    }
    printf("\n");

    // run the algorithm to find the shortest word ladder
    WordNode* myLadder = findShortestWordLadder(words, usedWord, numWords, wordSize, startWord, finalWord);

    // display word ladder and its height if one was found
    if (myLadder == NULL) {
        printf("There is no possible word ladder from %s to %s\n",startWord,finalWord);    
    } else {
        printf("Shortest Word Ladder found!\n");
        printLadder(myLadder);
    }
    printf("Word Ladder height = %d\n",getLadderHeight(myLadder));
    
    // free the heap-allocated memory for the shortest ladder
    freeLadder(myLadder);
    // free the heap-allocated memory for the words array
    freeWords(words,numWords);
    free(usedWord);
    
    return 0;
}

