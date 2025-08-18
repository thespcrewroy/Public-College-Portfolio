# Title: module5.11.asm
# Author: Swapnil Roy
# Date: 7/20/2025
# Purpose: Calculate ax^2 + bx + c.

.data
a:          .float 1.0                                   # ax^2
bb:         .float 2.0                                   # bx ('b' is reserved as 'branch' in MIPS, so we use 'bb')
c:          .float 3.0                                   # c
Prompt:     .asciiz "Enter x: "                         # prompt for the 'x' floating point domain value
Result:     .asciiz "ax^2 + bx + c = x^2 + 2x + 3 = "   # result prompt using hardcoded variable name data

.text
main:
# Print String Prompt
    li $v0, 4                   # syscall for print_str
    la $a0, Prompt              # $a0 = address of string to print
    syscall                     # call print_str(Prompt)

# Read Float From Stdin
    li $v0, 6                   # syscall for read_float(stdin)
    syscall                     # ($f0 = x) = read_float(stdin)

# Load Given Data
    l.s $f1, a                  # $f1 = a
    l.s $f2, bb                 # $f2 = b
    l.s $f3, c                  # $f3 = c

# Create Quadratic Equation: ax^2 + bx + c
    mul.s $f4, $f0, $f0         # $f4 = x^2
    mul.s $f5, $f1, $f4         # $f5 = ax^2
    mul.s $f6, $f2, $f0         # $f6 = bx
    add.s $f7, $f5, $f6         # $f7 = ax^2 + bx
    add.s $f7, $f7, $f3         # $f7 = ax^2 + bx + c

# Print Result Prompt
    li $v0, 4                   # syscall for print_str
    la $a0, Result              # $a0 = address of string to print
    syscall                     # call print_str(Result)

# Print Result
    mov.s $f12, $f7             # move $f7 into printing register $f12
    li $v0, 2                   # syscall for print_float()
    syscall                     # call print_float($f7)

# Exit
    li $v0, 10                  # load system.exit function
    syscall                     # call system.exit(0)