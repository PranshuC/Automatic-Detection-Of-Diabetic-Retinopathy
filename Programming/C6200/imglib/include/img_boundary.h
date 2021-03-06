/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.9     Sun Sep 29 03:31:21 2002 (UTC)              */
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
/*  NAME                                                                    */
/*      IMG_boundary -- Returns coordinates of IMG_boundary pixels.         */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      10-Jul-2001                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and is called as follows:               */
/*                                                                          */
/*          void IMG_boundary                                               */
/*          (                                                               */
/*              const unsigned char *restrict i_data,                       */
/*              int rows, int cols,                                         */
/*              int *restrict o_coord,                                      */
/*              int *restrict o_grey                                        */
/*          );                                                              */
/*                                                                          */
/*      The arguments are defined as follows:                               */
/*                                                                          */
/*          i_data   Input images that is cols-by-rows in size.             */
/*          rows     Height of the input image                              */
/*          cols     Width of the input image                               */
/*          o_coord  Array to write output coordinates to                   */
/*          o_grey   Array to write output grey levels to                   */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This routine scans an image looking for non-zero pixels.            */
/*      The locations of those pixels are stored out to the o_coord         */
/*      as packed Y/X pairs, with Y in the upper half, and X in             */
/*      the lower half.  The grey levels encountered are stored             */
/*      in the o_grey array in parallel.                                    */
/*                                                                          */
/*      The following is a C code description of the kernel without         */
/*      restrictions.  This implementation has restrictions as noted        */
/*      in the assumptions below.                                           */
/*                                                                          */
/*        void IMG_boundary                                                 */
/*        (                                                                 */
/*            const unsigned char *restrict i_data,                         */
/*            int rows, int cols,                                           */
/*            int *restrict o_coord,                                        */
/*            int *restrict o_grey                                          */
/*        )                                                                 */
/*        {                                                                 */
/*            int x, y, p;                                                  */
/*                                                                          */
/*            for (y = 0; y < rows; y++)                                    */
/*                for (x = 0; x < cols; x++)                                */
/*                    if ((p = i_data[x + y*cols] != 0)                     */
/*                    {                                                     */
/*                        *o_coord++ = ((y & 0xFFFF) << 16) |               */
/*                                      (x & 0xFFFF);                       */
/*                        *o_grey++  = p;                                   */
/*                    }                                                     */
/*        }                                                                 */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      At least one row is being processed.                                */
/*                                                                          */
/*      Input is word aligned.                                              */
/*                                                                          */
/*      Input is a multiple of 4 pixels wide.                               */
/*                                                                          */
/*      Output buffers o_coord and o_grey start in different banks          */
/*      and are word-aligned.                                               */
/*                                                                          */
/*      No more than 32764 rows or 32764 columns are being processed.       */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      Outer and inner loops are collapsed together.                       */
/*      Inner loop is unrolled to process four pixels per iteration.        */
/*      Packed coordinate value is updated directly, rather than            */
/*      by repacking x, y every time it's needed.                           */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts occur as long as o_coord and o_grey start         */
/*      in different banks.  If they start in the same bank, every          */
/*      access to each array will cause a bank conflict.                    */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code blocks interrupts for nearly its entire duration.         */
/*      It is interrupt tolerant, but not interruptible.                    */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = (5 * rows * cols) / 4 + 12.                                */
/*                                                                          */
/*  CODESIZE                                                                */
/*      160 bytes                                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_BOUNDARY_H_
#define IMG_BOUNDARY_H_ 1

void IMG_boundary
(
    const unsigned char *restrict i_data,
    int rows, int cols,
    int *restrict o_coord,
    int *restrict o_grey
);

#endif
/* ======================================================================== */
/*  End of file:  img_boundary.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
