OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE_ADDRESS = 0x80000000;

SECTIONS
{
    . = BASE_ADDRESS;

    .text : {
        *(.text.init)
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        *(.bss .bss.*)
        *(.sbss .sbss.*)
    }

    . = ALIGN(16);
    stack_bottom = .;
    . += 4096;
    stack_top = .;

    end = .;
}