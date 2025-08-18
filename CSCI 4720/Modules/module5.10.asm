# Title: module5.10.asm
# Author: Swapnil Roy
# Date: 7/20/2025
# Purpose: Compare two single precision numbers. If num1 < num2, 
#           print 'condition is true'. Else, print 'condition is false'.

.data
num1:           .float 2.0                           # spfp number num1
num2:           .float 5.0                           # spfp number num2
True:           .asciiz "condition is True."        # print 'condition is True'
False:          .asciiz "condition is False."       # print 'condition is False'

.text
main:
# Load Given Data
    l.s $f0, num1               # $f0 = num1
    l.s $f1, num2               # $f1 = num2

# Compare Float Nmbers
    c.lt.s $f0, $f1             # if (num1 < num2)
    bc1t yes                    # if (num1 < num2), execute 'yes'
    j no                        # else, execute 'no'

no:
# Print False Condition
    li $v0, 4                   # syscall for print_str
    la $a0, False               # $a0 = address of string to print
    syscall                     # call print_str(False)
    j exit                      # ignore 'yes' label and jump to 'exit'

yes:
# Print True Condition
    li $v0, 4                   # syscall for print_str
    la $a0, True                # $a0 = address of string to print
    syscall                     # call print_str(True)

exit:
# Exit
    li $v0, 10                  # load system.exit function
    syscall                     # call system.exit(0)