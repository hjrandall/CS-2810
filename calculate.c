#include <stdio.h>
void calculate(char *line){
    char nl;
    int num1,num2,total;
    if (sscanf(line,"What is %d added to %d?%c",&num1 , &num2, &nl)==3 && nl=='\n'){
       total=num1+num2;
        printf("%d + %d = %d\n",num1,num2,total );
    }
    else if(sscanf(line,"What is %d subtracted from %d?%c",&num1 , &num2, &nl)==3 && nl=='\n'){
        total=num2-num1;
        printf("%d - %d = %d\n",num2,num1,total );
    }
    else if(sscanf(line,"What is %d times %d?%c",&num1 , &num2, &nl)==3 && nl=='\n'){
        total=num1*num2;
        printf("%d * %d = %d\n",num1,num2,total );
    }
    else if(sscanf(line,"What is %d divided by %d?%c",&num1 , &num2, &nl)==3 && nl=='\n'){
        if (num2!=0){
            total=num1/num2;
            printf("%d / %d = %d\n",num1,num2,total );
        }
        else{
            printf("Division by zero is undefined\n");
        }
    }
    else if(sscanf(line,"What is the remainder of %d divided by %d?%c",&num1 , &num2, &nl)==3 && nl=='\n'){
        if (num2!=0){
            total=num1%num2;
            printf("%d %% %d = %d\n",num1,num2,total );
        }
        else{
            printf("Division by zero is undefined\n");
        }
    }
    else if(sscanf(line,"What is %d negated?%c",&num1, &nl)==2 && nl=='\n'){
        total=num1*(-1);
        printf("-(%d) = %d\n",num1,total );
    }
    else if(sscanf(line,"What is %d squared?%c",&num1, &nl)==2 && nl=='\n'){
        total=num1*num1;
        printf("(%d)^2 = %d\n",num1,total );
    }
    else if(sscanf(line,"Which is larger: %d or %d?%c",&num1,&num2, &nl)==3 && nl=='\n'){
       if(num1==num2){
           printf("Both values are the same\n");

       }
       else if (num1>num2){
            printf("%d is larger than %d\n",num1,num2);
        }
        else{
            printf("%d is larger than %d\n",num2,num1);
        }
    }
    else{
        printf("I do not understand the question, sorry!\n");
    }
}