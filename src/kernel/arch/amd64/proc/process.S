#include "amd64_context.h"

.macro _m_save_context
    movq    %r15,   CONTEXT_R15(%rdi);
    movq    %r14,   CONTEXT_R14(%rdi);
    movq    %r13,   CONTEXT_R13(%rdi);
    movq    %r12,   CONTEXT_R12(%rdi);
    movq    %rbp,   CONTEXT_RBP(%rdi);
    movq    %rsp,   CONTEXT_RSP(%rdi);
    movq    %rbx,   CONTEXT_RBX(%rdi);
    movq    %rdi,   CONTEXT_RDI(%rdi);
    movq    %rsi,   CONTEXT_RSI(%rdi);

    movq    %cr0,   %rax;
    movq    %rax,   CONTEXT_CR0(%rdi);
    movq    %cr2,   %rax;
    movq    %rax,   CONTEXT_CR2(%rdi);
    movq    %cr3,   %rax;
    movq    %rax,   CONTEXT_CR3(%rdi);
    movq    %cr4,   %rax;
    movq    %rax,   CONTEXT_CR4(%rdi);
.endm

.macro _m_restore_context
    movq    CONTEXT_R15(%rsi), %r15;
    movq    CONTEXT_R14(%rsi), %r14;
    movq    CONTEXT_R13(%rsi), %r13;
    movq    CONTEXT_R12(%rsi), %r12;
    movq    CONTEXT_RBP(%rsi), %rbp;
    movq    CONTEXT_RSP(%rsi), %rsp;
    movq    CONTEXT_RBX(%rsi), %rbx;
    // movq    CONTEXT_RDI(%rsi), %rsi;
    // movq    CONTEXT_RSI(%rsi), %rdi;

    // movq    CONTEXT_CR0(%rsi), %rax;
    // movq    %rax,              %cr0;
    // movq    CONTEXT_CR2(%rsi), %rax;
    // movq    %rax,              %cr2;
    // movq    CONTEXT_CR4(%rsi), %rax;
    // movq    %rax,              %cr4;
    movq    CONTEXT_RIP(%rsi), %rax;
    movq    %rax,              %rcx;
.endm

.global save_context
.type save_context,@function
save_context:
    pushq   %rbp
    movq    %rsp, %rbp

    _m_save_context

    leaveq
    retq

.global switch_context
.type switch_context,@function
switch_context:
    _m_save_context
    _m_restore_context

    movq    CONTEXT_CR3(%rsi), %rax
    movq    %cr3,              %rbx
    cmpq    %rax,              %rbx
    je      1f
    movq    %rax,              %cr3
1:  jmpq    *%rcx
