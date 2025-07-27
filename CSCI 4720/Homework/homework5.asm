# Title: homework5.asm
# Author: Swapnil Roy
# Date: 7/20/2025
# Purpose: Compare two single precision floating point numbers. If numA > numB, swap the numbers.

.data
numA:           .float 6.8                  # float numA
numB:           .float 5.5                  # float numB
InitA:          .asciiz "numA "            # print prompt for pre-swap array
InitB:          .asciiz "numB: "           # print prompt for post-swap array
ResultA:        .asciiz "Result numA "     # print prompt for pre-swap array
ResultB:        .asciiz "Result numB: "    # print prompt for post-swap array
Newline:        .asciiz "\n"               # print a newline character
Comma:          .asciiz ", "               # print a comma and space character

.text
main:
# Load Given Data
    lwc1 $f0, numA                          # $f0 = numA
    lwc1 $f1, numB                          # $f1 = numB

# Print Initial numA And numB
    li $v0, 4                               # syscall for print_str
    la $a0, InitA                           # $a0 = address of string to print
    syscall                                 # call print_str(InitA)
    mov.s $f12, $f0                         # move numA from $f0 to $f12
    li $v0, 2                               # syscall for print_float
    syscall                                 # call print_float(numA)
    li $v0, 4                               # syscall for print_str
    la $a0, Comma                           # $a0 = address of string to print
    syscall                                 # call print_str(Comma)
    li $v0, 4                               # syscall for print_str
    la $a0, InitB                           # $a0 = address of string to print
    syscall                                 # call print_str(InitB)
    mov.s $f12, $f1                         # move numB from $f1 to $f12
    li $v0, 2                               # syscall for print_float
    syscall                                 # call print_float(numB)
    li $v0, 4                               # syscall for print_str
    la $a0, Newline                         # $a0 = address of string to print
    syscall                                 # call print_str(Newline)

# Swap Condition Logic
    c.lt.s $f1, $f0                         # is numB < numA?
    bc1t swap                               # if (numA > numB), execute 'swap'
    j exit                                  # jump to 'exit'

swap:
# Swap Logic
    swc1 $f1, numA                          # store numA in $f1 
    swc1 $f0, numB                          # store numB in $f0

exit:
# Reload swapped values from memory
    lwc1 $f0, numA                          # $f1 = numA
    lwc1 $f1, numB                          # $f0 = numB

# Print Final numA And numB
    li $v0, 4                               # syscall for print_str
    la $a0, ResultA                         # $a0 = address of string to print
    syscall                                 # call print_str(ResultA)
    mov.s $f12, $f0                         # move numA from $f0 to $f12
    li $v0, 2                               # syscall for print_float
    syscall                                 # call print_float(numA)
    li $v0, 4                               # syscall for print_str
    la $a0, Comma                           # $a0 = address of string to print
    syscall                                 # call print_str(Comma)
    li $v0, 4                               # syscall for print_str
    la $a0, ResultB                         # $a0 = address of string to print
    syscall                                 # call print_str(ResultB)
    mov.s $f12, $f1                         # move numB from $f1 to $f12
    li $v0, 2                               # syscall for print_float
    syscall                                 # call print_float(numB)

# Exit
    li $v0, 10                          # load system.exit function
    syscall                             # call system.exit(0)