/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_wave_horz_d.c -- Example file for IMG_wave_horz                 */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C62x IMGLIB           */
/*      IMG_wave_horz function. All inputs to the function contain random   */
/*      data. The assembly version of IMG_wave_horz is called and the       */
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
#include "IMG_wave_horz.h"

/* Header file for the C function */
void IMG_wave_horz_c(const short * in_data, const short * qmf, const short * mqmf, short * out_data, int n);

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
#define COLS (256)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int     test = 0;
const short  in_data[COLS] = 
{
     0x0489,  0x60E2,  0x3E29, -0x23C0,  0x62DF, -0x3AA9,  0x4957,  0x50DA,
    -0x6C39, -0x0451,  0x0DDE, -0x5773, -0x23B8, -0x2094,  0x49D3, -0x7386,
     0x0F41,  0x4381,  0x627F, -0x44AF, -0x097C,  0x67E6,  0x0FDD, -0x2119,
    -0x65A3, -0x38F1, -0x32F1, -0x7499,  0x6286,  0x7E9A,  0x4B32,  0x2CE3,
     0x12AA, -0x4ED3, -0x0E1E, -0x75AE,  0x4300,  0x2FD1,  0x73F9,  0x0114,
     0x2796,  0x7606, -0x1031, -0x5E8F,  0x3F0C, -0x3A85,  0x2F67, -0x6D5B,
     0x2CC9,  0x49CD,  0x5B1D,  0x5EE8, -0x03F4, -0x5EC4, -0x2988,  0x6F70,
     0x6283,  0x49D8, -0x795C, -0x20A1,  0x3E54,  0x79ED, -0x3E76, -0x5F0C,
    -0x405A, -0x4783,  0x37FA,  0x1D0E, -0x6FEF,  0x12B6,  0x0577,  0x0F1B,
    -0x4D65, -0x6EAC, -0x544A,  0x1295, -0x4F77, -0x21FC, -0x0C7B, -0x3A45,
    -0x45B7, -0x287A, -0x627E,  0x0080, -0x2097, -0x2630,  0x2B80, -0x197C,
     0x33D0,  0x0E80,  0x6241,  0x528A,  0x5654,  0x2A41,  0x4E85,  0x5720,
     0x2B7D, -0x0CDC, -0x5075, -0x59F6,  0x4A4F, -0x2161,  0x783A, -0x47ED,
    -0x724D,  0x6EBC,  0x5584,  0x1C36,  0x16FD,  0x017D,  0x67AD, -0x6302,
    -0x61E5,  0x5538, -0x7F76,  0x626B,  0x3DC6, -0x0961, -0x1755, -0x0133,
     0x3596,  0x2020,  0x3C2C, -0x2720,  0x4C3B, -0x4A5A,  0x7126,  0x7893,
    -0x2844,  0x394E, -0x3857, -0x3F47, -0x3307,  0x0096,  0x0222,  0x4598,
     0x67A8, -0x28ED,  0x0F7A, -0x2E23, -0x2BAD,  0x3C8E,  0x60C5, -0x3B60,
    -0x1BEA, -0x3D6A,  0x1493,  0x4060,  0x02D6, -0x3A87,  0x7745,  0x2120,
     0x506E,  0x625D,  0x3895, -0x090E,  0x1F76,  0x1472,  0x22EC, -0x3D5B,
    -0x6280, -0x4351,  0x758C,  0x0035, -0x180D, -0x077C, -0x0014,  0x449B,
     0x4A32, -0x2ACF,  0x72DF, -0x0E7F, -0x0288, -0x7E08, -0x0EA4,  0x3D06,
     0x1634,  0x1965, -0x4754, -0x47BF,  0x77FA, -0x171C, -0x2547,  0x5BE0,
     0x212B, -0x1105,  0x1224,  0x59A8, -0x24B0, -0x1346, -0x43E8,  0x333C,
    -0x7596, -0x7F3B, -0x389A,  0x1F2A, -0x02E3, -0x4445, -0x396F, -0x2E82,
    -0x557A,  0x6EE2, -0x24F6,  0x10FB, -0x5601,  0x40B9,  0x2895,  0x2D63,
    -0x399B,  0x742A, -0x424E, -0x23A2, -0x7096,  0x4A5F,  0x656F, -0x6A13,
     0x439C, -0x40B0, -0x61E6,  0x1F88, -0x5622, -0x70C8, -0x512A, -0x603C,
     0x2AF2, -0x39E8, -0x4192,  0x6A7D, -0x5D1B, -0x1DF3,  0x5830,  0x72E7,
    -0x0618, -0x0A08,  0x413A, -0x6CBD, -0x2555,  0x67DE, -0x2FD3, -0x1AD0,
    -0x48F2,  0x6B1C, -0x5A18, -0x596E, -0x102F, -0x5E85, -0x71A6, -0x4F7A,
     0x5D99,  0x4B1B, -0x007C,  0x3410, -0x47C1, -0x67D6,  0x71C3, -0x3AB9
};

const short qmf[8] = {
    -347, 1078, 1009, -6128,    // Low-pass filter  
    -918, 20673, 23423, 7550
};
                                
const short mqmf[8]= {
    -7550, 23423, -20673, -918, // High-Pass filter 
     6128, 1009, -1078, -347
}; 


short  out_data_asm[COLS];
short  out_data_c[COLS];


int main()
{   

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG62x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_wave_horz(in_data, qmf, mqmf, out_data_asm, COLS);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_wave_horz_c(in_data, qmf, mqmf, out_data_c, COLS);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(out_data_asm, "out_data_asm", out_data_c, "out_data_c", sizeof(out_data_c));
    if (test == 0) 
    {
      printf("out_data_asm vs. out_data_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  img_wave_horz_d.c                                         */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
