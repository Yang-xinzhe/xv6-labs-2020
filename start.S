.section .text.init
.global _start

_start:
    la sp, stack_top

    call main

hang:
    j hang