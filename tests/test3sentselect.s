.data
print_fmt: .string "%ld\n"
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
# Declarando variable: c en offset -8
 movq $98, %rax
    movq %rax, -8(%rbp) # c
# Cargando valor de c 0 -1
 movq -8(%rbp), %rax # c
    movq %rax, %rdx
 movq $97, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse1
.section .rodata
printf_fmt_0: .string "Es 'a'\n"
.text
    leaq printf_fmt_0(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
    jmp .Lendif2
.Lelse1:
# Cargando valor de c 0 -1
 movq -8(%rbp), %rax # c
    movq %rax, %rdx
 movq $98, %rax
    cmpq %rax, %rdx
    movq $0, %rax
    sete %al
    movzbq %al, %rax
    testq %rax, %rax
    jz .Lelse3
.section .rodata
printf_fmt_1: .string "Es 'b'\n"
.text
    leaq printf_fmt_1(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
    jmp .Lendif4
.Lelse3:
.section .rodata
printf_fmt_2: .string "Es otro caracter\n"
.text
    leaq printf_fmt_2(%rip), %rdi
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.Lendif4:
.Lendif2:
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
