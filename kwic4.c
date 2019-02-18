/*
 * Louis Kedziora
 * V00820695
 * Dr. Zastre - SENG 265
 * Assignment 4
 * April 6, 2018
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

typedef struct node_s node_s;
typedef struct node_w node_w;
void strupr(char *str);
void strlowr(char *str);
void add_exclusion_word(char* exl);
void index_line(char* line);
int check_exclusion(char* word);
void add_index_word(char* word);
void kwic_line(char* word, char* line);
void add_word_sentence(char* took);
void add_kwicd_line(char* final_line,char* word);
void free_sentence();
void print_free();

/*
 * node_w
 * Struct that provides linked list data structure for index words.
 *
 * next     = Pointer to next element in list.
 * head_s   = Pointer to the first element in the,
 *            list of indexed lines of that word.
 * word     = The index word.
 * length   = Length of either the index word or the,
 *            accompanying indexed list.
 */
typedef struct node_w {
    node_w* next;
    node_s* head_s;
    char* word;
    int length;
} node_w;
node_w head_wds = {.length=0, .next=NULL};//Intialize head of index words list.
node_w head_exl = {.length=0, .next=NULL};//Intialize head of exclusion words list.

/*
 * node_s
 * Struct that provides linked list data structure for a sentence.
 *
 * next     = Pointer to next element in list.
 * prev     = Pointer to previous element in list.
 * word     = The word in the sentence.
 * length   = Length of the word in the sentence.
 */
typedef struct node_s {
    node_s* prev;
    node_s* next;
    char* word;
    int length;
} node_s;
node_s head_sent = {.length=0, .next=NULL, .prev=NULL};//Intialize head of sentence list.
node_s tail_sent = {.length=0, .next=NULL, .prev=NULL};//Intialize tail of sentence list.

/*
 * main(int argc, char **argv)
 *
 * This is where the rest of the program gets,
 * called and executed.
 */
int main(int argc, char **argv) {
    char* buffer = NULL;
    size_t buffer_size, char_read;
    int exl_ct = 0;

    while((char_read = getline(&buffer, &buffer_size, stdin)) != -1) {
        switch(exl_ct) {

            case 0:
            case 1:
                exl_ct++;
                continue; //ignore first two lines
            
            case 2: //exclusion words builder
                if(buffer[0] == ':') { //end of exl_wds
                    exl_ct++;
                    break;
                }
                strlowr(buffer);
                add_exclusion_word(buffer);
                break;

            default :  //build the index
                if(buffer[0] == '\n') break; //This should skip empty lines
                index_line(buffer);
        }
    }
    print_free();
}

/*
 * print_free()
 *
 * This function steps through the index words linked list,
 * and prints out the contents, then it frees all the malloc'd,
 * memory and returns.
 *
 * @params  = N/A
 * @returns = N/A
 */
void print_free() {
    node_w* w1_stepper = head_wds.next;
    node_w* w2_stepper = head_wds.next;
    node_s* s1_stepper;
    node_s* s2_stepper;
    for(int s = 0; s < head_wds.length; s++) {
        s1_stepper = w1_stepper->head_s->next;
        for(int b = 0; b < w1_stepper->head_s->length; b++) {
            printf("%s", s1_stepper->word);
            s1_stepper = s1_stepper->next;
        }
        w1_stepper = w1_stepper->next;
    }
    w1_stepper = NULL; //Clean memory.
    w2_stepper = NULL;
    s1_stepper = NULL;
    s2_stepper = NULL;
    if(head_exl.length > 0) { //No need to free if no mallocs happen.
        //free exclusion words
        w1_stepper = head_exl.next;
        w2_stepper = w1_stepper->next;
        for (int i = 0; i < head_exl.length - 1; i++) {
            free(w1_stepper->word);
            free(w1_stepper->head_s);
            free(w1_stepper);
            w1_stepper = w2_stepper;
            w2_stepper = w2_stepper->next;
        }
    }
    w1_stepper = NULL; //Clean memory.
    w2_stepper = NULL;
    s1_stepper = NULL;
    s2_stepper = NULL;
    if(head_wds.length > 0) {
        //Free index words and their kwic lines.
        w1_stepper = head_wds.next;
        w2_stepper = w1_stepper;
        for (int k = 0; k < head_wds.length - 1; k++) {
            s1_stepper = w1_stepper->head_s->next;
            s2_stepper = s1_stepper;
            for (int f = 0; f < w1_stepper->head_s->length; f++) {
                s2_stepper = s2_stepper->next;
                free(s1_stepper->word);
                free(s1_stepper);
                s1_stepper = s2_stepper;
            }
            w2_stepper = w2_stepper->next;
            free(w1_stepper->word);
            free(w1_stepper->head_s);
            free(w1_stepper);
            w1_stepper = w2_stepper;
        }
    }


}

