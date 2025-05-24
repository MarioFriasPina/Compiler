	.globl main
	.data
		x: .space 80
	.text
main:
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the frame pointer
	li $a0 0 # Set the parameter to an initial value of 0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the next parameter
	li $a0 0 # Set the parameter to an initial value of 0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the next parameter
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
	lw $a0 4($sp) # Load the parameter in $a0
	li $v0, 1 # Print the parameter
	syscall
	lw $ra 4($sp)
	addiu $sp $sp 12
	lw $fp 0($sp)
	jr $ra
minimo_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space for return address
	li $a0 0 # Load constant into $a0
	sw $a0 0($fp)
	lw $a0 0($fp) # Load the value of afrom the stack
	sw $a0 0($fp)
	li $a0 1 # Load constant into $a0
	sw $a0 0($fp)
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 28 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
main_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space for return address
	li $a0 0 # Load constant into $a0
	sw $a0 0($fp)
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the frame pointer
	jal input_entry
	sw $a0 0($fp)
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the frame pointer
	li $a0 0 # Set the parameter to initial value of 0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the next parameter
	jal output_entry
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 16 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
