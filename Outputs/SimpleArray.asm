	.globl main
	.data
	.text
main:
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the frame pointer
	addiu $sp $sp -4 # Allocate space in the stack for the parameter
	addiu $sp $sp -80 # Allocate space in the stack for the parameter
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
recieve_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
	li $a0 0 # Load constant into $a0
	sw $a0 8($fp) # Save $a0 as the new value of i in the stack
while_line_5_entry:
	lw $a0 8($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 20 # Load constant into $a0
	lw $t1 4($sp)
	slt $a0 $a0 $t1 # LESS
	addiu $sp $sp 4
	beq $a0 1 while_line_5_exit # Return to the beginning of the loop
		 # Call function output
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	lw $a0 8($fp) # Load the value of i from the stack
	sll $a0 $a0 2 # Multiply the index by 4 to align with the word
	addu $t2 $fp 4 # Add the offset of the array
	lw $t2 0($t2) # Load the address of the array into $t2
	addu $t2 $t2 $a0 # Add the offset of the array
	lw $a0 0($t2) # Load the address of the array into $a0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	jal output_entry
	lw $a0 8($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 1 # Load constant into $a0
	lw $t1 4($sp)
	add $a0 $t1 $a0 # ADD
	addiu $sp $sp 4
	sw $a0 8($fp) # Save $a0 as the new value of i in the stack
	b while_line_5_entry # Return to the beginning of the loop
while_line_5_exit:
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 16 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
main_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
	li $a0 0 # Load constant into $a0
	sw $a0 84($fp) # Save $a0 as the new value of i in the stack
while_line_16_entry:
	lw $a0 84($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 20 # Load constant into $a0
	lw $t1 4($sp)
	slt $a0 $a0 $t1 # LESS
	addiu $sp $sp 4
	beq $a0 1 while_line_16_exit # Return to the beginning of the loop
	lw $a0 84($fp) # Load the value of i from the stack
	move $t3 $a0 # Save the value to be saved into $t3
	lw $a0 84($fp) # Load the value of i from the stack
	sll $a0 $a0 2 # Multiply the index by 4 to align with the word
	addu $t2 $a0 4 # Add the offset of the array
	addu $t2 $fp $t2 # Add the base address of the array
	sw $t3 0($t2) # Save the value of t3 into the stack
	lw $a0 84($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 1 # Load constant into $a0
	lw $t1 4($sp)
	add $a0 $t1 $a0 # ADD
	addiu $sp $sp 4
	sw $a0 84($fp) # Save $a0 as the new value of i in the stack
	b while_line_16_entry # Return to the beginning of the loop
while_line_16_exit:
		 # Call function recieve
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	addiu $sp $sp -4 # Allocate space in the stack for local variable
	addu $t2 $fp 4 # Add the offset of the array
	move $a0 $t2 # Load the address of the array into $a0
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack for parameter
	jal recieve_entry
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 16 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
