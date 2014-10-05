;;kernel.asm

;nasm directive - 32 bit
bits 32
section .text
        ;multiboot spec
        align 4
        dd 0x1BADB002            ;magic
        dd 0x00                  ;flags
        dd - (0x1BADB002 + 0x00) ;checksum. m+f+c should be zero

global start
global gdt_flush     ; Allows the C code to link to this
global idt_load
global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9    ; Fill in the rest here!
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31
global _irq0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15
global stop
global go
extern kmain	        ;kmain is defined in the c file
extern gp            ; Says that '_gp' is in another file
extern idtp
extern gdt_install
extern idt_install
extern irq_install
extern kbd_int
extern fault_handler
extern irq_handler


;  0: Divide By Zero Exception
_isr0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
    
_isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

_isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

_isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

_isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

_isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

_isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

_isr8:
    cli
    push byte 8
    jmp isr_common_stub

_isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

_isr10:
    cli
    push byte 10
    jmp isr_common_stub

_isr11:
    cli
    push byte 11
    jmp isr_common_stub

_isr12:
    cli
    push byte 12
    jmp isr_common_stub

_isr13:
    cli
    push byte 13
    jmp isr_common_stub

_isr14:
    cli
    push byte 14
    jmp isr_common_stub

_isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

_isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

_isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

_isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

_isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

_isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

_isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

_isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

_isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

_isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

_isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

_isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

_isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

_isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

_isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

_isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

_isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

_irq0:
    cli
    push byte 0    
    push byte 32
    jmp irq_common_stub

_irq1:
    cli
    push byte 0
    push byte 33
    jmp irq_common_stub

_irq2:
    cli
    push byte 0
    push byte 34
    jmp irq_common_stub

_irq3:
    cli
    push byte 0
    push byte 35
    jmp irq_common_stub

_irq4:
    cli
    push byte 0
    push byte 36
    jmp irq_common_stub

_irq5:
    cli
    push byte 0
    push byte 37
    jmp irq_common_stub

_irq6:
    cli
    push byte 0
    push byte 38
    jmp irq_common_stub

_irq7:
    cli
    push byte 0
    push byte 39
    jmp irq_common_stub

_irq8:
    cli
    push byte 0
    push byte 40
    jmp irq_common_stub

_irq9:
    cli
    push byte 0
    push byte 41
    jmp irq_common_stub

_irq10:
    cli
    push byte 0
    push byte 42
    jmp irq_common_stub

_irq11:
    cli
    push byte 0
    push byte 43
    jmp irq_common_stub

_irq12:
    cli
    push byte 0
    push byte 44
    jmp irq_common_stub

_irq13:
    cli
    push byte 0
    push byte 45
    jmp irq_common_stub

_irq14:
    cli
    push byte 0
    push byte 46
    jmp irq_common_stub

_irq15:
    cli
    push byte 0
    push byte 47
    jmp irq_common_stub

isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

; This is a stub that we have created for IRQ based ISRs. This calls
; '_irq_handler' in our C code. We need to create this in an 'irq.c'
irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, irq_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

gdt_flush:
    lgdt [gp]        ; Load the GDT with our '_gp' which is a special pointer
    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!

flush2:
    ret               ; Returns back to the C code!

idt_load:
    lidt [idtp]
    ret

stop: cli
      ret
go: sti
    ret

start:
   sti
   call kmain
   ;hlt		 	;halt the CPU
 
