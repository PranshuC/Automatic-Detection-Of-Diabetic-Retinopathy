/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.13    Sun Sep 29 03:31:22 2002 (UTC)              */
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
/*      IMG_corr_gen : Generalized Correlatoion                             */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      29-Jan-2002                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*      void IMG_corr_gen                                                   */
/*      (                                                                   */
/*          short *in_data,      //  Input: pixel array           //        */
/*          short *h,            //  Input: 1-by-M mask           //        */
/*          short *out_data,     //  Output: correlation sum      //        */
/*          int M,               //  Input: # of filter taps      //        */
/*          int cols             //  Input: Width of input image  //        */
/*      );                                                                  */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This routine performs a generalized correlation with a 1 by M       */
/*      tap filter. It can be called repetitively to form an arbitrary      */
/*      M by n 2D generalized correlation kernel.  The correlation sum      */
/*      is currently stored as half words.  The input pixel, and mask       */
/*      data is assumed to come in as shorts.  No restrictions are          */
/*      placed cols and M.                                                  */
/*                                                                          */
/*  C CODE                                                                  */
/*      The C code is a general implementation without restrictions.        */
/*      The assembly code may have some restrictions, as noted below.       */
/*                                                                          */
/*      void IMG_corr_gen(short *in_data, short *h, short *out_data,int M,  */
/*      int cols)                                                           */
/*      {                                                                   */
/*                                                                          */
/*         //--------------------------------------------------------//     */
/*         // For all columns compute a M tap fir                    //     */
/*         // Add correlation sum to existing value to allow for a   //     */
/*         // generalized 2D correlation to be built using several   //     */
/*         // 1D correlations                                        //     */
/*         //--------------------------------------------------------//     */
/*                                                                          */
/*         for (j = 0; j < cols - M; j++)                                   */
/*         {                                                                */
/*            sum =  out_data[j];                                           */
/*            for (i = 0; i < M; i++)                                       */
/*            {                                                             */
/*               sum += in_data[i + j] * h[i];                              */
/*            }                                                             */
/*            out_data[j] = sum;                                            */
/*         }                                                                */
/*      }                                                                   */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      Since this kernel is a generalized correlation kernel the # of      */
/*      filter taps can be as small as 1.  Hence it is not beneficial       */
/*      to pipeline this loop.  In addition collapsing both the loops       */
/*      causes data dependencies and degrades the performance.  However     */
/*      this is one of the few cases where loop order interchange can       */
/*      be used effectively.  In this case the outer loop of the            */
/*      natural C code is exchanged to be the inner loop that is to be      */
/*      software pipe- lined.  it is beneficial to pieline this loop        */
/*      because typical image dimensions are larger than the # of           */
/*      filter taps.  Notice however that the # of loads and stores do      */
/*      go up within this loop as compared to the natural C code.           */
/*                                                                          */
/*      The hand assembly version of the code tries to balance the          */
/*      computing with the loads/stores that have to be performed           */
/*      because of the loop order interchange.  In order to do this         */
/*      word-wide loads are used.  In addition the outer loop that          */
/*      computes one filter-tap at a time is unrolled and computes two      */
/*      filter taps at a time.  This has certain ramifications that are     */
/*      discussed later.  In order to decrease the # of loads the first     */
/*      word is pre-loaded outside the j loop and data is re-used           */
/*      within the j loop.  The updating of the present correl- ation       */
/*      sum with the previous correlation sum is done using the add2        */
/*      instruction.  In addition to get better multiplier utilization      */
/*      the inner loop is unrolled four times and four outputs are          */
/*      computed together.  The inner loop unrolling also has certain       */
/*      ramifications that are explained below.                             */
/*                                                                          */
/*      Special Cases                                                       */
/*                                                                          */
/*      Unrolling the outer loop assumes that there are an even number      */
/*      of filter taps.  Two cases arise:                                   */
/*                                                                          */
/*      a) M = 1.  In this case a seperate version that does just 1 tap     */
/*      needs to be used and the code should directly start from this       */
/*      loop without executing the even version.                            */
/*                                                                          */
/*      b) M is odd.  In this case the evn version of the loop is used      */
/*      for as many even taps as possible and then the last tap is          */
/*      computed using the odd tap special version created for M = 1.       */
/*                                                                          */
/*      In this way problems associated with unrolling the outer loop       */
/*      are overcome.                                                       */
/*                                                                          */
/*      Unrolling the inner loop four times assumes that the loop           */
/*      iteration cols - M is a multiple of four.  In most typical          */
/*      images cols is a multiple of 8 but since M is totally general       */
/*      cols - M is definitely not a multiple of 4.  In addition if cols    */
/*      - M is not a multiple of 4 then the inner loop iterates fewer       */
/*      times than required and certain output pixels will not be           */
/*      computed.  This problem is overcome as foll                         */
/*                                                                          */
/*      a) Increment (cols - M) by 4 so that the next higher multiple of    */
/*      4 is computed.  This implies that in certain cases up to 4          */
/*      extra pixels may be computed if cols - M is an exact multiple of    */
/*      4.  In other cases 1,2 or 3 extra pixels may be computed.  In       */
/*      order to annull this extra computation 4 locations starting at      */
/*      x[cols-M] are zeroed out before returning to ensure identical       */
/*      behaviour with the natural C code.                                  */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      a) input image, output array must be word-alligned                  */
/*                                                                          */
/*      b) output array must have at-least a dimension of cols -            */
/*      M + 4                                                               */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts should occurr in this kernel.                     */
/*      This code is ENDIAN NEUTRAL                                         */
/*                                                                          */
/*  NOTE                                                                    */
/*      This code masks interrupts for nearly its entire duration.          */
/*      The code is interrupt-tolerant, but not interruptible               */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cols = width of the image                                           */
/*      M    = # filter taps                                                */
/*                                                                          */
/*      when M is even:                                                     */
/*          (cols - M + 24) * M/2 + 36                                      */
/*      when M is odd:                                                      */
/*          (cols - M + 23) * (M-1)/2 + (cols - M + 3) * 3/4 + 50           */
/*                                                                          */
/*      For M = 8, cols = 720: 2980 cycles                                  */
/*      For M = 9, cols = 720: 3520 cycles                                  */
/*                                                                          */
/*  CODESIZE                                                                */
/*      768 bytes                                                           */
/*                                                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_CORR_GEN_H_
#define IMG_CORR_GEN_H_ 1

void IMG_corr_gen
(
    short *in_data,      //  Input: pixel array           //
    short *h,            //  Input: 1-by-M mask           //
    short *out_data,     //  Output: correlation sum      //
    int M,               //  Input: # of filter taps      //
    int cols             //  Input: Width of input image  //
);

#endif
/* ======================================================================== */
/*  End of file:  img_corr_gen.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
