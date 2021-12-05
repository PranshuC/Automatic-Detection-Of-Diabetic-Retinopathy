MEMORY
{
    INTMEM:   o = 00000400h   l = 0000FC00h                    
}

SECTIONS
{
    .cinit      >       INTMEM
    .text       >       INTMEM
    .stack      >       INTMEM
    .bss        >       INTMEM
    .const      >       INTMEM
    .data       >       INTMEM
    .far        >       INTMEM
    .switch     >       INTMEM
    .sysmem     >       INTMEM
    .tables     >       INTMEM
    .cio        >       INTMEM
}                             

