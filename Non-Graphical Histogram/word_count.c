#include "word_count.h"

int redundantCat(char* test_word, category* current);
int redundantWord(char* test_word, category* the_category);
void newCat(int length, int frequency, category* the_tail);
void newWord(char* string, category* the_category, word_t* current);
void freeThemAll(category* current);
void freeSubList(word_t* current);
void printList(category* current);
void sortCats(category* head);
void sortWords(int mode, word_t* first);


//MAIN - parses command line, reads from file, and calls a bunch of functions
int main(int argc, char *argv[]) {
    
    FILE* Read_File = NULL;
    
    int infile = 0;
    int sort = 0;

    //Parse the command line, exit program if necessary
    for (int i = 1; i < argc; i++) {
        
        if (strcmp(argv[i], "--infile") == 0) {
            
            infile = 1;

            if (argc >= i + 1) {
            
                i++;
                Read_File = fopen(argv[i], "r");
            
                if (Read_File == NULL) {
                    fprintf(stderr, "Unable to open '%s' for reading.\n", argv[i]);
                    exit(2);
                }
            }
        }
        else if (strcmp(argv[i], "--sort") == 0) {
            sort = 1;
        }
        else {
            //Check if it's merely an invalid switch (i.e. --bad-arg)
            if (strlen(argv[i]) >= 2) {
                if (argv[i][0] == '-' && argv[i][1] == '-') {
                    //do nothing? do nothing.
                }
                else {
                    fprintf(stderr, "Invalid command line.\n");
                    exit(1);
                }
            }
            else {
                fprintf(stderr, "Invalid command line.\n");
                exit(1);
            }
        }
    }
    if (infile == 0) {
        fprintf(stderr, "Error, no '--infile' argument given.\n");
        exit(1);
    }
    
    
    //Set up the linked list!
    //Make the dummy head
    category* head = (category*)emalloc(sizeof(category));
    
    head->next = NULL;
    head->prev = NULL;
    head->first_word = NULL;
    head->freq = 0;
    head->len = -1;
    
    //Make the dummy tail
    category* tail = (category*)emalloc(sizeof(category));
    
    tail->next = NULL;
    tail->prev = head;
    tail->first_word = NULL;
    tail->freq = 0;
    tail->len = -2;

    head->next = tail;


    //Get tehe character count of the file, so we can know how big to make inputData[]    
    
    char charr;
    int character_count = 0;
    while(fscanf(Read_File, "%c", &charr) == 1) {
        character_count++;
    }
    rewind(Read_File);

    char* inputData = (char*)emalloc((character_count+1)*sizeof(char));
    for (int i = 0; fscanf(Read_File, "%c", &charr) == 1; i++) {
        inputData[i] = charr;
    }
    inputData[character_count] = '\0';    

    //Hey, test if we actually got nothing, if there are no words.
    if ((inputData[0] == '\n' || inputData[0] == ' ' || inputData[0] == '\t' || inputData[0] == '\0') && character_count == 1) {
        return 0;
    }

    //Extra bit to handle when a file contains only delimeters or non-characters. Otherwise, strtok() throws a segmentation fault
    int no_char = 1;
    for (int i = 0; i < character_count; i++) {
        if (inputData[i] == '\n' || inputData[i] == ' ' || inputData[i] == '\t' || inputData[i] == '\0' || inputData[i] == '.' || inputData[i] == ',' || inputData[i] == ';' || inputData[i] == '(' || inputData[i] == ')') {
            //do nothing, because we don't want to change no_char. I would do this the other way, but because of logic, I can't!
        }
        else {
            no_char = 0;
        }
    }   
    if (no_char == 1) {
        return 0;
    }
 
    /*if (Read_File != NULL) {
        printf("\nNOTNULL\n");
    }*/

    //Tolkienize!
    char* char_poin;
    char_poin = strtok(inputData, " .,;()\n");
    while (char_poin != NULL) {

        //printf("Word is: (%s)\n", char_poin); 
        if (!redundantCat(char_poin, head->next)) {
                    
            //We need to make a new category!
            newCat(strlen(char_poin), 1, tail);
                    
            //We also need to make a new word!
            newWord(char_poin, tail->prev, tail->prev->first_word);
        }
        
        char_poin = strtok(NULL, " .,;()\n");
    }

    
    //Sort the doubly linked lists!
    sortCats(head);

    category* this = head->next;
    while (this->len != -2) {
        sortWords(sort, this->first_word);
        this = this->next;
    }

    //Print out the histogram and words    
    printList(head);

    //Free everything that was allocated
    freeThemAll(head);
    free(inputData);
    fclose(Read_File);
    
    //End program!
    return 0;
}


