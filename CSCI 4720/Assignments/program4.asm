# Title: program4.asm
# Author: Swapnil Roy
# Date: 7/25/2025
# Purpose: arrayC[i] = arrayA[i] - arrayB[i]. Both arrays are single precision arrays. Result is saved in arrayC and printed to standard output.

.data
arrayA: .float 10.5, 15.6, 17.9, 25.3, 12.5      # arrayA
arrayB: .float 4.1, 16.5, 7.9, 15.2, 22.6        # arrayB
arrayC: .float 0.0, 0.0, 0.0, 0.0, 0.0           # arrayC
Length: .word 5                                 # array.length
Result: .asciiz "Result is "                    # result prompt
Lbrack: .asciiz "["                             # left bracket
Rbrack: .asciiz "]\n"                           # right bracket
Comma:  .asciiz ", "                            # comma

.text
main:
# Load Given Data
    la $t0, arrayA           # $t0 = arrayA[0]
    la $t1, arrayB           # $t1 = arrayB[0]
    la $t2, arrayC           # $t2 = arrayC[0]
    lw $t7, Length           # $t7 = array.length
    li $t8, 0                # $t8 = (i = 0)

# Print Prompt And Array Starting Bracket
    li $v0, 4                # syscall for print_str
    la $a0, Result           # $a0 = address of string to print
    syscall                  # print_str(Result)

    li $v0, 4                # syscall for print_str
    la $a0, Lbrack           # $a0 = address of string to print
    syscall                  # print_str(Lbrack)

loop:
# Loop Logic
    lwc1 $f0, 0($t0)         # $f0 = arrayA[i]
    lwc1 $f1, 0($t1)         # $f1 = arrayB[i]
    sub.s $f3, $f0, $f1      # $f3 = arrayA[i] - arrayB[i]
    swc1 $f3, 0($t2)         # arrayC[i] = arrayA[i] - arrayB[i]

# Print arrayC
    mov.s $f12, $f3          # move arrayC[i] from $t5 to argument $f12
    li $v0, 2                # syscall for print_float
    syscall                  # call print_float(arrayC[i])

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
    addi $t0, $t0, 4         # byte addressing problem: arrayA(4i)
    addi $t1, $t1, 4         # byte addressing problem: arrayB(4i)
    addi $t2, $t2, 4         # byte addressing problem: arrayC(4i)
    j loop                   # restart 'loop'

exit:
# Right Bracket Signifies Array End
    li $v0, 4                # syscall for print_str
    la $a0, Rbrack           # $a0 = address of the string to print
    syscall                  # print_str(Rbrack)

# Exit
    li $v0, 10               # syscall for exit
    syscall                  # call system.exit(0)