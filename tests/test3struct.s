.data
print_fmt: .string "%ld\n"
# Miembro: ancho tipo: int offset: 0 tamaño: 8
# Miembro: alto tipo: int offset: 8 tamaño: 8
# Struct Rectangulo tamaño total: 16
# Campo alto: offset 8
# Campo ancho: offset 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
# Declarando variable: r en offset -16
# Declarando variable: ptr en offset -24
    movq $0, -16(%rbp) # inicializar r a 0
 movq $10, %rax
 pushq %rax
 popq %rax # recuperar valor a asignar
 pushq %rax # guardar valor temporalmente
 leaq -16(%rbp), %rax # dirección de ancho en struct local
 movq %rax, %rbx # guardar dirección
 popq %rax # recuperar valor
 movq %rax, (%rbx) # escribir valor en member
 movq $20, %rax
 pushq %rax
 popq %rax # recuperar valor a asignar
 pushq %rax # guardar valor temporalmente
 leaq -8(%rbp), %rax # dirección de alto en struct local
 movq %rax, %rbx # guardar dirección
 popq %rax # recuperar valor
 movq %rax, (%rbx) # escribir valor en member
# Cargando valor de r 0 -1
 movq -16(%rbp), %rax # r
    leaq -16(%rbp), %rax  # &r
    movq %rax, -24(%rbp) # ptr
 movq $30, %rax
 pushq %rax
 popq %rax # recuperar valor a asignar
 pushq %rax # guardar valor temporalmente
# Cargando valor de ptr 0 -1
 movq -24(%rbp), %rax # ptr
 addq $0, %rax # dirección de ancho
 movq %rax, %rbx # guardar dirección
 popq %rax # recuperar valor
 movq %rax, (%rbx) # escribir valor en member
 movq $40, %rax
 pushq %rax
 popq %rax # recuperar valor a asignar
 pushq %rax # guardar valor temporalmente
# Cargando valor de ptr 0 -1
 movq -24(%rbp), %rax # ptr
 addq $8, %rax # dirección de alto
 movq %rax, %rbx # guardar dirección
 popq %rax # recuperar valor
 movq %rax, (%rbx) # escribir valor en member
.section .rodata
printf_fmt_0: .string "Rectangulo en (%d, %d)\n"
.text
    leaq printf_fmt_0(%rip), %rdi
# Cargando valor de ptr 0 -1
 movq -24(%rbp), %rax # ptr
 addq $0, %rax # offset del miembro ancho
 movq (%rax), %rax # cargar valor del miembro
    movq %rax, %rsi
# Cargando valor de ptr 0 -1
 movq -24(%rbp), %rax # ptr
 addq $8, %rax # offset del miembro alto
 movq (%rax), %rax # cargar valor del miembro
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
.section .rodata
printf_fmt_1: .string "Rectangulo en (%d, %d)\n"
.text
    leaq printf_fmt_1(%rip), %rdi
 movq -16(%rbp), %rax # ancho de struct local
    movq %rax, %rsi
 movq -8(%rbp), %rax # alto de struct local
    movq %rax, %rdx
    xorq %rax, %rax # 0 registros vectoriales usados
    call printf@PLT
 movq $0, %rax
    leave
    ret
.section .note.GNU-stack,"",@progbits
