.section .init
.global _init
.type _init, @function
_init:
    push %rbp
    movq %rsp, %rbp
    /* gcc will put contents of crtbegin.o .init section here */

.section .fini
.global _fini
.type _fini, @function
_fini:
    push %rbp
    movq %rsp, %rbp
    /* gcc will put contents of crtbegin.o .fini section here */
