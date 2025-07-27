# Title: program1.asm
# Author: Swapnil Roy
# Date: 6/29/2025
# Purpose: arrayC[i] = arrayA[i] - arrayB[i]


.data
arrayA: .word 10, 5, 6, 15, 17, 19, 25, 30, 12, 56  # arrayA
arrayB: .word 4, 15, 16, 5, 7, 9, 15, 10, 22, 6     # arrayB
arrayC: .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0          # arrayC
Length: .word 10                                    # array.length
Result: .asciiz "Result is "                        # result prompt
Lbrack: .asciiz "["                                 # left bracket
Rbrack: .asciiz "]\n"                               # right bracket
Comma:  .asciiz ", "                                # comma

.text
main:
# Load Given Data
    la $t0, arrayA           # $t0 = arrayA[0]
    la $t1, arrayB           # $t1 = arrayB[0]
    la $t2, arrayC           # $t2 = arrayC[0]
    lw $t7, Length           # $t7 = array.length
    li $t8, 0                # $t8 = (i = 0)

# Print Message
    li $v0, 4                # syscall for print_str
    la $a0, Result           # $a0 = address of string to print
    syscall                  # print_str(Result)

# Print Left Bracket
    li $v0, 4                # syscall for print_str
    la $a0, Lbrack           # $a0 = address of string to print
    syscall                  # print_str(Lbrack)

loop:
# Loop Logic
    lw $t3, 0($t0)           # $t3 = arrayA[i]
    lw $t4, 0($t1)           # $t4 = arrayB[i]
    sub $t5, $t3, $t4        # $t5 = arrayA[i] - arrayB[i]
    sw $t5, 0($t2)           # arrayC[i] = arrayA[i] - arrayB[i]

# Print arrayC
    li $v0, 1                # syscall for print_int
    move $a0, $t5            # move arrayC[i] from $t5 to argument register $a0
    syscall                  # call print_int(arrayC[i])

# Load Iterable For Loop
    addi $t8, $t8, 1         # i++
    blt $t8, $t7, comma      # if (i < array.length), execute 'comma'
    j exit                   # else, skip to 'exit'

comma:
# Comma Between Integers
    li $v0, 4                # syscall for print_str
    la $a0, Comma            # $a0 = address of string to print
    syscall                  # print_str(Comma)
    
# Next Iteration Loop
    addi $t0, $t0, 4          # byte addressing problem: arrayA(4i)
    addi $t1, $t1, 4          # byte addressing problem: arrayB(4i)
    addi $t2, $t2, 4          # byte addressing problem: arrayC(4i)
    j loop                    # restart 'loop'

exit:
# Right Bracket Signifies Array End
    li $v0, 4                # syscall for print_str
    la $a0, Rbrack           # $a0 = address of the string to print
    syscall                  # print_str(Rbrack)

# Exit
    li $v0, 10               # syscall for exit
    syscall                  # call system.exit(0)
