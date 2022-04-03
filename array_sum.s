                .global array_sum
                .text

# int array_sum(int *array, int count)
array_sum:
#a0: array
#a1: count

#s1: array
#s2: count
#s3: array_sum
#s4: i 
#s5: elt
                addi  sp,  sp,  -24         # save s registers
                sw    ra, 20(sp)
                sw    s5, 16(sp)
                sw    s4, 12(sp)
                sw    s3, 8(sp)
                sw    s2, 4(sp)
                sw    s1, 0(sp)

                mv    s1,  a0               # move array to a s register
                mv    s2,  a1               # move count to a s register
                li    s3,  0                # sum=0
                li    s4,  0                # i=0
                
                j     3f
1:
                slli  t0,  s4,  2
                add   t0,  s1,  t0         # get i[element]
                lw    s5,  (t0)
                
                mv    a0,  s5              # set element as parameter for function call
                la    a7, predicate        # load adrres of the function
                jalr  a7                   # call function

                beqz  a0,  2f              # check the return value 
                add   s3,  s3,  s5         # add element
2:              
                addi  s4,  s4,   1
3:              
                blt   s4,  s2,  1b         # i<count
                mv    a0,  s3              # put the sum in the return register a0

                lw    ra, 20(sp)           # restore values
                lw    s5, 16(sp)
                lw    s4, 12(sp)
                lw    s3, 8(sp)
                lw    s2, 4(sp)
                lw    s1, 0(sp)
                addi  sp,  sp,  24

                ret



