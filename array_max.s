                .global array_max
                .text

# int array_max(int *array, int count)

#a0: array
#a1: count
#a2: i
#a3: largest_num

#t2: element

array_max:
                    li      a2, 0                   #int i=0
                    lw      a3, (a0)                #int largest_num=array[0]
                    j       3f
1:                                                  #if ()
                    slli    t1, a2,  2              #|
                    add     t1, a0,  t1             #|array[i]    
                    lw      t2, (t1)                #|              
                    blt     t2, a3,  2f             #<largest_num               
                    mv      a3, t2                  #largest_num=array[i]
2:
                    addi    a2, a2,  1             #i++                      
3:                                               
                    blt     a2, a1,  1b             #while(i<count)
                    mv      a0, a3                  #return largest_num
                    ret     

