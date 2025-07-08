.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
# Declarando variable: y en offset -8
# Declarando variable: z en offset -16
# Declarando variable: f en offset -24
# Declarando variable: g en offset -32
 movq $5, %rax
    movq %rax, -8(%rbp) # y
# Cargando valor de y 0 -1
 movq -8(%rbp), %rax # y
    incq %rax
    movq %rax, -8(%rbp)
    movq %rax, -16(%rbp) # z
 movq $1, %rax
    movq %rax, -24(%rbp) # f
    andq $1, -24(%rbp) # asegurar 0/1 para bool
 movq $0, %rax
    movq %rax, -32(%rbp) # g
    andq $1, -32(%rbp) # asegurar 0/1 para bool
# Cargando valor de f 0 -1
 movq -24(%rbp), %rax # f
    testq %rax, %rax
    jz .Lfalse3
# Cargando valor de g 0 -1
 movq -32(%rbp), %rax # g
    testq %rax, %rax
    movq $1, %rax
    jnz .Lend4
.Lfalse3:
    movq $0, %rax
.Lend4:
    testq %rax, %rax
    jz .Lelse1
 movq $1, %rax
    movq %rax, %rdx
 movq $8, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setg %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse5
.section .rodata
printf_fmt_0: .string "Resultado: %d\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de z 0 -1
 movq -16(%rbp), %rax # z
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
    jmp .Lendif6
.Lelse5:
.section .rodata
printf_fmt_1: .string "Resultado: %d\n"
.text
    leaq printf_fmt_1(%rip), %rdi
# Cargando valor de z 0 -1
 movq -16(%rbp), %rax # z
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lendif6:
    jmp .Lendif2
.Lelse1:
# Cargando valor de f 0 -1
 movq -24(%rbp), %rax # f
    testq %rax, %rax
    jnz .Ltrue9
# Cargando valor de g 0 -1
 movq -32(%rbp), %rax # g
    testq %rax, %rax
    movq $0, %rax
    jz .Lend10
.Ltrue9:
    movq $1, %rax
.Lend10:
    testq %rax, %rax
    jz .Lelse7
 movq $1, %rax
    movq %rax, %rdx
 movq $8, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    setg %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse11
.section .rodata
printf_fmt_2: .string "Resultado: %d\n"
.text
    leaq printf_fmt_2(%rip), %rdi
# Cargando valor de z 0 -1
 movq -16(%rbp), %rax # z
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
    jmp .Lendif12
.Lelse11:
.section .rodata
printf_fmt_3: .string "Resultado: %d\n"
.text
    leaq printf_fmt_3(%rip), %rdi
# Cargando valor de z 0 -1
 movq -16(%rbp), %rax # z
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lendif12:
    jmp .Lendif8
.Lelse7:
.section .rodata
printf_fmt_4: .string "Resultado: %d\n"
.text
    leaq printf_fmt_4(%rip), %rdi
# Cargando valor de z 0 -1
 movq -16(%rbp), %rax # z
    movq %rax, %rsi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lendif8:
.Lendif2:
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
