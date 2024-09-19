	.file	"decomment.c"
	.text
	.section	.rodata
	.align 8
.LC0:
	.string	"Error: line %d: unterminated comment\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$1, -24(%rbp)
	movl	$0, -28(%rbp)
	movq	stdout(%rip), %rax
	movq	%rax, -16(%rbp)
	jmp	.L2
.L13:
	movl	-20(%rbp), %eax
	movb	%al, -29(%rbp)
	cmpb	$10, -29(%rbp)
	jne	.L3
	addl	$1, -24(%rbp)
.L3:
	movl	-28(%rbp), %eax
	cmpl	$6, %eax
	ja	.L2
	movl	%eax, %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L6(%rip), %rax
	movl	(%rdx,%rax), %eax
	cltq
	leaq	.L6(%rip), %rdx
	addq	%rdx, %rax
	notrack jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L6:
	.long	.L12-.L6
	.long	.L11-.L6
	.long	.L10-.L6
	.long	.L9-.L6
	.long	.L8-.L6
	.long	.L7-.L6
	.long	.L5-.L6
	.text
.L12:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_default
	jmp	.L2
.L11:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_comment_start
	jmp	.L2
.L10:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_slc
	jmp	.L2
.L9:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_mlc
	jmp	.L2
.L8:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_mlc_end
	jmp	.L2
.L7:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_in_string
	jmp	.L2
.L5:
	movsbl	-29(%rbp), %eax
	movq	-16(%rbp), %rdx
	leaq	-28(%rbp), %rcx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	handle_in_char
	nop
.L2:
	call	getchar@PLT
	movl	%eax, -20(%rbp)
	cmpl	$-1, -20(%rbp)
	jne	.L13
	movl	-28(%rbp), %eax
	cmpl	$2, %eax
	je	.L14
	movl	-28(%rbp), %eax
	cmpl	$3, %eax
	je	.L14
	movl	-28(%rbp), %eax
	cmpl	$4, %eax
	jne	.L15
.L14:
	movq	stderr(%rip), %rax
	movl	-24(%rbp), %edx
	leaq	.LC0(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	$1, %eax
	jmp	.L17
.L15:
	movl	$0, %eax
.L17:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L18
	call	__stack_chk_fail@PLT
.L18:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	handle_default
	.type	handle_default, @function
handle_default:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L20
	movq	-16(%rbp), %rax
	movl	$1, (%rax)
	jmp	.L24
.L20:
	cmpb	$34, -4(%rbp)
	jne	.L22
	movq	-16(%rbp), %rax
	movl	$5, (%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$34, %edi
	call	fputc@PLT
	jmp	.L24
.L22:
	cmpb	$39, -4(%rbp)
	jne	.L23
	movq	-16(%rbp), %rax
	movl	$6, (%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$39, %edi
	call	fputc@PLT
	jmp	.L24
.L23:
	movsbl	-4(%rbp), %eax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	fputc@PLT
.L24:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	handle_default, .-handle_default
	.section	.rodata
.LC1:
	.string	"/%c"
	.text
	.globl	handle_comment_start
	.type	handle_comment_start, @function
handle_comment_start:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L26
	movq	-16(%rbp), %rax
	movl	$2, (%rax)
	jmp	.L29
.L26:
	cmpb	$42, -4(%rbp)
	jne	.L28
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	fputc@PLT
	movq	-16(%rbp), %rax
	movl	$3, (%rax)
	jmp	.L29
.L28:
	movsbl	-4(%rbp), %edx
	movq	-24(%rbp), %rax
	leaq	.LC1(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
.L29:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	handle_comment_start, .-handle_comment_start
	.section	.rodata
.LC2:
	.string	" \n"
	.text
	.globl	handle_slc
	.type	handle_slc, @function
handle_slc:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	cmpb	$10, -4(%rbp)
	jne	.L32
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	movl	$2, %edx
	movl	$1, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	fwrite@PLT
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
.L32:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	handle_slc, .-handle_slc
	.globl	handle_mlc
	.type	handle_mlc, @function
handle_mlc:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	cmpb	$10, -4(%rbp)
	jne	.L34
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$10, %edi
	call	fputc@PLT
	jmp	.L36
.L34:
	cmpb	$42, -4(%rbp)
	jne	.L36
	movq	-16(%rbp), %rax
	movl	$4, (%rax)
.L36:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	handle_mlc, .-handle_mlc
	.globl	handle_mlc_end
	.type	handle_mlc_end, @function
handle_mlc_end:
.LFB5:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L38
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	jmp	.L42
.L38:
	cmpb	$10, -4(%rbp)
	jne	.L40
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$10, %edi
	call	fputc@PLT
	movq	-16(%rbp), %rax
	movl	$3, (%rax)
	jmp	.L42
.L40:
	cmpb	$42, -4(%rbp)
	jne	.L41
	movq	-16(%rbp), %rax
	movl	$4, (%rax)
	jmp	.L42
.L41:
	movq	-16(%rbp), %rax
	movl	$3, (%rax)
.L42:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	handle_mlc_end, .-handle_mlc_end
	.globl	handle_in_string
	.type	handle_in_string, @function
handle_in_string:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	cmpb	$34, -4(%rbp)
	jne	.L44
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$34, %edi
	call	fputc@PLT
	jmp	.L47
.L44:
	cmpb	$10, -4(%rbp)
	jne	.L46
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$10, %edi
	call	fputc@PLT
	jmp	.L47
.L46:
	movsbl	-4(%rbp), %eax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	fputc@PLT
.L47:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	handle_in_string, .-handle_in_string
	.globl	handle_in_char
	.type	handle_in_char, @function
handle_in_char:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -4(%rbp)
	movsbl	-4(%rbp), %eax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	fputc@PLT
	cmpb	$39, -4(%rbp)
	jne	.L49
	movq	-16(%rbp), %rax
	movl	$0, (%rax)
	jmp	.L51
.L49:
	cmpb	$10, -4(%rbp)
	jne	.L51
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	fputc@PLT
.L51:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	handle_in_char, .-handle_in_char
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
