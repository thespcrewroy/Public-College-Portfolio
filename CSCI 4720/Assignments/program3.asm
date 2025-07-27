# Title: program3.asm
# Author: Swapnil Roy
# Date: 7/20/2025
# Purpose: Searches for a number that is entered by user in an array with N integer numbers
#          and prints the index of the number in the array if it is found and -1 if not found.


.data
array:          .word 10, 11, -3, -10, 50               # array to hold integers
Length:         .word 5                                 # length of the array
SearchNumber:   .word 100                               # number to perform linear search for
Result:         .asciiz "Index of Desired Number: "     # resulting string prompt

.text
main:
# Load Given Data
    la $t0, array               # $t0 = array[0]
    lw $t1, Length              # $t1 = array.length
    lw $t2, SearchNumber        # $t2 = SearchNumber

# Load Iterable for Loop
    li $t3, 0                   # $t3 = (i = 0)

loop:
# Loop Conditional Check
    beq $t3, $t1, notfound      # if (i >= array.length), execute 'notfound'

# Loop Logic
    sll $t5, $t3, 2             # byte addressing problem: 4i
    add $t6, $t0, $t5           # $t6 = array(4i)
    lw $t4, 0($t6)              # $t4 = array[i]
    beq $t4, $t2, found         # if (array[i] == SearchNumber), execute 'found'

# Next Iteration For Loop
    addi $t3, $t3, 1            # i++
    j loop                      # restart loop

found:
# Print Message
    li $v0, 4                   # syscall for print_str
    la $a0, Result              # $a0 = address of string to print
    syscall                     # call print_str(Result)

# Print Index
    li $v0, 1                   # syscall for print_int
    move $a0, $t3               # move iterator from $t3 to argument register $a0
    syscall                     # call print_int(i)

# 'Break' Out of Loop
    j exit                      # jump to 'exit'

notfound:
# Print Message
    li $v0, 4                   # syscall for print_str
    la $a0, Result              # $a0 = address of string to print
    syscall                     # call print_str(Result)

# Print -1
    li $v0, 1                   # syscall for print_int
    li $a0, -1                  # load '-1' into the argument to print_int
    syscall                     # call print_int(-1)

exit:
# Exit
    li $v0, 10                  # load system.exit function
    syscall                     # call system.exit(0)