/*
 * free_sentence()
 *
 * This function steps through the sentence list,
 * and frees all the malloc'd memory contained there.
 *
 * @params  = N/A
 * @returns = N/A
 */
void free_sentence() {
    if(head_sent.length == 0)  return;
    node_s* free1 = head_sent.next;
    node_s* free2;
    for(int i = 0; i < head_sent.length - 1; i++) {
        free2 = free1->next;
        assert(free1->next != NULL);
        free(free1->word);
        assert(free1 != NULL);
        free(free1);
        free1 = free2;
    }
    free(free1->word);
    free(free1);

}

/*
 * add_kwic_line(char* final_line,char* word)
 *
 * This function takes the formatted line "final_line",
 * and places it in a node on the "word"'s linked list,
 * adding it to the back because of assignment specs.
 *
 * @params  = final_line - The finalized kwic'd sentence.
 *            word - The current word being worked on.
 * @returns = N/A
 */
void add_kwicd_line(char* final_line,char* word) {

    node_s* new_node = (node_s*)malloc(sizeof(struct node_s));
    if(!new_node) {
        printf("EXIT #9");
        exit(1);
    }
    new_node->word = (char*)malloc((strlen(final_line)+1)*sizeof(char));
    if(!(new_node->word)) {
        printf("EXIT #10");
        exit(1);
    }
    strncpy(new_node->word, final_line, strlen(final_line)+1);
    new_node->word[strlen(final_line)] = '\0';
    node_w* cur = &head_wds;
    int comp;
    cur = cur->next;
    for(int i = 0; i < head_wds.length - 1; i++) {
        comp = strcmp(word, cur->word);
        if(comp == 0) {
            break;
        }
        cur = cur->next;
    }
    if(cur->head_s->length == 0) {
        cur->head_s->next = new_node;
    }else {
        node_s* pos = cur->head_s->next;
        int count = cur->head_s->length;
        for(int i = 0;i < count - 1 ; i++ ) pos = pos->next;
        pos->next = new_node;
    }
    cur->head_s->length++;

}

/*
 * add_word_sentence(char* took)
 *
 * This function takes a char array pointer and creates,
 * a node then adds it to the end of the sentence.
 *
 * @params  = took - The word in which to add to the end of the sentence.
 * @returns = N/A
 */
void add_word_sentence(char* took) {
    node_s* new_node = (node_s*)malloc(sizeof(struct node_s));
    if(!new_node) {
        printf("EXIT #5");
        exit(1);
    }
    new_node->word = (char*)malloc((strlen(took)+1)*sizeof(char));
    if(!(new_node->word)) {
        printf("EXIT #6");
        exit(1);
    }
    strncpy(new_node->word, took, strlen(took) + 1);
    new_node->length = strlen(took);
    if(head_sent.length == 0) { //first one needs head and tail pointed to it
        tail_sent.next = new_node;
        head_sent.next = new_node;
        new_node->next = NULL;
    }else { //add to back
        new_node->prev = tail_sent.next;
        tail_sent.next->next = new_node;
        new_node->next = NULL;
        tail_sent.next = new_node;
    }
    head_sent.length++;
}

/*
 * kwic_line(char* word, char* line)
 *
 * This function takes and word and the sentence,
 * in which it resides and kwic indexes it according,
 * to assignment specs.
 *
 * @params  = word - The word that needs to be indexed.
 *            line - The sentence that will be indexed on.
 * @returns = N/A
 */
