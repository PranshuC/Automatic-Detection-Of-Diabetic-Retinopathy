/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.6     Sun Sep 29 03:31:28 2002 (UTC)              */
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
/*      IMG_pix_sat                                                         */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      21-May-2002                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*      void IMG_pix_sat(int n, short *in_data, unsigned char *out_data);   */
/*                                                                          */
/*      n        : Length of input array                                    */
/*      in_data  : Pointer to array of signed 16 bit numbers                */
/*      out_data : Pointer to array of clipped, unsigned 8 bit numbers.     */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This code performs the saturation of 16 bit signed numbers to 8     */
/*      bit unsigned numbers. If the data is over 255 it is saturated to    */
/*      255, if it is less than 0 it is saturated to 0.                     */
/*                                                                          */
/*  C CODE                                                                  */
/*      void IMG_pix_sat(int n, short *in_data, unsigned char * out_data)   */
/*      {                                                                   */
/*          int j, pixel, pel;                                              */
/*          for (j = 0; j < n; j++)                                         */
/*          {                                                               */
/*              pixel = in_data[j];                                         */
/*              pel = (unsigned char) pixel;                                */
/*              if (pixel > 0xff) pel = 0xff;                               */
/*              if (pixel < 0x00) pel = 0x00;                               */
/*              out_data[j] = pel;                                          */
/*          }                                                               */
/*      }                                                                   */
/*                                                                          */
/*      The above C code is a general implementation without                */
/*      restrictions.  The assembly code may have some restrictions, as     */
/*      noted below.                                                        */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The data is loaded in pairs of shorts, the sign bits are detected   */
/*      and the test is done to see if values are over 8 bits.  Outputs     */
/*      are packed back to gether to form words, i.e. if ( a & 0xff00) if   */
/*      ( a & 0x8000) sat to 0 else sat to 0xff                             */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The input array must be aligned on an 4 bytes boundary and be a     */
/*      multiple of 8 in length. n % 8 = 0. The code is interrupt tolerant  */
/*      interrupts are disabled during execution.                           */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      There are no bank conflicts in this code.                           */
/*      It is LITTLE ENDIAN.                                                */
/*                                                                          */
/*  CYCLES                                                                  */
/*      37 + n                                                              */
/*                                                                          */
/*      For n = 640 :  677 cycles                                           */
/*      For n = 1024: 1061 cycles                                           */
/*                                                                          */
/*  CODESIZE                                                                */
/*      448 bytes                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_PIX_SAT_H_
#define IMG_PIX_SAT_H_ 1

void IMG_pix_sat(int n, short *in_data, unsigned char *out_data);

#endif
/* ======================================================================== */
/*  End of file:  img_pix_sat.h                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
