# Title: quiz3.2.asm
# Author: Swapnil Roy
# Date: 7/10/2025
# Purpose: Swap 2nd element of an array with 3rd element of the array.

.data
array:      .word 5, 10, -9, 15, 7      # array of integers
Length:     .word 5                     # length of the array
Init:       .asciiz "Array: "           # print prompt for pre-swap array
Result:     .asciiz "Swap: "            # print prompt for post-swap array
Newline:    .asciiz "\n"                # print a newline character
Space:      .asciiz " "                 # print a space character

.text
main:
# Load Given Data
    la $s0, array                       # $s0 = array[0]
    lw $t2, Length                      # $t2 = array.length

# Print Prompt
    li $v0, 4                           # syscall for print_str
    la $a0, Init                        # $a0 = address of string to print
    syscall                             # call print_str(Init)

# Differentiate Pre-Swap vs. Post-Swap Array
    li $t6, 0                           # set a flag (swap = 0)

beforeloop:
# Load Iterable for Loop Logic
    li $t1, 0                           # $t1 = (i = 0)

loop:
# Loop Conditional Check            
    beq $t1, $t2, swap                  # if (i >= array.length), jump to 'swap'

# Loop Logic
    sll $t3, $t1, 2                     # byte addressing problem: $t3 = 4i
    add $t4, $s0, $t3                   # $t4 = array(4i)
    lw $t5, 0($t4)                      # $t5 = array[i]

# Print Integer
    li $v0, 1                           # syscall for print_int
    move $a0, $t5                       # move integer from array[i] in $t5 to argument register $a0
    syscall                             # call print_int(array[i])

# Print Space
    li $v0, 4                           # syscall for print_str
    la $a0, Space                       # $a0 = address of string to print
    syscall                             # call print_str(Space)

# Next Iteration For Loop
    addi $t1, $t1, 1                    # i++
    j loop                              # restart the loop
    
swap:
# Check Flag To Prevent Infinite Loop
    bne $t6, $zero, exit                # if (swap == 1), then jump to 'exit'

# Print Newline
    li $v0, 4                           # syscall for print_str
    la $a0, Newline                     # $a0 = address of string to print
    syscall                             # call print_str(Newline)

# Load the Given Data
    lw $s1, 4($s0)                       # $s1 = array[1]
    lw $s2, 8($s0)                       # $s2 = array[2]

# Swap Elements
    sw $s1, 8($s0)                       # array[2] = array[1]
    sw $s2, 4($s0)                       # array[1] = array[2]

# Print Prompt
    li $v0, 4                           # syscall for print_str
    la $a0, Result                      # $a0 = address of string to print
    syscall                             # call print_str(Result)

# Restart Loop
    li $t6, 1                           # set a flag (swap = 1)
    j beforeloop                        # restart the loop to 

exit:
# Exit
    li $v0, 10                          # load system.exit function
    syscall                             # call system.exit(0)