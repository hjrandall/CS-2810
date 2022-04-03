#include <stdio.h>
void print_diamond(int size) {
    int c_ount;
    int k_ount;
    for (k_ount = 1; k_ount <= size; k_ount++){
        for (c_ount = 1; c_ount <= size-k_ount; c_ount++){
            printf(" ");
        }
        for (c_ount = 1; c_ount <= 2*k_ount-1; c_ount++){
            printf("*");
        }
        printf("\n");
    }

    for (k_ount = 1; k_ount<= size - 1; k_ount++){
        for (c_ount = 1; c_ount <= k_ount; c_ount++){
            printf(" ");
        }

        for (c_ount = 1 ; c_ount <= 2*(size-k_ount)-1; c_ount++){
            printf("*");
        }

        printf("\n");
    }
}