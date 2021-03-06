/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.3     Sun Sep 29 03:31:30 2002 (UTC)              */
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
/*      threshold                                                           */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      25-Jun-2000                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*      void IMG_thr_gt2max                                                 */
/*      (                                                                   */
/*          const unsigned char *in_data,     //  Input image data  //      */
/*          unsigned char *restrict out_data, //  Output image data //      */
/*          short cols, short rows,           //  Image dimensions  //      */
/*          unsigned char       threshold     //  Threshold value   //      */
/*      )                                                                   */
/*                                                                          */
/*      This routine performs a thresholding operation on an input          */
/*      image in in_data[] whose dimensions are given in the arguments      */
/*      'cols' and 'rows'.  The thresholded pixels are written to the       */
/*      output image pointed to by out_data[].  The input and output        */
/*      are exactly the same dimensions.                                    */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Pixels that are above the threshold value are written to the        */
/*      output unmodified.  Pixels that are greater than the threshold      */
/*      are set to 255 in the output image.                                 */
/*                                                                          */
/*      The exact thresholding function performed is described by           */
/*      the following transfer function diagram:                            */
/*                                                                          */
/*                 255_|          _________                                 */
/*                     |         |                                          */
/*                     |         |                                          */
/*            O        |         |                                          */
/*            U        |         |                                          */
/*            T    th _|. . . . .|                                          */
/*            P        |        /.                                          */
/*            U        |      /  .                                          */
/*            T        |    /    .                                          */
/*                     |  /      .                                          */
/*                   0_|/________.__________                                */
/*                     |         |        |                                 */
/*                     0        th       255                                */
/*                                                                          */
/*                             INPUT                                        */
/*                                                                          */
/*      Please see the IMGLIB functions IMG_thr_gt2thr, IMG_thr_le2thr,     */
/*      and IMG_thr_le2min for other thresholding functions.                */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The input and output buffers do not alias.                          */
/*                                                                          */
/*      The input and output buffers must be word aligned.                  */
/*                                                                          */
/*      The total number of pixels rows*cols must be at least               */
/*      16 and a multiple of 16.                                            */
/*                                                                          */
/*      Stack is aligned to a double-word boundary.                         */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      For performance, the code outside the loop has been interleaved as  */
/*      much as possible with the prolog and epilog code of the loops.      */
/*                                                                          */
/*      Twin stack-pointers are used to accelerate stack acceses.           */
/*                                                                          */
/*      The inner loop is unrolled 16 times and the data is maniuplated     */
/*      in packed format for speed.                                         */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      At most, two bank conflicts occur during the setup code.  No        */
/*      bank conflicts occur in the loop, regardless of the pointer         */
/*      alignment.  (The bank conflicts in the setup code will not occur    */
/*      if the stack and the input arrays are both double-word aligned.)    */
/*                                                                          */
/*      The code requires 8 words of stack space to save Save-On-Entry      */
/*      registers.                                                          */
/*                                                                          */
/*      This code is ENDIAN NEUTRAL.                                        */
/*                                                                          */
/*  NOTES                                                                   */
/*      This routine is fully interruptible.                                */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 24 + 9 * (cols * rows / 16)                                */
/*                                                                          */
/*      For cols =  64 and rows = 8,  cycles = 312.                         */
/*      For cols =  32 and rows = 32, cycles = 600.                         */
/*      For cols = 640 and rows = 8,  cycles = 2904.                        */
/*                                                                          */
/*      This number includes 6 cycles of function call overhead.  The       */
/*      exact overhead will vary depending on compiler options used.        */
/*                                                                          */
/*  CODESIZE                                                                */
/*      512 bytes                                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_THR_GT2MAX_H_
#define IMG_THR_GT2MAX_H_ 1

void IMG_thr_gt2max
(
    const unsigned char *in_data,     //  Input image data  //
    unsigned char *restrict out_data, //  Output image data //
    short cols, short rows,           //  Image dimensions  //
    unsigned char       threshold     //  Threshold value   //
);

#endif
/* ======================================================================== */
/*  End of file:  img_thr_gt2max.h                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
