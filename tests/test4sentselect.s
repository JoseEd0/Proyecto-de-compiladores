.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
# Declarando variable: val en offset -8
# Declarando variable: size en offset -16
# Declarando variable de for: i en offset -24
 movq $1, %rax
    movq %rax, -8(%rbp) # val
 movq $10, %rax
    movq %rax, -16(%rbp) # size
 movq $0, %rax
    movq %rax, -24(%rbp) # i
.Lfor1:
# Cargando valor de i 0 -1
 movq -24(%rbp), %rax # i
    movq %rax, %rdx
# Cargando valor de size 0 -1
 movq -16(%rbp), %rax # size
    cmpq %rax, %rdx
    movq $0, %rax
    setl %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendfor2
# Declarando variable de for: j en offset -32
 movq $0, %rax
    movq %rax, -32(%rbp) # j
.Lfor3:
# Cargando valor de j 0 -1
 movq -32(%rbp), %rax # j
    movq %rax, %rdx
# Cargando valor de size 0 -1
 movq -16(%rbp), %rax # size
    cmpq %rax, %rdx
    movq $0, %rax
    setl %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendfor4
# Cargando valor de j 0 -1
 movq -32(%rbp), %rax # j
    movq %rax, %rdx
# Cargando valor de i 0 -1
 movq -24(%rbp), %rax # i
    cmpq %rax, %rdx
    movq $0, %rax
    setle %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse5
.section .rodata
printf_fmt_0: .string "%d "
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de val 0 -1
 movq -8(%rbp), %rax # val
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
# Cargando valor de val 0 -1
 movq -8(%rbp), %rax # val
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -8(%rbp)
    jmp .Lendif6
.Lelse5:
.section .rodata
printf_fmt_1: .string ". "
.text
    leaq printf_fmt_1(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lendif6:
# Cargando valor de j 0 -1
 movq -32(%rbp), %rax # j
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -32(%rbp)
    jmp .Lfor3
.Lendfor4:
.section .rodata
printf_fmt_2: .string "\n"
.text
    leaq printf_fmt_2(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
# Cargando valor de i 0 -1
 movq -24(%rbp), %rax # i
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -24(%rbp)
    jmp .Lfor1
.Lendfor2:
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
