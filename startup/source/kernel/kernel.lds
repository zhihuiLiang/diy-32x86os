SECTIONS{
    . = 0x37000;
    
    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss :  {
        *(.bss)
    }
}