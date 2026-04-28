.code16
.global _start

_start:
    cli                         # 1. Apagar interrupciones
    lgdt gdt_descriptor         # 2. Cargar la GDT

    mov %cr0, %eax              # 3. Activar Modo Protegido (bit PE)
    or $0x1, %eax
    mov %eax, %cr0

    ljmp $0x08, $init_pm        # 4. Salto largo a modo 32 bits

.code32
init_pm:
    # 5. Cargar selectores de datos (0x10 apunta al descriptor de datos)
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # --- PRUEBA DE ESCRITURA ---
    # Si en la GDT el segmento es solo lectura, aquí la VM explotará/reiniciará
    movl $0xDEADBEEF, (0x0)     
    
    hlt

# --- GLOBAL DESCRIPTOR TABLE ---
.align 8
gdt_start:
    .quad 0x0                   # Descriptor Nulo

    # Descriptor de Código (Offset 0x08)
    # Base: 0x0, Límite: 0xFFFFF, Tipo: Ejecutable/Lectura
    .word 0xffff, 0x0000
    .byte 0x00, 0x9a, 0xcf, 0x00

    # Descriptor de Datos (Offset 0x10)
    # Base: 0x00020000 (Diferenciada!), Límite: 0xFFFFF
    # Acceso: 0x92 (Lectura/Escritura) -> CAMBIAR A 0x90 para "Solo Lectura"
    .word 0xffff, 0x0000        # Límite y Base (bits 0-15)
    .byte 0x02, 0x90, 0xcf, 0x00 # Base 16-23 es 0x02 (Base total 0x20000)

gdt_end:

gdt_descriptor:
    .word gdt_end - gdt_start - 1
    .long gdt_start
