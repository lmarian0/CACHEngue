.intel_syntax noprefix
.text
.globl float_to_int
.type float_to_int, @function

# long float_to_int(double value)
# Input : XMM0 = value
# Output: RAX  = truncated integer
# ABI   : System V AMD64
float_to_int:
    push rbp
    mov rbp, rsp

    cvttsd2si rax, xmm0

    pop rbp
    ret

.section .note.GNU-stack,"",@progbits
