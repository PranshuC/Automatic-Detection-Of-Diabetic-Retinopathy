/****************************************************************************/
/*  lnk.cmd   v1.01                                                         */
/*  Copyright (c) 1996-1997  Texas Instruments Incorporated                 */
/****************************************************************************/
-heap  0x2000
-stack 0x4000

MEMORY
{
    MEM:        o = 80000000h   l = 10000000h 
}

SECTIONS
{
    vectors     >       MEM
    .text       >       MEM

    .stack      >       MEM
    .far        >       MEM
    .switch     >       MEM 
    .tables     >       MEM
    .data       >       MEM
    .bss        >       MEM
    .sysmem     >       MEM
    .cinit      >       MEM
    .const      >       MEM
    .cio        >       MEM 
}
