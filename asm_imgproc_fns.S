/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

/*
 * Definitions of image transformation functions
 */

/*
 * Given a 32-bit pixel, extract its 8-bit red component (bits 24-31).
 *
 * Parameter
 * %rdi - 32-bit pixel value
 * 
 * @return the 32-bit red value of pixel (24-31)
 */
	.globl get_r
get_r:
	/* Register use:
	 * %rdi is the 64-bit pixel value
	 * %edi is the lower 32 bits of %rdi
	 * %rax is the return value (the red component of pixel)
	 * %eax is the lower 32 bits of %rax
	 */
	/* prologue */
	pushq %rdi

	/* zero the non-red component of %edi */
	andl $0xFF000000, %edi

	/* extract the red component of %edi */
	shr $24, %edi

	/* epilogue */
	movl %edi, %eax
	popq %rdi
	ret

/*
 * Given a pixel, extract its 8-bit green component (bits 16-23).
 *
 * Parameter
 * %rdi - pixel value
 * 
 * @return the green value of pixel (16-23)
 */
	.globl get_g
get_g:
	/* Register use:
	 * %rdi is the 64-bit pixel value
	 * %edi is the lower 32 bits of %rdi
	 * %rax is the return value (the green component of pixel)
	 * %eax is the lower 32 bits of %rax
	 */
	/* prologue */
	pushq %rdi

	/* zero the non-green component of %edi */
	andl $0x00FF0000, %edi

	/* extract the green component of %edi */
	shr $16, %edi

	/* epilogue */
	movl %edi, %eax
	popq %rdi
	ret

/*
 * Given a pixel, extract its 8-bit blue component (bits 8-15).
 *
 * Parameter
 * %rdi - pixel value
 * 
 * @return the blue value of pixel (8-15)
 */
	.globl get_b
get_b:
	/* Register use:
	 * %rdi is the 64-bit pixel value
	 * %edi is the lower 32 bits of %rdi
	 * %rax is the return value (the blue component of pixel)
	 * %eax is the lower 32 bits of %rax
	 */
	/* prologue */
	pushq %rdi

	/* zero the non-blue component of %edi */
	andl $0x0000FF00, %edi

	/* extract the blue component of %edi */
	shr $8, %edi

	/* epilogue */
	movl %edi, %eax
	popq %rdi
	ret

/*
 * Given a pixel, extract its 8-bit alpha component (bits 0-7).
 *
 * Parameter
 * %rdi - pixel value
 *
 * @return the alpha value of pixel (0-7)
 */
	.globl get_a
get_a:
	/* Register use:
	 * %rdi is the 64-bit pixel value
	 * %edi is the lower 32 bits of %rdi
	 * %rax is the return value (the alpha component of pixel)
	 * %eax is the lower 32 bits of %rax
	 */
	/* prologue */
	pushq %rdi

	/* zero the non-alpha component of %edi */
	andl $0x000000FF, %edi

	/* epilogue */
	movl %edi, %eax
	popq %rdi
	ret

/*
 * Given red, green, blue, and alpha values, construct a 32-bit
 * pixel with r being in bits 24-31, b in bits 16-23, g in bits
 * 8-15, and a in bits 0-7.
 *
 * Parameter
 * %rdi - 32-bit number 0-255 representing the red component
 * %rsi - 32-bit number 0-255 representing the green component
 * %rdx - 32-bit number 0-255 representing the blue component
 * %rcx - 32-bit number 0-255 representing the alpha component
 * 
 * @return the complete pixel
 */
	.globl make_pixel
make_pixel:
	/* Register use:
	 * %rax is the return value (the complete pixel)
	 * %eax is the lower 32 bits of %rax
	 */

	/* prologue */
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	movq $0, %rax

	/* add the red component */
	shl $24, %edi
	addl %edi, %eax

	/* add the green component */
	shl $16, %esi
	addl %esi, %eax

	/* add the blue component */
	shl $8, %edx
	addl %edx, %eax

	/* add the alpha component */
	addl %ecx, %eax

	/* epilogue */
	popq %rdi
	popq %rsi
	popq %rdx
	popq %rcx
	ret

