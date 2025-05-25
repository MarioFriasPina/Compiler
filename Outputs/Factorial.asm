	.globl main
	.data
	.text
main:
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the frame pointer
	addiu $sp $sp -4 # Allocate space in the stack for the parameter
	jal main_entry
	li $v0, 10
	syscall
input_entry:
	move $fp $sp
	sw $ra 0($sp)
	addiu $sp $sp -4
	li $v0 5 # Read an integer
	syscall
	move $a0 $v0 # Save the input in $a0
	lw $ra 4($sp)
	addiu $sp $sp 8
	lw $fp 0($sp)
	jr $ra
output_entry:
	move $fp $sp
	sw $ra 0($sp)
	addiu $sp $sp -4
	lw $a0 8($sp) # Load the parameter to $a0
	li $v0 1 # Print the parameter
	syscall
	li $v0 11 # Print a newline char
	li $a0 10
	syscall
	lw $ra 4($sp)
	addiu $sp $sp 12
	lw $fp 0($sp)
	jr $ra
factorial_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
# Start of if statement if_line_2
	lw $a0 4($fp) # Load the value of n from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 0 # Load constant into $a0
	lw $t1 4($sp)
	seq $a0 $t1 $a0 # EQUAL
	addiu $sp $sp 4
	beq $a0 1 true_if_line_2 # Branch if condition is true
false_if_line_2:
	lw $a0 4($fp) # Load the value of n from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
		 # Call function factorial
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	lw $a0 4($fp) # Load the value of n from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 1 # Load constant into $a0
	lw $t1 4($sp)
	sub $a0 $t1 $a0 # SUBSTRACT
	addiu $sp $sp 4
	sw $a0 0($sp) # Save the value of n in the stack
	addiu $sp $sp -4
	jal factorial_entry
	lw $t1 4($sp)
	mul $a0 $t1 $a0 # MULTIPLY
	addiu $sp $sp 4
	b end_if_line_2 # Branch to the end of the if statement
true_if_line_2:
	li $a0 1 # Load constant into $a0
end_if_line_2:
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 12 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
main_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
		 # Call function input
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	jal input_entry
	sw $a0 4($fp) # Save $a0 as the new value of n in the stack
		 # Call function output
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
		 # Call function factorial
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	lw $a0 4($fp) # Load the value of n from the stack
	sw $a0 0($sp) # Save the value of n in the stack
	addiu $sp $sp -4
	jal factorial_entry
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	jal output_entry
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 12 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
