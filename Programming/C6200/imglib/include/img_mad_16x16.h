/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.19    Sun Sep 29 03:31:26 2002 (UTC)              */
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
/*      IMG_mad_16x16    Minimum Absolute Difference                        */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      21-May-2002                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*      void IMG_mad_16x16                                                  */
/*      (                                                                   */
/*          const unsigned char *refImg,// reference image                  */
/*          const unsigned char *srcImg,// 16x16 block image to look for    */
/*          int pitch,                  // Width of reference image         */
/*          int                 h,      // horiz. size of search area       */
/*          int                 v,      // vert.  size of search area       */
/*          unsigned            *match  // Result                           */
/*      )                               //     match[0] is packed x, y.     */
/*                                      //     match[1] is MAD value.       */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This routine returns the location of the minimum absolute           */
/*      difference between a 16x16 search block and some block in a         */
/*      (h + 16) x (v + 16) search area. h and v are the sizes of the       */
/*      search space for the top left coordinate of the search block.       */
/*      refImg points to the top left pixel of the search area.             */
/*                                                                          */
/*           (0,0)          (h,0)      (h+16,0)                             */
/*             ;--------------+--------;                                    */
/*             ;    search    |        ;                                    */
/*             ;    space     |        ;                                    */
/*             ;              |        ;        search area                 */
/*             ;--------------+        ;                                    */
/*           (0,v)          (h,v)      ;                                    */
/*             ;                       ;                                    */
/*             ;-----------------------;                                    */
/*           (0, v+16)                 (v+16,h+16)                          */
/*                                                                          */
/*      The location is returned relative to the above coordinate system    */
/*      as x and y packed in two 16-bit quantities in a 32-bit word:        */
/*                                                                          */
/*                  31             16 15             0                      */
/*                  +----------------+----------------+                     */
/*       match[0]:  |       x        |       y        |                     */
/*                  +----------------+----------------+                     */
/*                                                                          */
/*                  31                               0                      */
/*                  +---------------------------------+                     */
/*       match[1]:  |   SAD value at location x, y    |                     */
/*                  +---------------------------------+                     */
/*                                                                          */
/*  C Code                                                                  */
/*                                                                          */
/*      void IMG_mad_16x16                                                  */
/*      (                                                                   */
/*          const unsigned char *restrict refImg,                           */
/*          const unsigned char *restrict srcImg,                           */
/*          int pitch,                                                      */
/*          unsigned int *restrict match,                                   */
/*          int h, int v                                                    */
/*      )                                                                   */
/*      {                                                                   */
/*          int i, j, x, y, matx, maty;                                     */
/*          unsigned matpos, matval;                                        */
/*                                                                          */
/*          matval = ~0U;                                                   */
/*          matx   = maty = 0;                                              */
/*                                                                          */
/*          for (x = 0; x < h; x++)                                         */
/*              for (y = 0; y < v; y++)                                     */
/*              {                                                           */
/*                  unsigned acc = 0;                                       */
/*                                                                          */
/*                  for (i = 0; i < 16; i++)                                */
/*                      for (j = 0; j < 16; j++)                            */
/*                          acc += abs(srcImg[i*16 + j]                     */
/*                              - refImg[(i+y)*pitch + x + j]);             */
/*                                                                          */
/*                  if (acc < matval)                                       */
/*                  {                                                       */
/*                      matval = acc;                                       */
/*                      matx   = x;                                         */
/*                      maty   = y;                                         */
/*                  }                                                       */
/*              }                                                           */
/*                                                                          */
/*          matpos = (0xffff0000 & (matx << 16)) | (0x0000ffff & maty);     */
/*          match[0] = matpos;                                              */
/*          match[1] = matval;                                              */
/*      }                                                                   */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      Every inner loop iteration computes 4 pixel differences each for    */
/*      two vertically adjacent search locations. 4 iterations are          */
/*      therefore required to compute one line and 4*16=64 iterations       */
/*      to compute the complete SADs of the two search locations.           */
/*      Delay slot stuffing and outer loop branch overhead is minimized.    */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      v must be a multiple of 2.                                          */
/*      srcImg and refImg do not alias in memory                            */
/*      No special alignment of src_data or ref_data is expected.           */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      At most one bank conflict can occurr over the whole kernel. No      */
/*      special alignment of data arrays is expected. This is because the   */
/*      src and ref pixels do not parallelize in their loads.               */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code is Little Endian.                                         */
/*      This code masks interrupts for nearly its entire duration.          */
/*      As a result, the code is interrupt-tolerant, but not                */
/*      interruptible.                                                      */
/*                                                                          */
/*  CYCLES                                                                  */
/*      (63 * 7 + 22) * h * v / 2 + 21 = 463 * h * v / 2 + 21               */
/*                                                                          */
/*      Example: h= 4, v= 4:   3,725 cycles                                 */
/*               h=64, v=32: 474,133 cycles                                 */
/*                                                                          */
/*  CODESIZE                                                                */
/*      832 bytes                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_MAD_16X16_H_
#define IMG_MAD_16X16_H_ 1

void IMG_mad_16x16
(
    const unsigned char *refImg,// reference image
    const unsigned char *srcImg,// 16x16 block image to look for
    int pitch,                  // Width of reference image
    int                 h,      // horiz. size of search area
    int                 v,      // vert.  size of search area
    unsigned            *match  // Result
);                               //     match[0] is packed x, y.

#endif
/* ======================================================================== */
/*  End of file:  img_mad_16x16.h                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