//Iterates through the major doubly linked list to determine if the new word fits in an already existing category
//Returns 0 if it was not redundant,
//returns 1 if it was redundant and also makes a call to newWord() to make the new word so I don't have to remember the reference to where we're at.
int redundantCat(char* test_word, category* current) {
    
    int length = strlen(test_word);
    
    //Using the fact that dummy tail has len = -2:
    while (current->len != -2) {
        
        if (current->len == length) {
            current->freq += 1;
                        
            //If the word doesn't yet exist...
            if (!redundantWord(test_word, current)) {
                //We need to create it!
                newWord(test_word, current, current->first_word);
            }
            
            //printf("Category already exists\n");
            return 1;
        }
        
        current = current->next;
    }
    
    //printf("Category does not exist\n");
    return 0;
}


//Iterates through the secondary doubly linked list to determine if word is already present
//Returns 1 if redundant, 0 is not.
int redundantWord(char* test_word, category* the_category) {
    
    word_t* current = the_category->first_word;

    while(current != NULL) {
       
        //printf("Comparing: [%s] with [%s]\n", current->the_word, test_word);
 
        if (strcmp(current->the_word, test_word) == 0) {
            //printf("Word already exists\n");
            return 1;
        }
        
        current = current->next;
    }
    
    //printf("Word does not yet exist\n");
    return 0;
}


//Pushes new Category node to the end of the doubly linked list
void newCat(int length, int frequency, category* the_tail) {
    
    //Allocating space for new node
    category* new_cat = (category*)emalloc(sizeof(category));

    //Putting in variables
    new_cat->freq = frequency;
    new_cat->len = length;
    new_cat->first_word = NULL;
    
    //Shuffling of pointers
    new_cat->prev = the_tail->prev;
    the_tail->prev->next = new_cat;
    new_cat->next = the_tail;
    the_tail->prev = new_cat;
    //What Was: Node <-> Tail
    //What Is Now: Node <-> NewNode <-> Tail
}


//Pushes new word to the end of a doubly linked word list
void newWord(char* string, category* the_category, word_t* spot) {
    
    word_t* current = spot;

    int i = 0;

    //We can't traverse what isn't there!
    if (current == NULL) {
        word_t* new_word = (word_t*)emalloc(sizeof(word_t));
        new_word->next = NULL;
        new_word->prev = NULL;
        new_word->the_word = string;
        
        the_category->first_word = new_word;

        //printf("New word - %s\n", the_category->first_word->the_word);
    }
    else {

        i++;
        
        //Go to the end of the list
        while (current->next != NULL) {
            current = current->next;
            i++;
        }
        
        word_t* new_word = (word_t*)emalloc(sizeof(word_t));
        new_word->next = NULL;
        new_word->prev = current;
        new_word->the_word = string;

        current->next = new_word;
    }

    //printf("Am I still the same? %s\n", the_category->first_word->the_word);
}


//Free every allocated space EVERYWHERE!
//Logic:
//      Are we at head/tail?
//          Are we at head?
//              free(head) - head doesn't have a sublist, unique code needed to avoid trying to access what isn't there
//          else
//              free(tail)
//      Otherwise
//          free(current)
//      

void freeThemAll(category* current) {
 
    int i = 0;
    while(current != NULL) {
        
        if (current->len == -1) {
            //printf("[x] Dummy Head\n");
            current = current->next;
            free(current->prev);
            current->prev = NULL;
        }
        else if (current->len == -2) {
            //printf("[x] Dummy Tail\n");
            free(current);
            current = NULL;
        }
        else {
                        
            current = current->next;
            //printf("[%d] Len is: %d, Freq is: %d\n", i, current->prev->len, current->prev->freq);
            i++;

            freeSubList(current->prev->first_word);
            free(current->prev);
            current->prev = NULL;      
        }
    }
}


