/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.11    Sun Sep 29 03:31:28 2002 (UTC)              */
/*      Snapshot date:  23-Oct-2003                                         */
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
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
/* ======================================================================== */
/*  Assembler compatibility shim for assembling 4.30 and later code on      */
/*  tools prior to 4.30.                                                    */
/* ======================================================================== */
/* ======================================================================== */
/*  End of assembler compatibility shim.                                    */
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
/*      25-Feb-2000                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*      void IMG_quantize                                                   */
/*      (                                                                   */
/*          short       *data,      // Data to be quantized.        //      */
/*          int         num_blks,   // Number of 64-element blocks. //      */
/*          int         blk_size,   // Block size (multiple of 16). //      */
/*          const short *recip_tbl, // Quant. values (reciprocals). //      */
/*          int         q_pt        // Q-point of Quant values.     //      */
/*      )                                                                   */
/*                                                                          */
/*      The number of blocks, num_blks, must be at least 1.  The block      */
/*      size (number of elements in each block) must be at least 16,        */
/*      and a multiple of 16.  The Q-point, q_pt, controls rounding and     */
/*      final truncation; it must be in the range from 0 <= q_pt <= 31.     */
/*                                                                          */
/*      Both input arrays, data[] and recip_tbl[], must be word aligned.    */
/*      The data[] array must be 'num_blks * blk_size' elements, and the    */
/*      recip_tbl[] array must be 'blk_size' elements.                      */
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
/*      The assembly code requires 25 + (blk_size/16) * (4 + num_blks*12)   */
/*      cycles to execute, including function call overhead.  Interrupts    */
/*      are disabled for 14 + (blk_size/16) * (4 + num_blks*12) cycles.     */
/*                                                                          */
/*      The following C code describes the general implementation of        */
/*      IMG_quantize().                                                     */
/*                                                                          */
/*      void IMG_quantize                                                   */
/*      (                                                                   */
/*          short       *data,      // Data to be quantized.        //      */
/*          int         num_blks,   // Number of 64-element blocks. //      */
/*          int         blk_size,   // Block size (multiple of 16). //      */
/*          const short *recip_tbl, // Quant. values (reciprocals). //      */
/*          int         q_pt        // Q-point of Quant values.     //      */
/*      )                                                                   */
/*      {                                                                   */
/*          short recip;                                                    */
/*          int i, j, k, quot, round;                                       */
/*                                                                          */
/*          // ------------------------------------------------------- //   */
/*          //  Set rounding term as 0.5, effectively.                 //   */
/*          // ------------------------------------------------------- //   */
/*          round = q_pt ? 1 << (q_pt - 1) : 0;                             */
/*                                                                          */
/*          // ------------------------------------------------------- //   */
/*          //  Outer loop:  Step between quant matrix elements.       //   */
/*          // ------------------------------------------------------- //   */
/*          for (i = 0; i < blk_size; i++)                                  */
/*          {                                                               */
/*              // --------------------------------------------------- //   */
/*              //  Load a reciprocal and point to appropriate         //   */
/*              //  element of block.                                  //   */
/*              // --------------------------------------------------- //   */
/*              recip   = recip_tbl[i];                                     */
/*              k       = i;                                                */
/*                                                                          */
/*              // --------------------------------------------------- //   */
/*              //  Inner loop:  Step between blocks of elements,      //   */
/*              //  quantizing.                                        //   */
/*              // --------------------------------------------------- //   */
/*              for (j = 0; j < num_blks; j++)                              */
/*              {                                                           */
/*                  quot    = data[k] * recip + round;                      */
/*                  data[k] = quot >> q_pt;                                 */
/*                  k      += blk_size;                                     */
/*              }                                                           */
/*          }                                                               */
/*      }                                                                   */
/*                                                                          */
/*      The above C code is a general implementation without                */
/*      restrictions.  The assembly code has restrictions, as noted above.  */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The outer loop is unrolled 16 times to allow greater amounts        */
/*      of work to be performed in the inner loop.                          */
/*                                                                          */
/*      Reciprocals and data terms are loaded in pairs with word-wide       */
/*      loads, making better use of the availably memory bandwidth.         */
/*                                                                          */
/*      The outer loop has been interleaved with the prolog and epilog      */
/*      of the inner loop.                                                  */
/*                                                                          */
/*      The inner loop is software-pipelined, with two iterations in        */
/*      parallel.  If "num_blks == 1", the inner loop kernel is branched    */
/*      over, thus eliminating the minimum loop-trip-count restriction.     */
/*                                                                          */
/*      Epilog code from the inner loop has been moved into the exit-       */
/*      code delay slots through creative use of branch delay slots.        */
/*                                                                          */
/*      Twin stack pointers have been used to speed up stack accesses.      */
/*                                                                          */
/*      The inner loop steps through individual blocks, while the           */
/*      outer loop steps through reciprocals for quantization.  This        */
/*      eliminates redundant loads for the quantization terms.              */
/*                                                                          */
/*      The direction of travel for the inner loop oscillates with each     */
/*      iteration of the outer loop to simplify pointer updating in the     */
/*      outer loop and reduce register pressure.  (eg.  in the first iter.  */
/*      of the outer loop, the inner loop steps forward through memory; in  */
/*      the second iter. of the outer loop, the inner loop steps backwards  */
/*      through memory, etc.)                                               */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The recip_tbl[] and data[] must be word aligned.                    */
/*      The block size, blk_size, must be a multiple of 16.                 */
/*      The number of blocks, num_blks, must be at least 1.                 */
/*      The Q-point, q_pt, must be in the range 0 <= q_pt <= 31.            */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts occur, regardless of the relative orientation     */
/*      of r_tbl[] and data[].                                              */
/*                                                                          */
/*      A total of 14 words of stack frame are used for saving the          */
/*      Save-On-Entry registers.                                            */
/*                                                                          */
/*  NOTES                                                                   */
/*      Interrupts are disabled throughout most of the function.            */
/*      Input arrays must be word-aligned for correct operation.            */
/*      No checking is performed on the input arguments for correctness.    */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 25 + (blk_size/16) * (4 + num_blks*12)                     */
/*                                                                          */
/*      For blk_size =  64 and num_blks =  8, cycles =  425                 */
/*      For blk_size = 256 and num_blks = 24, cycles = 4695                 */
/*                                                                          */
/*  CODESIZE                                                                */
/*      1024 bytes                                                          */
/*                                                                          */
/*  SOURCE                                                                  */
/*      n/a                                                                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_QUANTIZE_H_
#define IMG_QUANTIZE_H_ 1

void IMG_quantize
(
    short       *data,      /* Data to be quantized.        */
    int         num_blks,   /* Number of 64-element blocks. */
    int         blk_size,   /* Block size (multiple of 16);. */
    const short *recip_tbl, /* Quant. values (reciprocals);. */
    int         q_pt        /* Q-point of Quant values.     */
);

#endif
/* ======================================================================== */
/*  End of file:  img_quantize.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
