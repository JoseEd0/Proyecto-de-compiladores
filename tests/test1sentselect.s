.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
# Declarando variable: a en offset -8
# Declarando variable: b en offset -16
 movq $1, %rax
    movq %rax, -8(%rbp) # a
 movq $0, %rax
    movq %rax, -16(%rbp) # b
# Cargando valor de a 0 -1
 movq -8(%rbp), %rax # a
    movq %rax, %rdx
 movq $1, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jnz .Ltrue3
# Cargando valor de b 0 -1
 movq -16(%rbp), %rax # b
    movq %rax, %rdx
 movq $1, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    movq $0, %rax
    jz .Lend4
.Ltrue3:
    movq $1, %rax
.Lend4:
    testq %rax, %rax
    jz .Lelse1
.section .rodata
printf_fmt_0: .string "Al menos una condicion es verdadera\n"
.text
    leaq printf_fmt_0(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lelse1:
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
