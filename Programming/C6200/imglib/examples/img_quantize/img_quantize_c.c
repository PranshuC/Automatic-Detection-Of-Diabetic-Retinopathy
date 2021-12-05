/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2002 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */


/* ======================================================================== */
/*                                                                          */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_quantize -- Matrix Quantization w/ Rounding, Little Endian      */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      10-Dec-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*      void IMG_quantize                                                   */
/*      (                                                                   */
/*          short       *data,      (* Data to be quantized.        *)      */
/*          int         num_blks,   (* Number of 64-element blocks. *)      */
/*          int         blk_sz,     (* Block size (multiple of 8).  *)      */
/*          const short *recip_tbl, (* Quant. values (reciprocals). *)      */
/*          int         q_pt        (* Q-point of Quant values.     *)      */
/*      )                                                                   */
/*                                                                          */
/*      The number of blocks, num_blks, must be at least 1.  The block      */
/*      size (number of elements in each block) must be at least 1.         */
/*      The Q-point, q_pt, controls rounding and final truncation; it       */
/*      must be in the range from 0 <= q_pt <= 31.                          */
/*                                                                          */
/*      The data[] array must be 'num_blks * blk_sz' elements, and the      */
/*      recip_tbl[] array must be 'blk_sz' elements.                        */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The function IMG_quantize() quantizes matrices by multiplying their */
/*      contents with a second matrix that contains reciprocals of the      */
/*      quantization terms.  This step corresponds to the quantization      */
/*      that is performed in 2-D DCT-based compression techniques,          */
/*      although IMG_quantize() may be used on any signed 16-bit data using */
/*      signed 16-bit quantization terms.                                   */
/*                                                                          */
/*      IMG_quantize() merely multiplies the contents of the quantization   */
/*      matrix with the data being quantized.  Therefore, it may be used    */
/*      for inverse quantization as well, by setting the Q-point            */
/*      appropriately.                                                      */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The inner loop steps through individual blocks, while the           */
/*      outer loop steps through reciprocals for quantization.  This        */
/*      eliminates redundant loads for the quantization terms.              */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The number of blocks, num_blks, must be at least 1.                 */
/*      The Q-point, q_pt, must be in the range 0 <= q_pt <= 31.            */
/*                                                                          */
/*  NOTES                                                                   */
/*      No checking is performed on the input arguments for correctness.    */
/*                                                                          */
/*  SOURCE                                                                  */
/*      n/a                                                                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

void IMG_quantize_c
(
    short       *data,          /* Data to be quantized.            */
    int         num_blks,       /* Number of 64-element blocks.     */
    int         blk_size,       /* Block size (multiple of 8).      */
    const short *recip_tbl,     /* Quant. values (reciprocals).     */
    int         q_pt            /* Q-point of Quant values.         */
)
{
    short recip;                /* Reciprocal term.                 */
    int   quot;                 /* Quotient (data * reciprocal)     */
    int   round;                /* Rounding value.                  */
    int   i, j, k;              /* Loop counters.                   */

    round = 1 << (q_pt - 1);    /* 0.5, represented in our q-point  */

    /* -------------------------------------------------------------------- */
    /*  Outer loop:  Step between quantization matrix elements.             */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < blk_size; i++)
    {
        /* ---------------------------------------------------------------- */
        /*  Load a reciprocal and point to appropriate element of block.    */
        /* ---------------------------------------------------------------- */
        recip   = recip_tbl[i];
        k       = i;

        /* ---------------------------------------------------------------- */
        /*  Inner loop:  Step between blocks of elements, quantizing.       */
        /* ---------------------------------------------------------------- */
        for (j = 0; j < num_blks; j++)
        {
            quot    = data[k] * recip + round;
            data[k] = quot >> q_pt;
            k      += blk_size;
        }
    }
}

/* ======================================================================== */
/*  End of file:  img_quantize.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
