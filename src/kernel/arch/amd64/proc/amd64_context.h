#ifndef AMD64_CONTEXT_H
#define AMD64_CONTEXT_H

#ifndef __ASSEMBLER__

    #include <klib/stdint.h>
    #include <proc/process.h>
    #include <arch/amd64/instructions.h>

    namespace xenon
    {
        struct context_regs
        {
            uint64_t r15;
            uint64_t r14;
            uint64_t r13;
            uint64_t r12;
            uint64_t rbp;
            uint64_t rsp;
            uint64_t rbx;
            uint64_t rip;
            uint64_t cr0;
            uint64_t cr2;
            uint64_t cr3;
            uint64_t cr4;
        };

        extern "C" void save_context(context_regs *regs);

        class amd64_context : public context
        {
            context_regs regs_;

        public:
            ~amd64_context() final
            {
            }

            context_regs *get_regs() final
            {
                return &regs_;
            }

            void save() final
            {
                save_context(&regs_);
            }
        };
    }

#else // __ASSEMBLER__

    #define CONTEXT_R15(base)  0x0(base)
    #define CONTEXT_R14(base)  0x8(base)
    #define CONTEXT_R13(base) 0x10(base)
    #define CONTEXT_R12(base) 0x18(base)
    #define CONTEXT_RBP(base) 0x20(base)
    #define CONTEXT_RSP(base) 0x28(base)
    #define CONTEXT_RBX(base) 0x30(base)
    #define CONTEXT_RIP(base) 0x38(base)
    #define CONTEXT_CR0(base) 0x40(base)
    #define CONTEXT_CR2(base) 0x48(base)
    #define CONTEXT_CR3(base) 0x50(base)
    #define CONTEXT_CR4(base) 0x58(base)

/*
    #define ADDRESSING(val) val
    #define CONTEXT_R15(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_R14(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_R13(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_R12(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_RBP(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_RSP(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_RBX(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_RIP(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_CR0(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_CR2(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_CR3(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
    #define CONTEXT_CR4(base) ADDRESSING(__builtin_offsetof(context_regs, r15))(base)
*/

#endif // __ASSEMBLER__

#endif // AMD64_CONTEXT_H
