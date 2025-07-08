.data
print_fmt: .string "%ld\n"
.text
.globl main
.globl arreglar
arreglar:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
# Declarando variable de for: i en offset -8
 movq $0, %rax
    movq %rax, -8(%rbp) # i
.Lfor1:
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rdx
 movq $10, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setl %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendfor2
# Declarando variable: l en offset -16
# Declarando variable de for: J en offset -24
 movq $8, %rax
    movq %rax, -16(%rbp) # l
 movq $0, %rax
    movq %rax, -24(%rbp) # J
.Lfor3:
# Cargando valor de J 0 -1
 movq -24(%rbp), %rax # J
    movq %rax, %rdx
 movq $10, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setl %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendfor4
.section .rodata
printf_fmt_0: .string "hola\n"
.text
    leaq printf_fmt_0(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $8, %rax
 movq %rax, %rbx # guardar operando derecho
 movq -16(%rbp), %rax # cargar local l
 addq %rbx, %rax
 movq %rax, -16(%rbp) # guardar en local l
# Cargando valor de J 0 -1
 movq -24(%rbp), %rax # J
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -24(%rbp)
    jmp .Lfor3
.Lendfor4:
.section .rodata
printf_fmt_1: .string "probando el l: %d \n"
.text
    leaq printf_fmt_1(%rip), %rdi
# Cargando valor de l 0 -1
 movq -16(%rbp), %rax # l
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -8(%rbp)
    jmp .Lfor1
.Lendfor2:
    leave
    ret
main:
 pushq %rbp
 movq %rsp, %rbp
    call arreglar
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
