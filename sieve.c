#include <stdbool.h>
#include <stdio.h>
#include <math.h>
void sieve(bool *array, int size ){
    for(int i=0;i<size;i++){
        array[i]=true;
    }
    double sqr_size=size;
    for (int i=2;i<sqr_size;i++){
        bool num=true;
        for (int j=2;j<i;j++){
            if (i%j==0 && j!=i){
                num=false;

                break;
            }
        }
        array[i]=num;
        if (num==true){
            for(int h=2;i*h<size;h++){
                array[i*h]=false;
            }
        }
    }
}