# Title: homework3.asm
# Author: Swapnil Roy
# Date: 7/10/2025
# Purpose: Iterates through 2 list of numbers, adds them together into a sum, and places the value '10'
#          into that index 'sum' so that we receive a large array that all contain the value '10'.

.data
arrayD:  .space 80              # empty array of 80 indices
val_a:   .word 2                # the variable 'a'
val_b:   .word 3                # the variable 'b'
Result:  .asciiz "arrayD: "     # the 'result' string

.text
main:
# Load Given Data
    lw $s0, val_a               # loads the value 2 (not the address like with 'la')
    lw $s1, val_b               # loads the value 3 (not the address like with 'la')
    la $s2, arrayD              # $s2 = arrayD[0]
    li $t7, 10                  # load the value '10'

# Print String Prompt
    li $v0, 4                   # syscall for print_str
    la $a0, Result              # $a0 = address of string to print
    syscall                     # call print_str(Result)

# Load Iterable for Loop1
    li $t0, 0                   # i = 0

loop1:
# Loop Conditional Check
    slt $t2, $t0, $s0           # check if (i < a)
    beq $t2, $zero, exitloop1   # if (i >= a), then exit the outer loop

# Load Iterable For Loop2
    li $t1, 0                   # $t1 = j

loop2:
# Loop Conditional Check
    slt $t3, $t1, $s1           # check if (j < b)
    beq $t3, $zero, exitloop2   # if (j >= b), then exit the inner loop

# Loop Logic  
    add $t6, $t0, $t1           # $t6 = i + j
    sll $t6, $t6, 2             # byte addressing problem: 4(i + j)
    add $t5, $t6, $s2           # $t5 = BaseOfarrayD + 4(i + j)
    sw $t7, 0($t5)              # arrayD[i + j] = 10

# Print Array
    li $v0, 1                   # syscall for print_int
    move $a0, $t7               # move the value '10' from $t7 to argument register $a0
    syscall                     # call print_int(10)

# Next Iteration: Loop 2
    addi $t1, $t1, 1            # j++
    j loop2                     # restart loop2

exitloop2:
# Next Iteration: Loop 1
    addi $t0, $t0, 1            # i++
    j loop1                     # restart loop1

exitloop1:
    li $v0, 10                  # load system.exit function
    syscall                     # call system.exit(0)