void kwic_line(char* word, char* line) {
    char play_line[strlen(line)];
    strncpy(play_line, line, strlen(line) + 1);
    play_line[strlen(play_line) - 1] = '\0';
    char* delim = " ";

    //Intialize and clean memory.
    char final_line[71];
    memset(final_line,'\0',sizeof(final_line));
    char left_side[31];
    memset(left_side,'\0',sizeof(left_side));
    char right_side[42];
    memset(right_side,'\0',sizeof(right_side));
    char left_hold[21];
    memset(left_hold,'\0',sizeof(left_hold));
    head_sent.length = 0;
    char* took = strtok(play_line, delim);//can make copy of line
    do {
        add_word_sentence(took);
        took = strtok(NULL, delim);
    } while(took != NULL);

    node_s* cur = &head_sent;
    cur = cur->next;

    int position = 0;
    char* lowercomp = (char*)malloc((strlen(cur->word)+1)*sizeof(char));
    if(!lowercomp) {
        printf("EXIT #7");
        exit(1);
    }
    strncpy(lowercomp, cur->word,strlen(cur->word) + 1);
    strlowr(lowercomp);
    int comp = strcmp(lowercomp, word);
    if(comp != 0) {
        int yes = 0;
        for (int i = 0; i < head_sent.length - 1; i++) {
            cur = cur->next;
            lowercomp = (char*)malloc((strlen(cur->word)+1)*sizeof(char));
            if (!lowercomp) {
                printf("EXIT #8");
                exit(1);
            }
            strncpy(lowercomp, cur->word, cur->length + 1);
            strlowr(lowercomp);
            comp = strcmp(lowercomp, word);//might be accessing wrong element
            if (comp == 0) {
                yes = 1;
                position = i + 1;
                break;
            }
            free(lowercomp);

        }
        if (yes == 0) return; //use this to return if word wasn't in sentence
    } else {

    }
    node_s* jumper = cur->prev; //This may change current! not make a copy of it!
    int left_len = 0;
    int left_count = 0;
    for(int j = 0; j < position; j++) { //get count and length of left
        left_len = left_len + jumper->length + 1;
        if(left_len >= 21) {
            left_count = j;
            break;
        }else if(left_len == 20) {
            left_count = j + 1;
            break;
        }
        jumper = jumper->prev;
    }
    if(left_count == 0 && left_len == 21) {
        left_count = position - 1;
    }else if(left_count == 0) {
        left_count = position;
    }
    int right_len = cur->length;
    node_s* move = cur->next;
    int right_count = 0;
    while(move != NULL) {//get count and length of right
        right_len = right_len + move->length + 1;
        if(right_len > 31) {
            break;
        }else {
            right_count++;
        }
        move = move->next;
    }
    //if(right_len != cur->length) right_count--;
    node_s* ptr1 = cur;
    strupr(word);
    strcat(right_side, word);
    for(int k = 0; k < right_count;k++) {
        ptr1 = ptr1->next;
        sprintf(right_side,"%s %s",right_side,ptr1->word);
    }
    sprintf(right_side,"%s\n",right_side);
    right_side[strlen(right_side)] = '\0';

    if(left_count > 0) {
        node_s *ptr2 = cur;
        for (int t = 0; t < left_count; t++) {
            ptr2 = ptr2->prev;
        }
        strcat(left_side, ptr2->word);
        for (int p = 0; p < left_count - 1; p++) {
            ptr2 = ptr2->next;
            sprintf(left_side, "%s %s", left_side, ptr2->word);
        }
        strcat(left_side, " ");
    }
    strncpy(left_hold,left_side, strlen(left_side));
    left_hold[strlen(left_hold)] = '\0';
    int space_num = 29 - strlen(left_side);
    memset(left_side,'\0',sizeof(left_side));
    memset(left_side,' ',space_num);
    strcat(left_side, left_hold);
    sprintf(final_line,"%s%s", left_side, right_side);
    strlowr(word);
    add_kwicd_line(final_line,word);
    free_sentence();

}

/*
 * add_index_word(char* word)
 *
 * This function takes a word as input copies it,
 * to a node and adds to the index word linked list,
 * while maintaining alphabetical order and not adding,
 * if the it already exists.
 * AFTERTHOUGHT: Mallocing before checking if already in list so,
 * this will be lost memory since we will have no pointer to this,
 * memory after return.
 *
 * @params  = word - The word to add to the index list.
 * @returns = N/A
 */
