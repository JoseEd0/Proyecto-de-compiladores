.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $64, %rsp
# Declarando variable: n en offset -8
# Declarando variable: a en offset -16
# Declarando variable: b en offset -24
# Declarando variable: i en offset -32
# Declarando variable: pa en offset -40
# Declarando variable: pb en offset -48
# Declarando variable: fib en offset -56
 movq $10, %rax
    movq %rax, -8(%rbp) # n
 movq $0, %rax
    movq %rax, -16(%rbp) # a
 movq $1, %rax
    movq %rax, -24(%rbp) # b
 movq $2, %rax
    movq %rax, -32(%rbp) # i
# Cargando valor de a 0 -1
 movq -16(%rbp), %rax # a
    leaq -16(%rbp), %rax  # &a
    movq %rax, -40(%rbp) # pa
# Cargando valor de b 0 -1
 movq -24(%rbp), %rax # b
    leaq -24(%rbp), %rax  # &b
    movq %rax, -48(%rbp) # pb
 movq $0, %rax
    movq %rax, -56(%rbp) # fib
# Cargando valor de n 0 -1
 movq -8(%rbp), %rax # n
    movq %rax, %rdx
 movq $0, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse1
# Cargando valor de pa 0 -1
 movq -40(%rbp), %rax # pa
    movq (%rax), %rax  # *ptr
 pushq %rax
 popq %rax
 movq %rax, -56(%rbp) # guardar en local fib
    jmp .Lendif2
.Lelse1:
# Cargando valor de n 0 -1
 movq -8(%rbp), %rax # n
    movq %rax, %rdx
 movq $1, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse3
# Cargando valor de pb 0 -1
 movq -48(%rbp), %rax # pb
    movq (%rax), %rax  # *ptr
 pushq %rax
 popq %rax
 movq %rax, -56(%rbp) # guardar en local fib
    jmp .Lendif4
.Lelse3:
.Lwhile5:
# Cargando valor de i 0 -1
 movq -32(%rbp), %rax # i
    movq %rax, %rdx
# Cargando valor de n 0 -1
 movq -8(%rbp), %rax # n
    cmpq %rax, %rdx
    movq $0, %rax
    setle %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lendwhile6
# Cargando valor de pa 0 -1
 movq -40(%rbp), %rax # pa
    movq (%rax), %rax  # *ptr
    movq %rax, %rdx
# Cargando valor de pb 0 -1
 movq -48(%rbp), %rax # pb
    movq (%rax), %rax  # *ptr
    addq %rdx, %rax
 pushq %rax
 popq %rax
 movq %rax, -56(%rbp) # guardar en local fib
# Cargando valor de pb 0 -1
 movq -48(%rbp), %rax # pb
    movq (%rax), %rax  # *ptr
 pushq %rax
# Cargando valor de pa 0 -1
 movq -40(%rbp), %rax # pa
 movq %rax, %rbx # guardar dirección
 popq %rax # recuperar valor a asignar
 movq %rax, (%rbx) # *ptr = valor
# Cargando valor de fib 0 -1
 movq -56(%rbp), %rax # fib
 pushq %rax
# Cargando valor de pb 0 -1
 movq -48(%rbp), %rax # pb
 movq %rax, %rbx # guardar dirección
 popq %rax # recuperar valor a asignar
 movq %rax, (%rbx) # *ptr = valor
# Cargando valor de i 0 -1
 movq -32(%rbp), %rax # i
    movq %rax, %rcx
    incq %rcx
    movq %rcx, -32(%rbp)
    jmp .Lwhile5
.Lendwhile6:
.Lendif4:
.Lendif2:
.section .rodata
printf_fmt_0: .string "Fibonacci(%d) = %d\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de n 0 -1
 movq -8(%rbp), %rax # n
    movq %rax, %rsi
# Cargando valor de fib 0 -1
 movq -56(%rbp), %rax # fib
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
