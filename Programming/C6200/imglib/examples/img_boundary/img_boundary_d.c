/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_boundary_d.c -- Example file for IMG_boundary                   */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C64x IMGLIB           */
/*      IMG_boundary function. All inputs to the function contain random    */
/*      data. The assembly version of IMG_boundary is called and the        */
/*      output is compared with the reference C code using mem_compare()    */
/*      in support.c.                                                       */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Header file for the memory compare function */
int mem_compare(const void *ptr1, const char *name1,
                const void *ptr2, const char *name2, int len);

/* Header file for the DSPLIB function */
#include "IMG_boundary.h"

/* Header file for the C function */
void IMG_boundary_c(const unsigned char * in_data, int ROWS, int COLS, int * out_coord, int * out_gray);

/* ======================================================================== */
/*  IMGLIB function-specific alignments. Refer to the                       */
/*  TMS320C64x IMG Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(in_data, 4);
#pragma DATA_ALIGN(out_coord_asm, 4);
#pragma DATA_ALIGN(out_coord_c, 4);
#pragma DATA_ALIGN(out_gray_asm, 4);
#pragma DATA_ALIGN(out_gray_c, 4);

/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define cols (128)
#define rows (3)
#define N    (cols*rows)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int   test = 0;
const unsigned char  in_data[N] = 
{
     0x99,  0xE8,  0xE8,  0x79,  0xAD,  0x4F,  0x63,  0x5E,
     0xCD,  0x48,  0x89,  0xF5,  0x48,  0x3F,  0xB4,  0xD8,
     0xA5,  0x7B,  0x42,  0x78,  0xD1,  0xE6,  0x6E,  0xDD,
     0x1C,  0x18,  0x03,  0x3D,  0xEB,  0x14,  0x72,  0xC3,
     0x29,  0x41,  0x01,  0xFD,  0xEA,  0x24,  0x51,  0x94,
     0x10,  0x19,  0x45,  0x68,  0x8B,  0xF0,  0x45,  0xE5,
     0x37,  0xB9,  0x96,  0x0A,  0xCD,  0x0D,  0x03,  0x17,
     0x24,  0xAE,  0x6C,  0xA2,  0x2D,  0x34,  0x11,  0xD5,
     0xD0,  0x2D,  0x1C,  0xAD,  0x29,  0xDE,  0x59,  0x8D,
     0x9A,  0x70,  0xFF,  0x7E,  0x54,  0x8F,  0x31,  0x83,
     0xE9,  0x91,  0x6E,  0x3D,  0xF8,  0x0C,  0xBC,  0x65,
     0xB2,  0x42,  0x8E,  0x38,  0xB2,  0x2F,  0xAE,  0x47,
     0xC7,  0x8F,  0x7A,  0x2D,  0x4E,  0x84,  0x8C,  0xB4,
     0x47,  0x95,  0x36,  0x85,  0x96,  0x03,  0x6C,  0xB7,
     0x14,  0xCA,  0xDC,  0x1F,  0xA8,  0xD4,  0xA8,  0xEA,
     0xCB,  0xFA,  0xC7,  0xDC,  0x01,  0xB3,  0x85,  0x06,
     0x5D,  0x97,  0x5A,  0x1A,  0x44,  0x04,  0xE9,  0xF3,
     0xFF,  0x63,  0xAE,  0x2D,  0x70,  0x5C,  0x21,  0x2A,
     0xE5,  0xB2,  0xD8,  0x20,  0x33,  0x32,  0x8D,  0xEA,
     0xE5,  0xC3,  0xF1,  0x66,  0xAF,  0x32,  0x7A,  0x04,
     0xE6,  0xBE,  0xB6,  0x7C,  0x1D,  0x56,  0xC4,  0xE9,
     0xB4,  0xE8,  0xCA,  0x45,  0x64,  0x7E,  0xE5,  0x47,
     0xBB,  0x83,  0xDC,  0x8F,  0xFB,  0xB4,  0xFF,  0xB6,
     0xDE,  0xB1,  0xAF,  0x4B,  0x0D,  0xBC,  0x03,  0xE2,
     0x98,  0x40,  0xCC,  0xEF,  0x2D,  0xD5,  0x9A,  0x33,
     0x6E,  0x1C,  0xE7,  0xDE,  0xA5,  0xBA,  0x7B,  0x14,
     0xA9,  0x89,  0x81,  0x17,  0xBC,  0x23,  0x0F,  0xF2,
     0x3F,  0xAE,  0xEB,  0xE0,  0x37,  0xFB,  0xDC,  0x8C,
     0xDC,  0x83,  0x00,  0x10,  0xA7,  0x6E,  0xC6,  0x46,
     0xE9,  0x37,  0x2A,  0x4E,  0x03,  0xCF,  0x92,  0x2A,
     0xA6,  0x78,  0xC6,  0xBB,  0x92,  0x03,  0x28,  0xA7,
     0x4E,  0x0A,  0x0D,  0xB6,  0x03,  0xD4,  0xFA,  0x42,
     0x7F,  0x07,  0x99,  0xAF,  0x31,  0x13,  0x9C,  0x5B,
     0x39,  0x12,  0x38,  0x8C,  0xF8,  0xD5,  0x11,  0x75,
     0xF0,  0x86,  0x58,  0xD7,  0x40,  0x99,  0x85,  0xA5,
     0x6A,  0x62,  0x68,  0xA5,  0x46,  0x3C,  0x46,  0x96,
     0x5F,  0x41,  0xC7,  0x3B,  0xF8,  0xF8,  0x69,  0xF0,
     0x03,  0x32,  0x76,  0x7C,  0x4D,  0x90,  0x62,  0x11,
     0x13,  0xB4,  0x81,  0xA0,  0x33,  0x00,  0xDC,  0xBD,
     0x39,  0x5F,  0x58,  0xEC,  0xFC,  0xAB,  0x9D,  0xD9,
     0xF6,  0xF6,  0x89,  0xF4,  0xC6,  0x19,  0xD9,  0x4D,
     0xB8,  0xE0,  0x2B,  0x8F,  0xDE,  0xAA,  0x45,  0xDF,
     0x26,  0xD9,  0x20,  0xA5,  0xBF,  0xBD,  0x9E,  0x40,
     0x1D,  0xCD,  0x98,  0xF5,  0xC9,  0xBE,  0x97,  0x79,
     0x5E,  0x90,  0xDF,  0x5D,  0x88,  0x86,  0x3D,  0xAB,
     0x6C,  0xE0,  0x26,  0x4B,  0xFD,  0x5C,  0xA9,  0x55,
     0xB0,  0xD9,  0x39,  0xCF,  0x1D,  0x6B,  0x69,  0x5F,
     0x7C,  0x91,  0x4C,  0x40,  0x5A,  0x80,  0x8D,  0xD2
};



int  out_coord_asm[N];
int  out_coord_c[N];

int  out_gray_asm[N];
int  out_gray_c[N];

int main()
{   
/* ======================================================================== */
/*  Clear output arrays.                                                    */ 
/* ======================================================================== */   
    memset(out_coord_asm, 0, sizeof(out_coord_asm));
    memset(out_coord_c,   0, sizeof(out_coord_c)  );
    memset(out_gray_asm,  0, sizeof(out_gray_asm) );
    memset(out_gray_c,    0, sizeof(out_gray_c)   );

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG64x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_boundary(in_data, rows, cols, out_coord_asm, out_gray_asm);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_boundary_c(in_data, rows, cols, out_coord_c, out_gray_c);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(out_coord_asm, "out_coord_asm", out_coord_c, "out_coord_c", sizeof(out_coord_c));
    if (test == 0) 
    {
      printf("out_coord_asm vs. out_coord_c");
      printf("\nVerification successful.\n");
    }

    test = mem_compare(out_gray_asm, "out_gray_asm", out_gray_c, "out_gray_c", sizeof(out_gray_c));
    if (test == 0) 
    {
      printf("out_gray_asm vs. out_gray_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  img_boundary_d.c                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
