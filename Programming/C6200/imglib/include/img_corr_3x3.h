/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.7     Fri Oct 11 16:11:42 2002 (UTC)              */
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
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      corr_3x3: 3x3 correlation with rounding for 8 bit data              */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      11-Oct-2002                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*          void IMG_corr_3x3                                               */
/*          (                                                               */
/*              const unsigned char *i_data,       // input image       //  */
/*              int        *restrict o_data,       // output image      //  */
/*              const unsigned char  mask[3][3],   // convolution mask  //  */
/*              int                  x_dim,        // width of image    //  */
/*              int                  n_out         // number of outputs //  */
/*          );                                                              */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The correlation performs a point by point multiplication of the     */
/*      3 by 3 mask with the input image.  The result of the nine           */
/*      multiplications are then summed up together to produce a            */
/*      convolution sum.  This sum is then stored to the output array.      */
/*                                                                          */
/*      The image mask to be correlated is typically part of the input      */
/*      image and indicates the area of the best match between the          */
/*      input image and mask.  The mask is moved one column at a time,      */
/*      advancing the mask over the portion of the row specified by         */
/*      'n_out'.  When 'n_out' is larger than 'x_dim', multiple rows        */
/*      will be processed.                                                  */
/*                                                                          */
/*      An application may call this kernel once per row to calculate       */
/*      the correlation for an entire image:                                */
/*                                                                          */
/*          for (i = 0; i < rows; i++)                                      */
/*          {                                                               */
/*              IMG_corr_3x3(&i_data[i * x_dim], &o_data[i * n_out],        */
/*                          mask, x_dim, n_out);                            */
/*          }                                                               */
/*                                                                          */
/*      Alternately, the kernel may be invoked for multiple rows at         */
/*      a time, although the two outputs at the end of each row will        */
/*      have meaningless values.  For example:                              */
/*                                                                          */
/*          IMG_corr_3x3(i_data, o_data, mask, x_dim, 2 * x_dim);           */
/*                                                                          */
/*      This will produce two rows of outputs into 'o_data'.  The           */
/*      outputs at locations o_data[x_dim - 2], o_data[x_dim - 1],          */
/*      o_data[2*x_dim - 2] and o_data[2*x_dim - 1] will have               */
/*      meaningless values.  This is harmless, although the application     */
/*      will have to account for this when interpreting the results.        */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The array pointed to by o_data does not alias with the array        */
/*      pointed to by i_data or mask.                                       */
/*                                                                          */
/*      The number of outputs 'n_out' must be a multiple of 2.  In cases    */
/*      where 'n_out' is not a multiple of 2, most applications can safely  */
/*      round 'n_out' up to the next multiple of 2 and ignore the extra     */
/*      outputs.  This kernel does not round 'n_out' up for the user.       */
/*                                                                          */
/*  NOTE                                                                    */
/*      This kernel is interrupt tolerant, but not interruptible.           */
/*      Interrupts are masked for the entire duration of the kernel         */
/*      by clearing GIE.                                                    */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      The mask[] array must be word aligned.  No other restrictions       */
/*      are placed on the alignments of the inputs.                         */
/*                                                                          */
/*      Up to 6 bank conflicts occur during the setup code.  No bank        */
/*                                                                          */
/*      This code assumes a LITTLE ENDIAN configuration.                    */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The inner loops are unrolled completely.  The outer loop is         */
/*      unrolled 2 times.                                                   */
/*                                                                          */
/*      To save register pressure, we store our mask values packed in       */
/*      registers.  This allows us to store our 9 element mask in 5         */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 4.5 * n_out + 35 + bc, where bc is between 0 and 5.        */
/*                                                                          */
/*      For n_out = 248, cycles = 1151.                                     */
/*                                                                          */
/*      This number includes 6 cycles of function call overhead.  The       */
/*      exact overhead will vary depending on compiler options used.        */
/*                                                                          */
/*      Up to 5 bank conflicts will occur in the setup code.  This is       */
/*      captured in the term 'bc' above.  Note that bank conflicts will     */
/*      not occur on C6211/C6711 devices.                                   */
/*                                                                          */
/*  CODESIZE                                                                */
/*      416 bytes.                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_CORR_3X3_H_
#define IMG_CORR_3X3_H_ 1

void IMG_corr_3x3
(
    const unsigned char *i_data,       // input image       //
    int        *restrict o_data,       // output image      //
    const unsigned char  mask[3][3],   // convolution mask  //
    int                  x_dim,        // width of image    //
    int                  n_out         // number of outputs //
);

#endif
/* ======================================================================== */
/*  End of file:  img_corr_3x3.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
