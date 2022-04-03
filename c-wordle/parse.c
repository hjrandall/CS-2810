#include "wordle.h"
#include <string.h>

guess parse_guess(char *line) {
    guess posibility;
    int index=0;
    for (long unsigned int i=0;i<strlen(line);){
        if (line[i]=='['){
            posibility.letters[index]=line[i+1];
            posibility.feedback[index]=EXACT_HIT;
            index+=1;
            i+=3;
        }
        else if (line[i]=='('){
            posibility.letters[index]=line[i+1];
            posibility.feedback[index]=PARTIAL_HIT;
            index+=1;
            i+=3;
        }
        else{
            posibility.letters[index]=line[i];
            posibility.feedback[index]=MISS;
            index+=1;
            i+=1;
        }
    }
    posibility.letters[5]='\0';
    return posibility;
}
