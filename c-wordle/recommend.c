#include "wordle.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
// a convenient struct to store a potential guess along with its score
struct recommendation {
    int score;
    char word[6];
};

void recommend(char **word_list, guess *guesses, int guess_count) {
    int count=0;
    struct recommendation top_20[RECOMMENDATION_COUNT+1];
    
    for (int word=0;word_list[word]!= NULL;word++){
        if (is_viable_candidate(word_list[word],guesses,guess_count)){

            int points= score(word_list,word_list[word],guesses,guess_count);
            struct recommendation option;
            option.score=points;
            strcpy(option.word,word_list[word]);
            top_20[count]=option;
            count+=1;
            for (int i=count-1;i>0;i--){
                if (top_20[i].score>top_20[i-1].score){
                    struct recommendation holder=top_20[i];
                    top_20[i]=top_20[i-1];
                    top_20[i-1]=holder;
                }
                else if(top_20[i].score==top_20[i-1].score){
                    if(strcmp(top_20[i].word,top_20[i-1].word) < 0){
                        struct recommendation holder=top_20[i-1];
                        top_20[i-1]=top_20[i];
                        top_20[i]=holder;
                    }
                }
            }
            if (count>20){
                count=20;
            }
        }
    }
    for(int i=0;i<count;i++){
        printf("%u: %s\n",top_20[i].score,top_20[i].word);
    }
    
}
