/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_wave_vert_d.c -- Example file for IMG_wave_vert                 */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C62x IMGLIB           */
/*      IMG_wave_vert function. All inputs to the function contain random   */
/*      data. The assembly version of IMG_wave_vert is called and the       */
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
#include "IMG_wave_vert.h"

/* Header file for the C function */
void IMG_wave_vert_c(const short ** in_data_ptr, const short * qmf, const short * mqmf, short * out_ldata, short * out_hdata, int n);

/* ======================================================================== */
/*  IMGLIB function-specific alignments. Refer to the                       */
/*  TMS320C62x IMG Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(in_data, 4);
#pragma DATA_ALIGN(qmf, 4);
#pragma DATA_ALIGN(mqmf, 4);
/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define COLS (32)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int   test = 0;
const short  in_data[COLS*8] = 
{
    -0x432A, -0x362C,  0x6BAB,  0x608B, -0x1783,  0x3883,  0x38ED,  0x22A6,
     0x63EF, -0x5505,  0x6A90,  0x5E5A, -0x3BDC, -0x4293,  0x637C, -0x27E3,
     0x2D92, -0x31A5,  0x0DEF, -0x7E67, -0x071A,  0x074B, -0x4CEB,  0x0FF3,
     0x7DFD,  0x0FCA, -0x5DDF, -0x1936,  0x6398,  0x33AA,  0x32EB,  0x7969,
     0x1185, -0x1D23, -0x1638,  0x6D8F,  0x492E,  0x42CB,  0x21C4, -0x0B87,
     0x48B6,  0x7AD5,  0x29EA,  0x311C, -0x6829, -0x1E82,  0x1495,  0x7DAA,
     0x231E, -0x5800, -0x2729, -0x62BC, -0x6AB5, -0x551C,  0x2724, -0x2D98,
    -0x3DE2,  0x7D7E, -0x6ACD,  0x4872,  0x2FE6, -0x2A60, -0x0A66,  0x7D2E,
     0x5962, -0x1229, -0x70E9,  0x33DF, -0x3104,  0x659D,  0x0E88,  0x3179,
     0x233B, -0x2B18,  0x393F, -0x2166, -0x0670,  0x34B9,  0x1EC6, -0x0887,
     0x2BF7, -0x4321, -0x03EC,  0x7E40, -0x31F7,  0x3AB5, -0x487C,  0x142D,
     0x4C20, -0x3F5C,  0x0ABD, -0x3396, -0x54C8, -0x556F, -0x5402, -0x5844,
     0x2DD3, -0x4B47, -0x3179,  0x084C,  0x6533,  0x1227,  0x3DA8, -0x7089,
    -0x5AE4, -0x7528,  0x5DB6,  0x2B2E, -0x6505, -0x14F9, -0x0909, -0x6BA9,
     0x6298, -0x7C19,  0x601A, -0x1227, -0x15E5,  0x5E0D, -0x3E4D, -0x37A5,
     0x5CD3,  0x5297,  0x4EFB, -0x2F7F,  0x0776, -0x0D40,  0x3542, -0x4878,
     0x0A3D, -0x1F31,  0x6885,  0x4F06, -0x2C60,  0x3184,  0x268B,  0x4E38,
     0x5B61, -0x6A82, -0x12BA,  0x51FB,  0x29F2, -0x0BFD, -0x6C0C,  0x5983,
    -0x698C, -0x7750,  0x2B5C,  0x2941,  0x0280,  0x218F, -0x4DBF,  0x11AC,
     0x76CF,  0x05C1,  0x7895, -0x425C,  0x4AB9,  0x4736,  0x6F89, -0x7D4A,
     0x4520, -0x0D8E,  0x2520, -0x1BA2, -0x2CC9,  0x4431, -0x150B,  0x7359,
    -0x577E, -0x53CA,  0x346F, -0x47ED, -0x7A2F, -0x5F1E, -0x544D,  0x1BEB,
    -0x795C, -0x289F,  0x4B81,  0x5D58,  0x0B6C, -0x1BEC,  0x1063,  0x2EF3,
     0x5365,  0x5F26, -0x47D8, -0x5669,  0x0D7C,  0x6E3B,  0x173D, -0x6288,
    -0x1906, -0x43BE,  0x65E1,  0x47BF,  0x3567, -0x416D,  0x2067,  0x1066,
    -0x240E,  0x06CD, -0x3DC1,  0x1BEA,  0x0F1B,  0x3D95,  0x703E,  0x5ACD,
     0x7EBA,  0x1961,  0x267D,  0x1FE5,  0x2263, -0x2D10,  0x30C7, -0x3D07,
     0x2765,  0x4DE2, -0x2B91, -0x68C5, -0x7C9D,  0x563C,  0x7F30, -0x0EFD,
    -0x0C5E, -0x3296, -0x012C,  0x0C47,  0x75AB,  0x360E, -0x70FA,  0x23B0,
    -0x3C4B, -0x5B65,  0x402B, -0x7A6C, -0x6AFE,  0x199B,  0x4FB8,  0x3C2E,
    -0x7FC9, -0x0BB4, -0x2FD7,  0x70C1, -0x6A20,  0x6103,  0x0E25, -0x1A97,
    -0x49DB, -0x4A0B,  0x626E, -0x2318,  0x671B, -0x4356, -0x7CD8, -0x2854
};

const short  *in_data_ptr[8] = { 
    &in_data[COLS*0], 
    &in_data[COLS*1], 
    &in_data[COLS*2], 
    &in_data[COLS*3],
    &in_data[COLS*4], 
    &in_data[COLS*5], 
    &in_data[COLS*6], 
    &in_data[COLS*7]
};

const short qmf[8] = {
    -347, 1078, 1009, -6128,    // Low-pass filter  
    -918, 20673, 23423, 7550
};
                                
const short mqmf[8]= {
    -7550, 23423, -20673, -918, // High-Pass filter 
     6128, 1009, -1078, -347
}; 


short  out_ldata_asm[COLS];
short  out_ldata_c[COLS];

short  out_hdata_asm[COLS];
short  out_hdata_c[COLS];


int main()
{   

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG62x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_wave_vert(in_data_ptr, qmf, mqmf, out_ldata_asm, out_hdata_asm, COLS);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_wave_vert_c(in_data_ptr, qmf, mqmf, out_ldata_c, out_hdata_c, COLS);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(out_ldata_asm, "out_ldata_asm", out_ldata_c, "out_ldata_c", sizeof(out_ldata_c));
    if (test == 0) 
    {
      printf("out_ldata_asm vs. out_ldata_c");
      printf("\nVerification successful.\n");
    }

    test = mem_compare(out_hdata_asm, "out_hdata_asm", out_hdata_c, "out_hdata_c", sizeof(out_hdata_c));
    if (test == 0) 
    {
      printf("out_hdata_asm vs. out_hdata_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  img_wave_vert_d.c                                         */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
