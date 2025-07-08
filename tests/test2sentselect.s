.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
# Declarando variable: x en offset -8
# Declarando variable: y en offset -16
 movq $1, %rax
    movq %rax, -8(%rbp) # x
 movq $0, %rax
    movq %rax, -16(%rbp) # y
# Cargando valor de x 0 -1
 movq -8(%rbp), %rax # x
    movq %rax, %rdx
 movq $1, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lfalse3
# Cargando valor de y 0 -1
 movq -16(%rbp), %rax # y
    movq %rax, %rdx
 movq $0, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    movq $1, %rax
    jnz .Lend4
.Lfalse3:
    movq $0, %rax
.Lend4:
    testq %rax, %rax
    jz .Lelse1
.section .rodata
printf_fmt_0: .string "Ambas condiciones son verdaderas\n"
.text
    leaq printf_fmt_0(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lelse1:
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
