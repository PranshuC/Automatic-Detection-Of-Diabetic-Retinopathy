/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.11    Sun Sep 29 03:31:25 2002 (UTC)              */
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
/*      IMG_histogram                                                       */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      01-May-2001                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*      void IMG_histogram                                                  */
/*      (                                                                   */
/*          unsigned char * in_data,  // input data to IMG_histogram   //   */
/*          int n,                    // number of points              //   */
/*          int accumulate,           // sign of accumulation (+1/-1)  //   */
/*          unsigned short * t_hist,  // temporary IMG_histogram buffer //  */
/*          unsigned short * hist     // final IMG_histogram output    //   */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This code takes a IMG_histogram of an array of n, 8 bit inputs.  It */
/*      returns the IMG_histogram of 256 bins at 16 bit precision.  It can  */
/*      either add or subtract to an existing IMG_histogram, using the      */
/*      'accumulate' control.                                               */
/*                                                                          */
/*      It requires some temporary storage for 4 temporary histograms,      */
/*      which are later summed together.                                    */
/*                                                                          */
/*    C CODE                                                                */
/*        void IMG_histogram                                                */
/*        (                                                                 */
/*            unsigned char * in_data,  // input data to IMG_histogram   // */
/*            int n,                    // number of points              // */
/*            int accumulate,           // sign of accumulation (+1/-1)  // */
/*            unsigned short * t_hist,  // temporary IMG_histogram buffer // */
/*            unsigned short * hist     // final IMG_histogram output    // */
/*        )                                                                 */
/*        {                                                                 */
/*           int pixel, j;                                                  */
/*           for (j = 0; j < n; j++)                                        */
/*           {                                                              */
/*              pixel = (int) in_data[j];                                   */
/*              hist[data] += accumulate;                                   */
/*           }                                                              */
/*        }                                                                 */
/*                                                                          */
/*        The above C code is a general implementation without              */
/*        restrictions.  The assembly code may have some restrictions, as   */
/*        noted below.                                                      */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      It is assumed that the temporary array of data, t_hist is           */
/*      initialised to zero.                                                */
/*                                                                          */
/*      The input array of in_data data must be aligned to a 4 byte         */
/*      boundary and n must be a multiple of 8.                             */
/*                                                                          */
/*      The maximum number of pixels that can be profiled in each bin       */
/*      is 65535 in the main IMG_histogram and the maximum n is 262143.     */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      This code operates on four interleaved IMG_histogram bins.  The     */
/*      loop is divided into two halves.  The even half operates on         */
/*      even words full of pixels and the odd half operates on odd          */
/*      words.  Both halves operate on the same 4 IMG_histogram bins.  This */
/*      introduces a memory dependency which ordinarily would degrade       */
/*      performance.  To break the memory depenencies, the two halves       */
/*      forward their results to each other.  Exact memory access           */
/*      ordering obviates the need to predicate stores The algorithm is     */
/*      ordered as follows:                                                 */
/*                                                                          */
/*      1.  Load from IMG_histogram for even half                           */
/*                                                                          */
/*      2.  Store odd_bin to IMG_histogram for odd half (previous itn.)     */
/*                                                                          */
/*      3.  if data_even == previous data_odd increment even_bin by 2       */
/*          else increment even_bin by 1, forward to odd                    */
/*                                                                          */
/*      4.  Load from IMG_histogram for odd half (current itn.)             */
/*                                                                          */
/*      5.  Store even_bin to IMG_histogram for even half                   */
/*                                                                          */
/*      6.  if data_odd == previous data_even increment odd_bin by 2        */
/*      else increment odd_bin by 1, forward to even                        */
/*                                                                          */
/*      7.  goto 1.                                                         */
/*                                                                          */
/*      With this particular ordering, forwarding is necessary between      */
/*      even/odd halves when pixels in adjacent halves need to be           */
/*      placed in the same bin.  The store is never predicated and          */
/*      occurs speculatively as it will be overwritten by the next          */
/*      value containing the extra forwarded value.                         */
/*                                                                          */
/*      The four histograms are interleaved with each bin spaced four       */
/*      half-words apart and each IMG_histogram starting in a different     */
/*      memory bank.  This allows the four IMG_histogram accesses to        */
/*      proceed in any order without worrying about bank conflicts.         */
/*      The diagram below illustrates this: (addresses are halfword         */
/*      offsets)                                                            */
/*                                                                          */
/*             0       1       2       3       4       5       6   ...      */
/*         | hst 0 | hst 1 | hst 2 | hst 3 | hst 0 | hst 1 | ...   ...      */
/*         | bin 0 | bin 0 | bin 0 | bin 0 | bin 1 | bin 1 | ...   ...      */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts should occur in this code in the optimized        */
/*      forms of hand assembly and serial assembly.                         */
/*                                                                          */
/*  NOTE                                                                    */
/*      The code is LITTLE_ENDIAN and is Interrupt tolerant.                */
/*      Interrupts are disabled during execution.                           */
/*                                                                          */
/*      With small modification this code can be used without the           */
/*      summing stage.  The sparse IMG_histogram can be accumulated into    */
/*      and the final summing loop left until the end of a series of        */
/*      histograms.                                                         */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 9/8 * n + 560                                              */
/*                                                                          */
/*      e.g.  n = 1024, cycles = 1712                                       */
/*      e.g.  n = 512,  cycles = 1136                                       */
/*                                                                          */
/*  CODESIZE                                                                */
/*      832 bytes                                                           */
/*                                                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_HISTOGRAM_H_
#define IMG_HISTOGRAM_H_ 1

void IMG_histogram
(
    unsigned char * in_data,  // input data to IMG_histogram   //
    int n,                    // number of points              //
    int accumulate,           // sign of accumulation (+1/-1)  //
    unsigned short * t_hist,  // temporary IMG_histogram buffer //
    unsigned short * hist     // final IMG_histogram output    //
);

#endif
/* ======================================================================== */
/*  End of file:  img_histogram.h                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
