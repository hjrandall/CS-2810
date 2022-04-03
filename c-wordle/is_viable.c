#include <stdbool.h>
#include<stdio.h>
#include "wordle.h"
#include <string.h>

bool is_viable_candidate(char *candidate, guess *guesses, int guess_count) {


    //check for exact hits

    for (int guess_num=0;guess_num<guess_count;guess_num++){
        char copy[6];
        strcpy(copy, candidate);
        for (int i=0;i<5;i++){
            if (guesses[guess_num].feedback[i]==EXACT_HIT){

                if (copy[i]!=guesses[guess_num].letters[i]){
                    return false;
                }
                else{
                    copy[i]='_';
                }
            }
        }


    //check for partial hits
        for (int i=0;i<5;i++){
            if (guesses[guess_num].feedback[i]==PARTIAL_HIT){
                if (copy[i]==guesses[guess_num].letters[i]){
                    return false;

                }
            }
        }

    //check for partial hits again
        for (int i=0;i<5;i++){
            if (guesses[guess_num].feedback[i]==PARTIAL_HIT){
                bool found=true;
                for(int letter=0;letter<5;letter++){
                    if (copy[letter]==guesses[guess_num].letters[i] ){
                        copy[letter]='_';
                        found=false;
                        break;
                    }
                }
                if (found){

                    return false;
                }
            }
        }


    //check for misses

        for (int i=0;i<5;i++){

            if (guesses[guess_num].feedback[i]==MISS){

                for(int letter=0;letter<5;letter++){
                    if (copy[letter]==guesses[guess_num].letters[i]){

                        return false;
                    }
                }
            }
        }
    }

    return true;
}
