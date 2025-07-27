# Title: quiz4.asm
# Author: Swapnil Roy
# Date: 7/16/2025
# Purpose: Procedure that calculates the number of negative numbers in an array of integers

.data
array:   .word -1, -2, 5, 4, 20         # array
Length:  .word 5                        # array.length
Result:  .asciiz "Negative Count: "     # result prompt print
Count:   .word 0                        # variable 'count'

.text
main:
# Load Given Data
    la $a1, array                       # $a1 = array[0]
    lw $a2, Length                      # $a2 = array.length

# Call Procedure
    jal count                           # call procedure function 'count'
    move $t6, $v1                       # move the count from result register $v1 into register $t6

# Print String Prompt
    li $v0, 4                            # syscall for print_str
    la $a0, Result                       # $a0 = address of string to print
    syscall                              # call print_str(Result)

# Print Count
    li $v0, 1                            # syscall for print_int
    move $a0, $t6                        # move the count from register $t6 into argument register $a0
    syscall                              # call print_int(count)

# Exit
    li $v0, 10                          # load system.exit function
    syscall                             # call system.exit(0)

count:
# Stack Allocation
    addi $sp, $sp, -8                   # allocate 2 indices in the stack
    sw $t0, 0($sp)                      # point $t0 to index 1 in the stack
    sw $t1, 4($sp)                      # point $t1 to index 0 in the stack

    li $t0, 0                           # $t0 = (i = 0)
    li $t1, 0                           # $t1 = (count = 0)

loop:
# Loop Conditional Check
    beq $t0, $a2, exit                  # if (i >= array.length), execute 'exit'

# Loop Logic
    lw $t3, 0($a1)                      # $t3 = array[i]
    slt $t4, $t3, $zero                 # if (array[i] <= 0), $t4 = 1
    beq $t4, $zero, next                # if (array[i] > 0), execute 'skip'
    addi $t1, $t1, 1                    # count++

next:
# Next Iteration For Loop
    addi $a1, $a1, 4                    # byte addressing problem: array(4i)
    addi $t0, $t0, 1                    # i++
    j loop                              # restart 'loop'

exit:
# Exitloop Logic
    move $v1, $t1                       # move 'count' from $t1 to result register $v1

# Restore Saved Registers
    lw $t0, 0($sp)                      # load index 1 from the stack
    lw $t1, 4($sp)                      # load index 0 from the stack
    addi $sp, $sp, 8                    # deallocate 2 indices from the stack

# Exit Procedure
    jr $ra                              # return to caller (jal count)
