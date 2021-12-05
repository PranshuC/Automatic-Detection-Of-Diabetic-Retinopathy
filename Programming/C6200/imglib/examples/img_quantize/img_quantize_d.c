/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_quantize_d.c -- Example file for IMG_quantize                   */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C62x IMGLIB           */
/*      IMG_quantize function. All inputs to the function contain random    */
/*      data. The assembly version of IMG_quantize is called and the        */
/*      output is compared with the reference C code using mem_compare()    */
/*      in support.c.                                                       */
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
#include "IMG_quantize.h"

/* Header file for the C function */
void IMG_quantize_c(short * data, int num_blks, int blk_size, const short * recip_tbl, int q_pt);

/* ======================================================================== */
/*  IMGLIB function-specific alignments. Refer to the                       */
/*  TMS320C62x IMG Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(data_asm,  4);
#pragma DATA_ALIGN(data_c,    4);
#pragma DATA_ALIGN(recip_tbl, 4);

/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define BLK_SIZE (64)
#define NUM_BLKS (1)
#define Q_PT     (3)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int    test = 0;
short  data_asm[64] = 
{
    -0x2EA8,  0x0A1B, -0x276E, -0x5A41, -0x23AB, -0x0B17,  0x3BD9,  0x3ED9,
     0x092E,  0x67EC, -0x1E61,  0x2473, -0x3C62, -0x31BC, -0x7B1E, -0x1F25,
     0x3701,  0x1FB6, -0x630F, -0x1161, -0x6E49,  0x2CFC, -0x6283,  0x7DA5,
     0x5D74,  0x1BE2,  0x47DF,  0x5AE4, -0x3365,  0x4095,  0x5F62,  0x0741,
     0x7FAC, -0x6399,  0x2E65,  0x554B,  0x2A43,  0x4F5F, -0x17CC, -0x316C,
    -0x69E6, -0x14CC, -0x6996, -0x3326,  0x54DC,  0x61A3,  0x709D, -0x28C6,
    -0x0C4C, -0x7D3D, -0x088A, -0x7774, -0x2121,  0x2F7F, -0x16D4, -0x6E5F,
    -0x1C09, -0x3B29,  0x4F66, -0x7B15, -0x6247,  0x47EA,  0x3DE5, -0x3445
};
short  data_c[64];

int num_blks = 1;


const  short recip_tbl[] = 
{ 
	0x1000,  0x1746,  0x1555,  0x1249,  0x1555,  0x199a,  0x1000,  0x1249,  
	0x13b1,  0x1249,  0x0e39,  0x0f0f,  0x1000,  0x0d79,  0x0aab,  0x0666,  
	0x09d9,  0x0aab,  0x0ba3,  0x0ba3,  0x0aab,  0x0539,  0x0750,  0x06eb,  
	0x08d4,  0x0666,  0x046a,  0x0505,  0x0432,  0x0444,  0x047e,  0x0505,  
	0x0492,  0x04a8,  0x0400,  0x038e,  0x02c8,  0x0348,  0x0400,  0x03c4,  
	0x02f1,  0x03b6,  0x04a8,  0x0492,  0x0333,  0x0259,  0x0329,  0x02f1,  
	0x02b2,  0x029d,  0x027c,  0x0276,  0x027c,  0x0421,  0x0353,  0x0244,  
	0x021e,  0x0249,  0x028f,  0x0222,  0x02c8,  0x0289,  0x027c,  0x0296  
};

int main()
{   
/* ------------------------------------------------------------------------ */
/*  Duplicate input for natural-C function call.                            */
/* ------------------------------------------------------------------------ */
    memcpy(data_c, data_asm, sizeof(data_asm));

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG62x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_quantize(data_asm, num_blks, BLK_SIZE, recip_tbl, Q_PT);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_quantize_c(data_c, num_blks, BLK_SIZE, recip_tbl, Q_PT);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(data_asm, "data_asm", data_c, "data_c", sizeof(data_c));
    if (test == 0) 
    {
      printf("data_asm vs. data_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  img_quantize_d.c                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
