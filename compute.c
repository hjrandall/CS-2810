#include "compute.h"
#include <string.h>
int compute(char *a, char *b){
    int a_len= strlen(a);
    int b_len= strlen(b);
    if (a_len!=b_len){
        return -1;
    }
    int ham_dis= 0;
    for (int i=0; i< a_len;++i ){
        if (a[i]!=b[i]){
            ham_dis+=1;
        }
    }
    return ham_dis;
}