void add_index_word(char* word) {

    node_w* new_node = (node_w*)malloc(sizeof(struct node_w));
    if(!new_node) {
        printf("EXIT #3");
        exit(1);
    }
    new_node->word = (char*)malloc((strlen(word)+1)*sizeof(char));
    if(!(new_node->word)) {
        printf("EXIT #4");
        exit(1);
    }
    new_node->head_s = (node_s*)malloc(sizeof(struct node_s));
    if(!new_node->head_s) {
        printf("EXIT #20");
        exit(1);
    }

    strncpy(new_node->word, word, strlen(word) + 1);
    new_node->head_s->length = 0;

    if(head_wds.length == 0) {
        head_wds.next = new_node;
        new_node->next = NULL;
        head_wds.length++;
        return;
    }else {
        node_w* cur = &head_wds;
        node_w* prev;
        cur = cur->next;
        int comp_val = strcmp(cur->word,word);
        if (comp_val > 0) {
            new_node->next = head_wds.next;
            head_wds.next = new_node;
            head_wds.length++;
            return;
        } else if(comp_val == 0) return;

        for(int k = 0; k < head_wds.length - 1; k++) {
            prev = cur;
            cur = cur->next;
            comp_val = strcmp(cur->word,word);
            if(comp_val == 0) {
                return;
            }else if(comp_val < 0) {
            }else {
                new_node->next = cur;
                prev->next = new_node;
                head_wds.length++;
                return;
            }
        }
        cur->next = new_node;
        head_wds.length++;
    }

}

/*
 * check_exclusion(char* word)
 *
 * This function takes a word and checks it against,
 * the exclusion word list and returns "1" if it is,
 * and "0" if it is not.
 *
 * @params  = word - Word to be checked against the list.
 * @returns = int  - "1" if in exclusion list.
 *                   "0" if NOT in exclusion list.
 */
// return true(1) if word is in exclusion list
// else return false(0)
int check_exclusion(char* word) {
    if(head_exl.length == 0) return 0;
    node_w* cur = head_exl.next;
    if((strcmp(cur->word, word)) == 0) return 1;
    for(int i = 0; i < head_exl.length -1; i++) {
        cur = cur->next;
        if((strcmp(cur->word, word)) == 0) return 1;
    }
    return 0;
}

/*
 * index_line(char* line)
 *
 * This function is the first call of main to start doing work,
 * parses all the words and calls the other functions to do,
 * all the indexes of that line and then returns.
 *
 * @params  = line - Line to index.
 * @returns = N/A
 */
void index_line(char* line) {
    char *line_chop = (char*)malloc((strlen(line)+1)*sizeof(char));
    if(!(line_chop)) {
        printf("EXIT #14");
        exit(1);
    }
    strncpy(line_chop, line, strlen(line) + 1);
    line_chop[strlen(line_chop) - 1] = '\0';

    char* word = strtok(line_chop, " ");
    do {
        strlowr(word);
        if(!check_exclusion(word)) { //return true if word is in list
            add_index_word(word);
        }
        word = strtok(NULL, " ");
    } while(word != NULL);

    node_w* stepper = head_wds.next;
    for(int i = 0; i < head_wds.length; i++) {
        kwic_line(stepper->word, line);
        stepper = stepper->next;
    }
    free(line_chop);

}

/*
 * add_exclusion_word(char* word)
 *
 * This function takes a word and adds it to,
 * the exclusion word list.
 *
 * @params  = word - Word to be added to exclusion list.
 * @returns = N/A
 */
void add_exclusion_word(char* exl) {
    exl[strlen(exl) - 1] = '\0';//get rid of newline char
    node_w* new_node = (node_w*)malloc(sizeof(struct node_w));
    if(!new_node) {
        printf("EXIT #1");
        exit(1);
    }
    new_node->word = (char*)malloc((strlen(exl)+1)*sizeof(char));
    if(!(new_node->word)) {
        printf("EXIT #2");
        exit(1);
    }
    strncpy(new_node->word, exl, strlen(exl) + 1);
    if(head_exl.length == 0) {
        new_node->next = NULL;
        head_exl.next = new_node;
    }else {
        node_w* cur = head_exl.next;
        for(int i = 0; i < head_exl.length - 1; i++) cur = cur->next;
        cur->next = new_node;
    }
    new_node->next = NULL;
    head_exl.length++;
}

/*
 * strupr(char *str)
 *
 * This function takes a a pointer to a word,
 * and makes all the letters uppercase.
 *
 * @params  = word - Word to be uppercased.
 * @returns = N/A
 */
void strupr(char *str) {
    for(int i = 0;i < (strlen(str)); i++) {
        str[i] = toupper(str[i]);
    }
}

/*
 * strlowr(char *str)
 *
 * This function takes a a pointer to a word,
 * and makes all the letters lowercase.
 *
 * @params  = word - Word to be lowercased.
 * @returns = N/A
 */
void strlowr(char *str) {
    for(int i = 0;i < (strlen(str)); i++) {
        str[i] = tolower(str[i]);
    }
}
