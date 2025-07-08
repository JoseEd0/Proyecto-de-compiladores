.data
print_fmt: .string "%ld\n"
.text
.globl main
.globl suma
suma:
    pushq %rbp
    movq %rsp, %rbp
# Parámetro: c reg_index: 0 is_reference: true
# Parámetro: b reg_index: 1 is_reference: false
# Cargando valor de c 1 0
 movq (%rdi), %rax # Valor de* c
    movq %rax, %rcx
    incq %rcx
    movq %rcx, (%rdi)  # *ref++
# Cargando valor de c 1 0
 movq (%rdi), %rax # Valor de* c
    leave
    ret
    leave
    ret
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
# Declarando variable: a en offset -8
# Declarando variable: b en offset -16
# Declarando variable: resultado en offset -24
 movq $10, %rax
    movq %rax, -8(%rbp) # a
.section .rodata
printf_fmt_0: .string "Entero: %d, Caracter: %c\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de a 0 -1
 movq -8(%rbp), %rax # a
    movq %rax, %rsi
 movq $97, %rax
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $20, %rax
    movq %rax, -16(%rbp) # b
    leaq -8(%rbp), %rax # &a
    movq %rax, %rdi
# Cargando valor de b 0 -1
 movq -16(%rbp), %rax # b
    movq %rax, %rsi
    call suma
    movq %rax, -24(%rbp) # resultado
.section .rodata
printf_fmt_1: .string "La suma es: %d\n"
.text
    leaq printf_fmt_1(%rip), %rdi
# Cargando valor de a 0 -1
 movq -8(%rbp), %rax # a
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