/*
 *  Transform the color component values in each input pixel
 *  by applying the bitwise complement operation. I.e., each bit
 *  in the color component information should be inverted
 *  (1 becomes 0, 0 becomes 1.) The alpha value of each pixel should
 *  be left unchanged.
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *         transformed pixels should be stored)
 */
	.globl imgproc_complement
imgproc_complement:
	/* TODO: implement */
	
	ret

/*
 *  Transform the input image by swapping the row and column
 *  of each source pixel when copying it to the output image.
 *  E.g., a pixel at row i and column j of the input image
 *  should be copied to row j and column i of the output image.
 *  Note that this transformation can only be applied to square
 *  images (where the width and height are identical.)
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *                    transformed pixels should be stored)
 * 
 *  @return 1 if the transformation succeeded, or 0 if the
 *          transformation can't be applied because the image
 *          width and height are not the same
 */
	.globl imgproc_transpose
imgproc_transpose:
	/* Register use: 
	* %rdi is the pointer to the input image
	* %rsi is the pointer to the output image
	* %ecx is height of input image
	* %edx is width of input image
	* %r10 is the current row
	* %r11 is the current column
	* %r12 is the pixel value to be copied from input -> output
	* %r8 is the 1D index for input[row][col]
	* %r9 is the 1D index for output[col][row]
	* %rax is the return value
	*/

	pushq %r12

	/* move height and width into %ecx and %edx */
	movl IMAGE_WIDTH_OFFSET(%rdi), %ecx
	movl IMAGE_HEIGHT_OFFSET(%rdi), %edx

	/* set default return value */
	mov $0, %rax

	/* Check that height == width and jump to epilogue if they aren't.*/
	cmp %edx, %ecx
	jne .LepilogueTranspose

	/* Now that we are sure that height == width, set return value to 1 */
	mov $1, %rax

	/* Pre-loop, set row and col to -1 */
	mov $-1, %r10
	mov $-1, %r11

	/* Outer loop: loop thru rows*/
	.LouterLoopTranspose:
		/* add 1 to row */
		add $1, %r10

		/* check if we're at the end (row == height) */
		cmp %r10d, %edx
		je .LepilogueTranspose

		/* go to inner loop */
		jmp .LinnerLoopTranspose

	/* Inner loop: loop thru cols and set output[row][col] to input[col][row] */
	.LinnerLoopTranspose:
		/* Add 1 to col */
		add $1, %r11

		/* Check if this row is finished (col == width) and go back to .Louterloop if so. */
		cmp %r11d, %ecx
		je .LouterLoopTranspose

		/* Save values in stack before calling compute_index */
		pushq %rdi
		pushq %rsi
		pushq %rdx
		pushq %rax
		
		/* call compute_index(input_img, row, col) for input[row][col] */
		movq %r10, %rsi
		movq %r11, %rdx
		call compute_index
		movq %rax, %r8
		
		/* call compute_index(input_img, row, col) for input[row][col] */
		movq %rsi, %rdi
		movq %r11, %rsi
		movq %r10, %rdx
		call compute_index
		movq %rax, %r9

		/* Restore values after calling compute_index */
		popq %rax
		popq %rdx
		popq %rsi
		popq %rdi

		/* Get the pixel value in input[row][col] */
		mov IMAGE_DATA_OFFSET(%rdi, %r8, 4), %r12
	
		/* Set output[col][row] to the pixel value */
		mov %r12, IMAGE_DATA_OFFSET(%rdx, %r9, 4)

		jmp .LinnerLoopTranspose

	/* Epilogue */
	.LepilogueTranspose:
		popq %r12
	ret
	
