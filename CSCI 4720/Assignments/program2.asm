# Title: program2.asm
# Author: Swapnil Roy
# Date: 6/29/2025
# Purpose: max(array)

.data
array:      .word 10, 3, 45, 90, 12             # initialize array
Length:     .word 5                             # initialize array.length
Result:     .asciiz "Max_Array: "               # initialize result prompt
Max:    .word 0                                 # store max permamnently in a variable

.text
main:
# Load Given Data
    la $t0, array          # $to = array
    lw $t7, Length         # $t7 = array.length
    li $t8, 1              # $t8 = (i = 1)
    lw $t1, 0($t0)         # $t1 = (max = array[0])

loop:
# Loop Logic
    addi $t0, $t0, 4       # byte addressing problem: array(4i)
    lw $t2, 0($t0)         # $t2 = array[i]
    ble $t2, $t1, skip     # if (array[i] < max)
    move $t1, $t2          # else, max = array[i]
    
skip:
# Next Iteration For Loop
    addi $t8, $t8, 1       # i++
    blt $t8, $t7, loop     # if (i < array.length), restart 'loop'

# Print Message
    li $v0, 4              # syscall for print_str
    la $a0, Result         # $a0 = address of string to print
    syscall                # print_str(Result)

# Print Max Integer
    sw $t1, Max            # store the value of $t1 in the data element 'Max'
    li $v0, 1              # syscall for print_int
    move $a0, $t1          # move the max to argument from $t1 to argument register $a0
    syscall                # print_int(max)

# Exit
    li $v0, 10             # syscall for Exit
    syscall                # execute the system.exit()
