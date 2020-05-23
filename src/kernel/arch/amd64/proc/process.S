#include "amd64_context.h"

.macro save_context
    movq    (%rsp), %rax;
    movq    %rax,   CONTEXT_RIP(%rdi);

    movq    %r15,   CONTEXT_R15(%rdi);
    movq    %r14,   CONTEXT_R14(%rdi);
    movq    %r13,   CONTEXT_R13(%rdi);
    movq    %r12,   CONTEXT_R12(%rdi);
    movq    %rbp,   CONTEXT_RBP(%rdi);
    movq    %rsp,   CONTEXT_RSP(%rdi);
    movq    %rbx,   CONTEXT_RBX(%rdi);

    movq    %cr0,   %rax;
    movq    %rax,   CONTEXT_CR0(%rdi);
    movq    %cr2,   %rax;
    movq    %rax,   CONTEXT_CR2(%rdi);
    movq    %cr3,   %rax;
    movq    %rax,   CONTEXT_CR3(%rdi);
    movq    %cr4,   %rax;
    movq    %rax,   CONTEXT_CR4(%rdi);
.endm

.macro restore_context
    movq    CONTEXT_R15(%rsi), %r15;
    movq    CONTEXT_R14(%rsi), %r14;
    movq    CONTEXT_R13(%rsi), %r13;
    movq    CONTEXT_R12(%rsi), %r12;
    movq    CONTEXT_RBP(%rsi), %rbp;
    movq    CONTEXT_RSP(%rsi), %rsp;
    movq    CONTEXT_RBX(%rsi), %rbx;

    movq    CONTEXT_CR0(%rsi), %rax;
    movq    %rax,              %cr0;
    movq    CONTEXT_CR2(%rsi), %rax;
    movq    %rax,              %cr2;
    movq    CONTEXT_CR3(%rsi), %rax;
    movq    %rax,              %cr3;
    movq    CONTEXT_CR4(%rsi), %rax;
    movq    %rax,              %cr4;

    movq    CONTEXT_RIP(%rdi), %rax;
    movq    %rax,              (%rsp);
.endm

.global switch_context
.type switch_context,@function
switch_context:
    pushq   %rbp;
    movq    %rsp, %rbp;

    save_context;
    //restore_context;

    leaveq
    retq
