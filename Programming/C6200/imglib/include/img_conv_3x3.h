/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.4     Sun Sep 29 03:31:22 2002 (UTC)              */
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
/* TEXAS INSTRUMENTS, INC.                                                  */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_conv_3x3 -- 3x3 convolution  hand assembly                      */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      24-Jan-2002                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*      void IMG_conv_3x3( const unsigned char *restrict inptr,             */
/*                               unsigned char *restrict outptr,            */
/*                                        int            x_dim,             */
/*                         const          char *restrict mask,              */
/*                                        int            shift)             */
/*                                                                          */
/*                                                                          */
/*     The convolution routine accepts three rows of 'x_dim' input points   */
/*     and performs some operation on each.  A total of 'x_dim' outputs     */
/*     are written to the output array. The 'mask' array has the 3 by 3     */
/*     array of coefficients.                                               */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*                                                                          */
/*     The convolution kernel accepts three rows of 'x_dim' input points    */
/*     and produces one output row of 'x_dim' points using the input mask   */
/*     of 3 by 3. The user defined shift value is used to shift the convo-  */
/*     lution value, down to the byte range. The convolution sum is also    */
/*     range limited to 0..255. The shift amount is non-zero for low pass   */
/*     filters, and zero for high pass and sharpening filters.              */
/*                                                                          */
/*     The following is the C code model for the algorithm:                 */
/*                                                                          */
/*                                                                          */
/*     void IMG_conv_3x3(   const unsigned char *restrict inptr,            */
/*                               unsigned char *restrict outptr,            */
/*                                        int            x_dim,             */
/*                         const          char *restrict mask,              */
/*                                        int            shift)             */
/*     {                                                                    */
/*          const   unsigned char   *IN1,*IN2,*IN3;                         */
/*          unsigned char           *OUT;                                   */
/*                                                                          */
/*          short    pix10,  pix20,  pix30;                                 */
/*          short    mask10, mask20, mask30;                                */
/*                                                                          */
/*          int      sum,      sum00,  sum11;                               */
/*          int      i;                                                     */
/*          int      sum22,    j;                                           */
/*                                                                          */
/*          IN1      =   inptr;                                             */
/*          IN2      =   IN1 + x_dim;                                       */
/*          IN3      =   IN2 + x_dim;                                       */
/*          OUT      =   outptr;                                            */
/*                                                                          */
/*          for (j = 0; j < x_dim ; j++)                                    */
/*          {                                                               */
/*              sum = 0;                                                    */
/*                                                                          */
/*              for (i = 0; i < 3; i++)                                     */
/*              {                                                           */
/*                  pix10  =   IN1[i];                                      */
/*                  pix20  =   IN2[i];                                      */
/*                  pix30  =   IN3[i];                                      */
/*                                                                          */
/*                  mask10 =   mask[i];                                     */
/*                  mask20 =   mask[i + 3];                                 */
/*                  mask30 =   mask[i + 6];                                 */
/*                                                                          */
/*                  sum00  =   pix10 * mask10;                              */
/*                  sum11  =   pix20 * mask20;                              */
/*                  sum22  =   pix30 * mask30;                              */
/*                                                                          */
/*                  sum   +=   sum00 + sum11+ sum22;                        */
/*              }                                                           */
/*                                                                          */
/*              IN1++;                                                      */
/*              IN2++;                                                      */
/*              IN3++;                                                      */
/*                                                                          */
/*              sum = (sum >> shift);                                       */
/*              if ( sum <  0  )       sum = 0;                             */
/*              if ( sum > 255 )       sum = 255;                           */
/*              *OUT++   =       sum;                                       */
/*          }                                                               */
/*     }                                                                    */
/*                                                                          */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*     The inner loop that computes the correlation sum is completely       */
/*     unrolled and two output pixels are computed together. The mask       */
/*     values are loaded and packed as words to minimize register pre-      */
/*     ssure. Input data is re-used between the computations of the         */
/*     two output pixels, to minimize the number of loads.                  */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*     An even number of output pixels needs to be computed. The image      */
/*     x dimension is even.                                                 */
/*                                                                          */
/*  NOTES                                                                   */
/*     Code is Little Endian. Code is interrup tolerant but not interru-    */
/*     ptible.                                                              */
/*                                                                          */
/*  CYCLES                                                                  */
/*     44 + x_dim/2 * 9                                                     */
/*                                                                          */
/*  CODESIZE                                                                */
/*     768 bytes                                                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_CONV_3X3_H_
#define IMG_CONV_3X3_H_ 1

void IMG_conv_3x3( const unsigned char *restrict inptr,
                         unsigned char *restrict outptr,
                                  int            x_dim,
                   const          char *restrict mask,
                                  int            shift);

#endif
/* ======================================================================== */
/*  End of file:  img_conv_3x3.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
