.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
# Declarando variable de for: i en offset -8
.section .rodata
printf_fmt_0: .string "Iniciando bucles anidados:\n"
.text
    leaq printf_fmt_0(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $1, %rax
    movq %rax, -8(%rbp) # i
.Lfor1:
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rdx
 movq $3, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setle %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendfor2
# Declarando variable: j en offset -16
 movq $1, %rax
    movq %rax, -16(%rbp) # j
.Lwhile3:
# Cargando valor de j 0 -1
 movq -16(%rbp), %rax # j
    movq %rax, %rdx
 movq $2, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setle %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendwhile4
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rdx
# Cargando valor de j 0 -1
 movq -16(%rbp), %rax # j
    addq %rdx, %rax
    movq %rax, %rdx
 movq $2, %rax
    movq %rax, %rcx
    movq %rdx, %rax
    cqo
    idivq %rcx
    movq %rdx, %rax
    movq %rax, %rdx
 movq $0, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse5
.section .rodata
printf_fmt_1: .string "i=%d, j=%d -> suma par\n"
.text
    leaq printf_fmt_1(%rip), %rdi
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rsi
# Cargando valor de j 0 -1
 movq -16(%rbp), %rax # j
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
    jmp .Lendif6
.Lelse5:
.section .rodata
printf_fmt_2: .string "i=%d, j=%d -> suma impar\n"
.text
    leaq printf_fmt_2(%rip), %rdi
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rsi
# Cargando valor de j 0 -1
 movq -16(%rbp), %rax # j
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lendif6:
# Cargando valor de j 0 -1
 movq -16(%rbp), %rax # j
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -16(%rbp)
    jmp .Lwhile3
.Lendwhile4:
# Cargando valor de i 0 -1
 movq -8(%rbp), %rax # i
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -8(%rbp)
    jmp .Lfor1
.Lendfor2:
.section .rodata
printf_fmt_3: .string "Bucles anidados finalizados.\n"
.text
    leaq printf_fmt_3(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
