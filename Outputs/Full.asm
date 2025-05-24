	.globl main
	.data
		x: .space 80
	.text
main:
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for the frame pointer
	addiu $sp $sp -4 # Allocate space in the stack for the parameter
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
minimo_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
	li $a0 0 # Load constant into $a0
	sw $a0 20($fp) # Save $a0 as the new value of k in the stack
	li $a0 0 # Load constant into $a0
	sll $a0 $a0 2 # Multiply the index by 4 to align with the word
	addu $t2 $fp 4 # Add the offset of the array
	lw $t2 0($t2) # Load the address of the array into $t2
	addu $t2 $t2 $a0 # Add the offset of the array
	lw $a0 0($t2) # Load the address of the array into $a0
	sw $a0 16($fp) # Save $a0 as the new value of x in the stack
	li $a0 1 # Load constant into $a0
	sw $a0 12($fp) # Save $a0 as the new value of i in the stack
while_line_8_entry:
	lw $a0 12($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	lw $a0 8($fp) # Load the value of high from the stack
	lw $t1 4($sp)
	slt $a0 $a0 $t1 # LESS
	addiu $sp $sp 4
	beq $a0 1 while_line_8_exit # Return to the beginning of the loop
# Start of if statement if_line_9
	lw $a0 12($fp) # Load the value of i from the stack
	sll $a0 $a0 2 # Multiply the index by 4 to align with the word
	addu $t2 $fp 4 # Add the offset of the array
	lw $t2 0($t2) # Load the address of the array into $t2
	addu $t2 $t2 $a0 # Add the offset of the array
	lw $a0 0($t2) # Load the address of the array into $a0
	sw $a0 0($sp)
	addiu $sp $sp -4
	lw $a0 16($fp) # Load the value of x from the stack
	lw $t1 4($sp)
	slt $a0 $a0 $t1 # LESS
	addiu $sp $sp 4
	beq $a0 1 true_if_line_9 # Branch if condition is true
false_if_line_9:
	b end_if_line_9 # Branch to the end of the if statement
true_if_line_9:
	lw $a0 12($fp) # Load the value of i from the stack
	sll $a0 $a0 2 # Multiply the index by 4 to align with the word
	addu $t2 $fp 4 # Add the offset of the array
	lw $t2 0($t2) # Load the address of the array into $t2
	addu $t2 $t2 $a0 # Add the offset of the array
	lw $a0 0($t2) # Load the address of the array into $a0
	sw $a0 16($fp) # Save $a0 as the new value of x in the stack
	lw $a0 12($fp) # Load the value of i from the stack
	sw $a0 20($fp) # Save $a0 as the new value of k in the stack
end_if_line_9:
	lw $a0 12($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 1 # Load constant into $a0
	lw $t1 4($sp)
	add $a0 $t1 $a0 # ADD
	addiu $sp $sp 4
	sw $a0 12($fp) # Save $a0 as the new value of i in the stack
	b while_line_8_entry # Return to the beginning of the loop
while_line_8_exit:
	lw $a0 20($fp) # Load the value of k from the stack
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 28 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
main_entry:
	move $fp $sp # Save frame pointer
	sw $ra 0($sp) # Save return address
	addiu $sp $sp -4 # Allocate space in the stack
	li $a0 0 # Load constant into $a0
	sw $a0 4($fp) # Save $a0 as the new value of i in the stack
		 # Call function input
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	jal input_entry
	sw $a0 8($fp) # Save $a0 as the new value of high in the stack
while_line_22_entry:
	lw $a0 4($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	lw $a0 8($fp) # Load the value of high from the stack
	lw $t1 4($sp)
	sle $a0 $a0 $t1 # LESS OR EQUAL
	addiu $sp $sp 4
	beq $a0 1 while_line_22_exit # Return to the beginning of the loop
		 # Call function input
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	jal input_entry
	move $t3 $a0 # Save the value to be saved into $t3
	lw $a0 4($fp) # Load the value of i from the stack
	sll $a0 $a0 2 # Multiply the index by 4 to align with the word
	addu $t2 $a0 4 # Add the offset of the array
	sw $t3 x($t2) # Save value of t3 into the global array
	lw $a0 4($fp) # Load the value of i from the stack
	sw $a0 0($sp)
	addiu $sp $sp -4
	li $a0 1 # Load constant into $a0
	lw $t1 4($sp)
	add $a0 $t1 $a0 # ADD
	addiu $sp $sp 4
	sw $a0 4($fp) # Save $a0 as the new value of i in the stack
	b while_line_22_entry # Return to the beginning of the loop
while_line_22_exit:
		 # Call function output
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
		 # Call function minimo
	sw $fp 0($sp) # Save frame pointer in the stack
	addiu $sp $sp -4 # Allocate space in the stack for fp
	addiu $sp $sp -4 #  Local Variable
	addiu $sp $sp -4 #  Local Variable
	addiu $sp $sp -4 #  Local Variable
	lw $a0 8($fp) # Load the value of high from the stack
	sw $a0 0($sp) # Save the value of high in the stack
	addiu $sp $sp -4
	la $t2 x # Load the address of the array into $t2
	addu $t2 $t2 4 # Add the offset for the beginning of the array
	move $a0 $t2 # Load the address of the array into $a0
	sw $a0 0($sp) # Save the value of a in the stack
	addiu $sp $sp -4
	jal minimo_entry
	sw $a0 0($sp) # Save the parameter in the stack
	addiu $sp $sp -4 # Allocate space in the stack
	jal output_entry
	lw $ra 4($sp) # Restore Return address
	addiu $sp $sp 16 # Restore stack pointer to the last function
	lw $fp 0($sp) # Restore frame pointer to the last function
	jr $ra
