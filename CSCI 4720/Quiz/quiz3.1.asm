# Title: quiz3.1.asm
# Author: Swapnil Roy
# Date: 7/10/2025
# Purpose: Counts the number of even numbers in an array and stores the result in a variable 'count'.

.data
array:      .word 5, 8, -9, 10, 12, 1   # array of integers
Length:     .word 6                     # length of the array
Count:      .word 0                     # variable to store the count of even numbers
Result:     .asciiz "count: "           # result prompt

.text
main:
# Load Given Data
    la $t0, array                       # $t0 = array[0]
    lw $s0, Length                      # $s0 = array.length
    li  $t1, 0                          # $t1 = 0 (count)

# Load Iterable For Loop
    li $s1, 0                           # i = 0
    li $t2, 2                           # $t2 = 2 (for checking even)

loop:
# Loop Conditional Check
    beq $s1, $s0, exit                  # if (i >= array.length), exit the loop

# Loop Logic
    lw $s2, 0($t0)                      # $s2 = array[i]
    div $s2, $t2                        # $s2 = array[i] / 2
    mfhi $s3                            # $s3 = array[i] % 2
    bne $s3, $zero, skip                # if ($s3 != 0, i.e. odd), execute 'skip'

# Next Iteration For Loop
    addi $t1, $t1, 1                    # i++

skip:
# Next Iteration For Loop
    addi $t0, $t0, 4                    # byte addressing problem: 4i
    addi $s1, $s1, 1                    # i++
    j loop                              # restart loop

exit:
# Print Logic
    li $v0, 4                           # syscall for print_str
    la $a0, Result                      # $a0 = address of string to print
    syscall                             # call print_str(Result)

    li $v0, 1                           # syscall for print_int
    move $a0, $t1                       # move address of integer from $t1 to $a0
    syscall                             # call print_int(count)

    sw $t1, Count                       # store the value of $t1 in the data element 'Count'

# Exit
    li $v0, 10                          # load system.exit function
    syscall                             # call system.exit(0)