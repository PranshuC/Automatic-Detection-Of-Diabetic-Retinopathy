/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.7     Sun Sep 29 03:31:28 2002 (UTC)              */
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
/*      IMG_pix_expand                                                      */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      14-Dec-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*      void IMG_pix_expand(int cols,unsigned char *in_data,short *out_data)*/
/*                                                                          */
/*          cols     = length of array                                      */
/*          in_data  = pointer to input array of unsigned chars             */
/*          out_data = pointer to output array of shorts                    */
/*                                                                          */
/*      (See the C compiler reference guide.)                               */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The code takes an array of unsigned chars (pixels) and zero         */
/*      extends them up to 16 bits to form shorts.                          */
/*                                                                          */
/*      This is the C equivalent of the assembly code, without              */
/*      restrictions.  The assembly code imposes various restrictions,      */
/*      as noted under 'ASSUMPTIONS'.                                       */
/*                                                                          */
/*          void IMG_pix_expand(int cols, unsigned char *in_data,           */
/*                            short *out_data)                              */
/*          {                                                               */
/*              int j;                                                      */
/*              for (j = 0; j < cols; j++)                                  */
/*                  out_data[j] = (short) in_data[j];                       */
/*          }                                                               */
/*                                                                          */
/*  ASSUMPTIONS:                                                            */
/*      Input and Output arrays must be aligned on at least 4 byte          */
/*      boundaries.                                                         */
/*                                                                          */
/*      Input array must be a multiple of 8 elements long and contain       */
/*      at least 8 elements.                                                */
/*                                                                          */
/*      This code is LITTLE ENDIAN.                                         */
/*                                                                          */
/*  MEMORY NOTE:                                                            */
/*      No memory bank hits occur in this code. 6 words of stack frame used.*/
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The code is unrolled 8 times, with two LDWs read in a total of 8    */
/*      bytes each iteration.  The bytes are extracted into registers,      */
/*      and are then re-packed as shorts.  The packed shorts are then       */
/*      written with four STWs.                                             */
/*                                                                          */
/*      The pack is achieved using MPYU and ADD.  First, the data is        */
/*      shifted left by 15 with the MPYU by multiplying with (1 << 15).     */
/*      The value is then added to itself to shift it left one more bit.    */
/*      A final ADD merges the shifted quantity with a second quantity,     */
/*      giving the packed result.                                           */
/*                                                                          */
/*  CYCLES                                                                  */
/*      Total cycle count = 0.5*cols + 26                                   */
/*      e.g. cols = 256,  cycles = 154                                      */
/*      e.g. cols = 1076, cycles = 562                                      */
/*                                                                          */
/*  NOTES                                                                   */
/*      Interupts are diabled on entry to the function.                     */
/*                                                                          */
/*  CODESIZE                                                                */
/*      288 bytes                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_PIX_EXPAND_H_
#define IMG_PIX_EXPAND_H_ 1

void IMG_pix_expand(int cols,unsigned char *in_data,short *out_data);

#endif
/* ======================================================================== */
/*  End of file:  img_pix_expand.h                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
