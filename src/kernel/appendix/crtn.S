.section .init
    /* gcc will put contents of crtend.o .init section here */
    popq    %rbp
    ret

.section .fini
    /* gcc will put contents of crtend.o .fini section here */
    popq    %rbp
    ret
