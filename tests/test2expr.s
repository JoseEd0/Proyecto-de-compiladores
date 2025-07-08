.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
# Declarando variable: a en offset -8
# Declarando variable: b en offset -16
# Declarando variable: c en offset -24
# Declarando variable: y en offset -32
 movq $10, %rax
    movq %rax, -8(%rbp) # a
 movq $2, %rax
    movq %rax, -16(%rbp) # b
 movq $4, %rax
    movq %rax, -24(%rbp) # c
# Cargando valor de a 0 -1
 movq -8(%rbp), %rax # a
    movq %rax, %rdx
# Cargando valor de b 0 -1
 movq -16(%rbp), %rax # b
    addq %rdx, %rax
    movq %rax, %rdx
# Cargando valor de c 0 -1
 movq -24(%rbp), %rax # c
    movq %rax, %rcx
    movq %rdx, %rax
    cqo
    idivq %rcx
    movq %rax, -32(%rbp) # y
.section .rodata
printf_fmt_0: .string "Resultado: %d\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de y 0 -1
 movq -32(%rbp), %rax # y
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