/* Return whether or not the pixel at a given row or column is within the ellipse
*
* Parameters:
* %rdi - pointer to the input Image
* %rsi - row at which the pixel is located	
* %rdx - col the column at which the pixel is located
*	
* @return true or false (1 or 0) based on if the given pixel is within the defined ellipse of the image
*/
/*	.globl is_in_ellipse					
is_in_ellipse:

	movq %r10, %rdi  //takes in width as temp variable (a)
	add IMAGE_HEIGHT_OFFSET, %rdi //offsets rdi to access height
	movq %r11, %rdi //takes in height as temp variable (b)
	shr %r10 // dives value in r10 by 2, in accordance with ellipse formula
	shr %r11 // same as above

	
	sub %rdx, %r10 //subtracts the column from a, creates (x)
	sub %rsi, %r11 //same as above, creates (y)

	movq %r10, %r12 //copies x to r12 to begin mathematical calculations
	movq %r11, %r13 //copies y to r13 to begin mathematical calculations

	Imul %r12, %r12 //computes x^2
	Imul %r13, %r13 //computes y^2
	Imul $10000, %r12 //multiplies by 10000
	Imul $10000, %r13 //multiplies by 10000

	Imul %r10, %r10 //computes a^2
	Imul %r11, %r11 //computes b^2

	movq %r12, %RAX //puts dividend in AX to prepare for division
	div %r10 //divides 10,000x^2 by a^2
	movq %RAX, %r12 //puts divided value back into r12

	movq %r13, %RAX //puts dividend in AX to prepare for division
	div %r11 //divides 10,000y^2 by b^2
	movq %RAX, %r13 //puts divided value back into r13

	add %r12, %r13 //adds together the two parts of the equation
	cmp $10000, %r13 //compares equation with 10000
	jle .Ltrue //control flow based on above comparison
	jmp .Lfalse

	.Lfalse: //puts false value in return register
	movq $0, %rax
	jmp .Lepilogue

	.Ltrue: //puts true value in return register
	movq $1, %rax
	jmp .Lepilogue

	.Lepilogue:
	ret

	*/
	
/* Compute the 1D index of a pixel given an image, row, and column.
*
* Paremeters:
* @rdi - pointer to the input Image
* %rsi - the row location of the pixel
* %rdx - the column location of the pixel
*	
* @return 1D location of the pixel
*/
	.globl	compute_index
compute_index:
	movq %rsi, %r10 //puts row into r10
	movq IMAGE_WIDTH_OFFSET(%rdi), %r11 //puts image width into r11
	Imul %r11, %r10 //multiplies row by width, in accordance with formula
	add %rdx, %r10 //adds column value to previous produce
	movq %r10, rax //puts final product into return address
	ret

/*
 *  Transform the input image by copying only those pixels that are
 *  within an ellipse centered within the bounds of the image.
 *  Pixels not in the ellipse should be left unmodified, which will
 *  make them opaque black.
 * 
 *  Let w represent the width of the image and h represent the
 *  height of the image. Let a=floor(w/2) and b=floor(h/2).
 *  Consider the pixel at row b and column a is being at the
 *  center of the image. When considering whether a specific pixel
 *  is in the ellipse, x is the horizontal distance to the center
 *  of the image and y is the vertical distance to the center of
 *  the image. The pixel at the coordinates described by x and y
 *  is in the ellipse if the following inequality is true:
 * 
 *    floor( (10000*x*x) / (a*a) ) + floor( (10000*y*y) / (b*b) ) <= 10000
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *                    transformed pixels should be stored)
 */
	.globl imgproc_ellipse
imgproc_ellipse:
	/* TODO: implement */
	ret

/*
 *  Transform the input image using an "emboss" effect. The pixels
 *  of the source image are transformed as follows.
 * 
 *  The top row and left column of pixels are transformed so that their
 *  red, green, and blue color component values are all set to 128,
 *  and their alpha values are not modified.
 * 
 *  For all other pixels, we consider the pixel's color component
 *  values r, g, and b, and also the pixel's upper-left neighbor's
 *  color component values nr, ng, and nb. In comparing the color
 *  component values of the pixel and its upper-left neighbor,
 *  we consider the differences (nr-r), (ng-g), and (nb-b).
 *  Whichever of these differences has the largest absolute value
 *  we refer to as diff. (Note that in the case that more than one
 *  difference has the same absolute value, the red difference has
 *  priority over green and blue, and the green difference has priority
 *  over blue.)
 * 
 *  From the value diff, compute the value gray as 128 + diff.
 *  However, gray should be clamped so that it is in the range
 *  0..255. I.e., if it's negative, it should become 0, and if
 *  it is greater than 255, it should become 255.
 * 
 *  For all pixels not in the top or left row, the pixel's red, green,
 *  and blue color component values should be set to gray, and the
 *  alpha value should be left unmodified.
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *                    transformed pixels should be stored)
 */
	.globl imgproc_emboss
imgproc_emboss:
	/* TODO: implement */
	ret

/*
.vim:ft=gas:
*/
