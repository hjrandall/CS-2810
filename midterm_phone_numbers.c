#include "phonenumber.h"
#include <stdio.h>

void format_phone_number(char *line) {
    int num1,num2,num3;
    char nl;

    if (sscanf(line,"(%3d) %3d-%4d%c",&num1 , &num2,&num3 ,&nl)==4 && nl=='\n'){
        printf("(%d) %d-%d\n",num1 , num2,num3);
    }
    else if (sscanf(line,":%3d:%3d-%4d%c",&num1 , &num2,&num3 ,&nl)==4 && nl=='\n'){
        printf("(%d) %d-%d\n",num1,num2,num3);
    }
    else if (sscanf(line,"%3d-%3d-%4d%c",&num1 , &num2,&num3 ,&nl)==4 && nl=='\n'){
        printf("(%d) %d-%d\n",num1,num2,num3);
    }
    else if (sscanf(line,"%3d--%3d--%4d%c",&num1 , &num2,&num3 ,&nl)==4 && nl=='\n'){
        printf("(%d) %d-%d\n",num1,num2,num3);
    }
    else if (sscanf(line,"..%3d..%3d..%4d..%c",&num1 , &num2,&num3 ,&nl)==4 && nl=='\n'){
        printf("(%d) %d-%d\n",num1,num2,num3);
    }
    else if (sscanf(line,"%3d%3d%4d%c",&num1 , &num2,&num3 ,&nl)==4 && nl=='\n' ){
        if (num3<=999){
            printf("Invalid input\n");
        }
        else{
            printf("(%d) %d-%d\n",num1,num2,num3);
        }
    }
    else{
        printf("Invalid input\n");
    }
}