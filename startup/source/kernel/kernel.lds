SECTIONS{
    . = 0x23000;
    
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