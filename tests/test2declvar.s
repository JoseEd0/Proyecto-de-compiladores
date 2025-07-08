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
    movq $0, -8(%rbp) # inicializar x a 0
    movq $0, -16(%rbp) # inicializar y a 0
 movq $1, %rax
 pushq %rax
 popq %rax
 movq %rax, -8(%rbp) # guardar en local x
 movq $10, %rax
 pushq %rax
 popq %rax
 movq %rax, -16(%rbp) # guardar en local y
 movq $20, %rax
 pushq %rax
 popq %rax
 movq %rax, -8(%rbp) # guardar en local x
.section .rodata
printf_fmt_0: .string " %d\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de x 0 -1
 movq -8(%rbp), %rax # x
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.section .rodata
printf_fmt_1: .string " %d\n"
.text
    leaq printf_fmt_1(%rip), %rdi
# Cargando valor de y 0 -1
 movq -16(%rbp), %rax # y
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
