#include "wordle.h"
#include <string.h>
#include <stddef.h>
#include <stdio.h>
int score(char **word_list, char *candidate, guess *guesses, int guess_count) {
        int sum=0;
        for (int word=0;word_list[word]!=NULL;word++){
                if (is_viable_candidate(word_list[word],guesses,guess_count)){
                    char copy[6];
                    strcpy(copy, word_list[word]);
                    for(int i=0;i<5;i++){
                        if (copy[i]==candidate[i]){
                            copy[i]='_';
                            sum+=EXACT_HIT_POINTS;

                        }
                    }
                    for(int letter=0;letter<5;letter++){
                        for(int i=0;i<5;i++){
                            if (copy[i]==candidate[letter]){
                                sum+=PARTIAL_HIT_POINTS;
                                copy[i]='_';
                                break;
                            }

                        }
                    }
                }

        }

        return sum;
    }
