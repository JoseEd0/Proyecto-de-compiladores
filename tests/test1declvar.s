.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
# Declarando variable: a en offset -8
 movq $10, %rax
    movq %rax, -8(%rbp) # a
.section .rodata
printf_fmt_0: .string "Entero: %d, Caracter: %c\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de a
 movq -8(%rbp), %rax # a
    movq %rax, %rsi
 movq $97, %rax
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits