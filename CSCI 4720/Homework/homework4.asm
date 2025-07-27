# Title: homework4.asm
# Author: Swapnil Roy
# Date: 7/13/2025
# Purpose: Procedure to calculate the average of the integers within arrays.

.data
array:   .word 10, 25, 34, 63           # array of four numbers
Length:  .word 4                        # array.length
Result:  .asciiz "Average is: "         # result prompt
Newline: .asciiz "\n"                   # newline character

.text
main:
# Load Given Data
    la $a1, array                        # $a1 = array[0]
    lw $a2, Length                       # $a2 = array.length

# Call Procedure
    jal average                          # call procedure function 'average'
    move $t6, $v0                        # move the average from result register $v0 into register $t6

# Print String Prompt
    li $v0, 4                            # syscall for print_str
    la $a0, Result                       # $a0 = address of string to print
    syscall                              # call print_str(Result)

# Print Average
    li $v0, 1                            # syscall for print_int
    move $a0, $t6                        # move the average from register $t6 into argument register $a0
    syscall                              # call print_int(average)

# Exit
    li $v0, 10                          # load system.exit function
    syscall                             # call system.exit(0)

average:
# Stack Allocation
    addi $sp, $sp, -8                   # allocate 2 indices in the stack
    sw $t0, 0($sp)                      # point $t0 to index 1 in the stack
    sw $t1, 4($sp)                      # point $t1 to index 0 in the stack

    li $t0, 0                           # $t0 = (sum = 0)
    li $t1, 0                           # $t1 = (i = 0)

loop:
# Loop Conditional Check
    beq $t1, $a2, exit                  # if (i >= array.length), execute 'exit'

# Loop Logic
    sll $t3, $t1, 2                     # byte addressing problem: 4i
    add $t4, $a1, $t3                   # $t4 = array(4i)
    lw $t5, 0($t4)                      # $t5 = array[i]
    add $t0, $t0, $t5                   # sum += array[i]

# Next Iteration For Loop
    addi $t1, $t1, 1                    # i++
    j loop                              # restart 'loop'

exit:
# Exitloop Logic
    div $t0, $a2                        # sum = sum / array.length
    mflo $v0                             # $v0 = sum % array.length

# Restore Saved Registers
    lw $t0, 0($sp)                      # load index 1 from the stack
    lw $t1, 4($sp)                      # load index 0 from the stack
    addi $sp, $sp, 8                    # deallocate 2 indices from the stack

# Exit Procedure
    jr $ra                              # return to caller (jal average)
