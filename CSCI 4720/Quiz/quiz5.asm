# Title: quiz5.asm
# Author: Swapnil Roy
# Date: 7/20/2025
# Purpose: Find the number of positive numbers in a double precision array. Save result in $count.

.data
array:      .double 1.2, 1.0, -5.1, 2.0, 6.0, 7.1, -8.1, 9.1, 4.0, -1.1     # array of doubles
Length:     .word 10                # number of elements
Count:      .word 0                 # counter storage
Result:     .asciiz "Pos_Count: "   # result prompt print

.text
main:
# Load Given Data
    la $s1, array                   # $s1 = array[0]
    lw $t1, Length                  # $t1 = array.length
    li $t4, 0                       # $t4 = counter
    li.d $f0, 0.0                   # $f0 = 0.0 (used for comparison to check if number is negative)
    li $t0, 0                       # $t0 = (i = 0)

loop:
# Loop Conditional Check
    beq $t0, $t1, exit              # if (i >= array.length), execute 'exit'
    l.d $f2, 0($s1)                 # $f2 = array[i]
    c.lt.d $f2, $f0                 # send 'if (array[i] < 0.0)' to bc1t as the default 'true'
    bc1t skip                       # if (array[i] < 0.0), execute 'skip'
    addi $t4, $t4, 1                # else, counter++

skip:
# Next Iteration For Loop
    addi $s1, $s1, 8                # $s1 = array(4i)
    addi $t0, $t0, 1                # i++
    j loop

exit:
# Store Immediates
    sw $t4, Count                   # store the value for '$t4' in 'Counter'

# Print String Prompt
    li $v0, 4                       # syscall for print_str
    la $a0, Result                  # $a0 = address of string to print
    syscall                         # call print_str(Result)

# Print Count
    move $a0, $t4                   # move the count from register $t4 into argument register $a0
    li $v0, 1                       # syscall for print_int
    syscall                         # call print_int(Count)

# Exit
    li $v0, 10                      # load system.exit function
    syscall                         # call system.exit(0)
