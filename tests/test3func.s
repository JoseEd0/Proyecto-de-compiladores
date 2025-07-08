.data
print_fmt: .string "%ld\n"
.text
.globl main
.globl factorial
factorial:
    pushq %rbp
    movq %rsp, %rbp
# Parámetro: n reg_index: 0 is_reference: false
# Cargando valor de n 0 0
 movq %rdi, %rax # Valor de n
    movq %rax, %rdx
 movq $1, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setle %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse1
 movq $1, %rax
    leave
    ret
.Lelse1:
# Cargando valor de n 0 0
 movq %rdi, %rax # Valor de n
    movq %rax, %rdx
# Cargando valor de n 0 0
 movq %rdi, %rax # Valor de n
    movq %rax, %rdx
 movq $1, %rax
    subq %rax, %rdx
    movq %rdx, %rax
    movq %rax, %rdi
    call factorial
    imulq %rdx, %rax
    leave
    ret
    leave
    ret
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
# Declarando variable: f en offset -8
 movq $5, %rax
    movq %rax, %rdi
    call factorial
    movq %rax, -8(%rbp) # f
.section .rodata
printf_fmt_0: .string "Factorial de 5 es: %d\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de f 0 -1
 movq -8(%rbp), %rax # f
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
