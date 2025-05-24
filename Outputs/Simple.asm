	.globl main
	.data
	.text
main:
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	li $a0 0 # Set the parameter to an initial value of 0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	li $a0 0 # Set the parameter to an initial value of 0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	jal main_entry
	li $v0, 10
	syscall
input_entry:
	move $fp $sp
	sw $ra 0($sp)
	addiu $sp $sp -4
	li $v0, 5 # Read an integer
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
	li $v0, 1 # Print the parameter
	syscall
	lw $ra 4($sp)
	addiu $sp $sp 12
	lw $fp 0($sp)
	jr $ra
main_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
		 # Call function input
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	jal input_entry
	sw $a0 8($fp) # Save $a0 as the new value of x in the stack
	lw $a0 8($fp) # Load the value of x from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 10 # Load constant into $a0
	lw $t1 4($sp)
	add $a0 $t1 $a0 # ADD
	addiu $sp $sp 4
	sw $a0 4($fp) # Save $a0 as the new value of y in the stack
		 # Call function output
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	lw $a0 4($fp) # Load the value of yfrom the stack
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	jal output_entry
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 16 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
