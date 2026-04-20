.intel_syntax noprefix
.text
.globl gini_plus_one
.type gini_plus_one, @function

# long gini_plus_one(double value)
# Input : XMM0 = value
# Output: RAX  = trunc(value) + 1
# ABI   : System V AMD64
gini_plus_one:
    push rbp
    mov rbp, rsp

    cvttsd2si rax, xmm0
    inc rax

    pop rbp
    ret

.section .note.GNU-stack,"",@progbits