//Frees every allocated space for the sub-lists
void freeSubList(word_t* current) {
   
    if (current == NULL) {
        printf("No words in category?\n");
    }
 
    while(current != NULL) {
        
        //printf("\tWord is: %s\n", current->the_word);
        if (current->next != NULL) {
            current = current->next;
            free(current->prev);
            current->prev = NULL;
        }
        else {
            free(current);
            current = NULL;
        }
    }
}


//Prints out the contents of the doubly linked list
void printList(category* current) { 
 
    word_t* curr = (word_t*) emalloc(sizeof(word_t));
    word_t* curr_ref = curr;

    while (current != NULL) {
        //printf("Current length: %d\n", current->len);
        if (current->len == -1) {
            //printf("Head\n");
            current = current->next;
            continue;   
        }
        else if (current->len == -2) {
            //printf("Tail\n");
            current = current->next;
            continue;
        }

        printf("Count[%02d]=%02d; (words: ", current->len, current->freq);
        
        curr = current->first_word;
        int num_words = 0;
        while (curr != NULL) {
            num_words++;
            curr = curr->next;
        }
        
        //Print formatting JUNK
        if (num_words == 1) {
            printf("\"%s\")", current->first_word->the_word);
        }
        else if (num_words == 2) {
            printf("\"%s\" and \"%s\")", current->first_word->the_word, current->first_word->next->the_word);
        }
        else {
            curr = current->first_word;
            while (curr != NULL) {
                
                if (num_words > 2) {
                    
                    printf("\"%s\", ", curr->the_word);
                }
                else {
                    printf("\"%s\" and \"%s\")", curr->the_word, curr->next->the_word);
                    break;
                }
            
                curr = curr->next;
                num_words--;
            }
        }
        
        printf("\n");        
        
        current = current->next;
    }
    
   free(curr_ref);
}


//Sort the catogories, depending on mode. Also calls sortWords.
void sortCats(category* head) {
    
    //if mode == 1: len from smallest to largest
    //REMEMBER: head has len = -1, tail has len = -2!
    category* current1 = head->next;
    category* current2 = current1->next;
    
    while (current1->next->len != -2) {

        while (current2->len != -2) {
            //Check if we should swap
            if (current1->len > current2->len) {
             
                //Hold values so we don't lose them
                int temp_len = current1->len;
                int temp_freq = current1->freq;
                
                //Swap values of one with the other
                current1->len = current2->len;
                current1->freq = current2->freq;
                
                //Swap other with one
                current2->len = temp_len;
                current2->freq = temp_freq;

                //Swap which sub-list they point to
                word_t* temp = current1->first_word;
                current1->first_word = current2->first_word;
                current2->first_word = temp;
            }
            
            current2 = current2->next;
        }
        
        //Setup for the next pass by of sorting
        current1 = current1->next;
        current2 = current1->next;
    }
}


//Sorts the words. If mode == 0, in acending order, otherwise, descending
void sortWords(int mode, word_t* first) {
    
    word_t* current = first;
    int swap_flag = 0;

    if (current == NULL) {
        return;
    }
    
    if (current->next == NULL) {
        return;
    }
   
    while (1) {       
       
        while (current->next != NULL) {

            if (mode == 0) {
            
                if (strcmp(current->the_word, current->next->the_word) > 0) {
                
                    //printf("Swapped [%s] with [%s]\n", current->the_word, current->next->the_word);
                
                    //Need to swap
                    char* temp = current->the_word;
                    current->the_word = current->next->the_word;
                    current->next->the_word = temp;
                
                    swap_flag = 1;
                }
            }
            else {
                if (strcmp(current->the_word, current->next->the_word) < 0) {
                    //Need to swap
                    char* temp = current->the_word;
                    current->the_word = current->next->the_word;
                    current->next->the_word = temp;
                
                    swap_flag = 1;
                }
            }
            current = current->next;
        }
         
        if (swap_flag == 0) {
            //printf("[%d] - WE ENDED ON: %s\n", o, current->the_word);
            break;
        }

        swap_flag = 0;
        current = first;
    }
}
