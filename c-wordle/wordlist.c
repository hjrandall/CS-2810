#include "wordle.h"
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

char **read_word_list(char *filename) {
    FILE *fp=fopen(filename,"r");
    int len=0;
    int capacity=32;
    char **array_words=malloc(sizeof(char*)*capacity);
    char line[6];
    while(fgets(line,6,fp)!=NULL){
        char *start = line;
        while (*start){
            while(*start && isspace(*start)){
                start++;
            }
            char *end=start;
            while(*end && !isspace(*end)){
                end++;
            }
            if (start==end){
                continue;
            }
            int wordlen=end-start;
            char *word = malloc(wordlen+1);
            assert(word!=NULL);
            strncpy(word,start,wordlen);
            array_words[len]=word;
            len+=1;

            if (len==capacity){
                capacity*=2;
                array_words=realloc(array_words,sizeof(char*)*capacity);
            }
            start=end;
        }
    }
    fclose(fp);

    return array_words;
}

void free_word_list(char **list) {
    for (int i=0; list[i]!=NULL;i++){
        free(list[i]);
    }
    free(list);
}
