/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_yc_demux_be16_d.c -- Example file for IMG_yc_demux_be16         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C62x IMGLIB           */
/*      IMG_yc_demux_be16 function. All inputs to the function contain      */
/*      random data. The assembly version of IMG_yc_demux_be16 is called    */
/*      and the output is compared with the reference C code using          */
/*      mem_compare() in support.c.                                         */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
    
#include <stdio.h>
#include <stdlib.h>

/* Header file for the memory compare function */
int mem_compare(const void *ptr1, const char *name1,
                const void *ptr2, const char *name2, int len);

/* Header file for the DSPLIB function */
#include "IMG_yc_demux_be16.h"

/* Header file for the C function */
void IMG_yc_demux_be16_c(int n, const unsigned char * yc, short * y, short * cr, short * cb);

/* ======================================================================== */
/*  IMGLIB function-specific alignments. Refer to the                       */
/*  TMS320C62x IMG Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(yc, 4);
#pragma DATA_ALIGN(y_asm, 4);
#pragma DATA_ALIGN(y_c, 4);
#pragma DATA_ALIGN(cr_asm, 4);
#pragma DATA_ALIGN(cr_c, 4);
#pragma DATA_ALIGN(cb_asm, 4);
#pragma DATA_ALIGN(cb_c, 4);

/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define N    (32)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int     test = 0;

unsigned char yc[N*2] = 
{
    0xF2, 0x8C, 0x24, 0xDA, 0x5F, 0x9B, 0xBE, 0x6A,
    0xDF, 0x89, 0x5B, 0xFE, 0x7B, 0xEE, 0x23, 0x20,
    0x65, 0xC4, 0x3B, 0xE7, 0x62, 0x81, 0x60, 0x69,
    0xE3, 0x63, 0x5E, 0x96, 0x98, 0x5F, 0x74, 0x5E,
    0xCB, 0xF0, 0x1E, 0xEF, 0xFB, 0x91, 0x2D, 0x49,
    0xC5, 0x27, 0x28, 0xB3, 0x33, 0x9E, 0x49, 0x47,
    0xC9, 0x13, 0x3A, 0x71, 0xEB, 0xF8, 0xBE, 0x47,
    0x94, 0x66, 0x16, 0xF5, 0xBB, 0x72, 0x8C, 0x36
};

short  y_asm[N];
short  y_c[N];

short  cr_asm[N/2];
short  cr_c[N/2];

short  cb_asm[N/2];
short  cb_c[N/2];

int main()
{   

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG62x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_yc_demux_be16(N, yc, y_asm, cr_asm, cb_asm);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_yc_demux_be16_c(N, yc, y_c, cr_c, cb_c);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(y_asm, "y_asm", y_c, "y_c", sizeof(y_c));
    if (test == 0) 
    {
      printf("y_asm vs. y_c");
      printf("\nVerification successful.\n");
    }

    test = mem_compare(cr_asm, "cr_asm", cr_c, "cr_c", sizeof(cr_c));
    if (test == 0) 
    {
      printf("cr_asm vs. cr_c");
      printf("\nVerification successful.\n");
    }

    test = mem_compare(cb_asm, "cb_asm", cb_c, "cb_c", sizeof(cb_c));
    if (test == 0) 
    {
      printf("cb_asm vs. cb_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  img_yc_demux_be16_d.c                                     */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
