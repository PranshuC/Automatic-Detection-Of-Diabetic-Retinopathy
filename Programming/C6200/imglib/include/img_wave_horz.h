/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.11    Sun Sep 29 03:31:30 2002 (UTC)              */
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
/*      IMG_wave_horz                                                       */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      21-Oct-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*          void IMG_wave_horz                                              */
/*          (                                                               */
/*              const short *restrict in_data,  // Row of input pixels  //  */
/*              const short *restrict qmf,      // Low-pass QMF filter  //  */
/*              const short *restrict mqmf,     // High-pass QMF filter //  */
/*              short       *restrict out_data, // Row of output data   //  */
/*              int                   cols      // Length of input.     //  */
/*          );                                                              */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This kernel performs a 1D Periodic Orthogonal Wavelet               */
/*      decomposition.  This also performs the row decomposition in a 2D    */
/*      wavelet transform.  An input signal x[n] is low pass and high pass  */
/*      filtered and decimated by two.  This results in a reference signal  */
/*      r1[n] which is the decimated output obtained by dropping the odd    */
/*      samples of the low pass filtered output and a detail signal d[n]    */
/*      obtained by dropping the odd samples of the high-pass output.  A    */
/*      circular convolution algorithm is implemented and hence the         */
/*      wavelet transform is periodic.  The reference signal and the        */
/*      detail signal are half the size of the original signal.  The        */
/*      reference signal may then be iterated again to perform another      */
/*      scale of multi-resolution analysis.                                 */
/*                                                                          */
/* C CODE                                                                   */
/*      This is the C equivalent of the assembly code without               */
/*      restrictions: Note that the assembly code is hand optimized and     */
/*      restrictions apply as noted under "ASSUMPTIONS".                    */
/*                                                                          */
/*          void IMG_wave_horz                                              */
/*          (                                                               */
/*              const short *restrict in_data,  // Row of input pixels      */
/*              const short *restrict qmf,      // Low-pass QMF filter      */
/*              const short *restrict mqmf,     // High-pass QMF filter     */
/*              short       *restrict out_data, // Row of output data       */
/*              int                   cols      // Length of input.         */
/*          )                                                               */
/*          {                                                               */
/*              int         i, res, iters;                                  */
/*              int         j, sum, prod;                                   */
/*              const short *restrict xptr  = in_data;                      */
/*              short       *restrict yptr  = out_data;                     */
/*              const short *restrict x_end = &in_data[cols - 1];           */
/*              short       xdata, hdata;                                   */
/*              const short *restrict xstart;                               */
/*              const short *restrict filt_ptr;                             */
/*              int         M = 8;                                          */
/*                                                                          */
/*              // ------------------------------------------------- //     */
/*              //  Set our loop trip count and starting x posn.     //     */
/*              //  'xstart' is used in the high-pass filter loop.   //     */
/*              // ------------------------------------------------- //     */
/*              iters  = cols;                                              */
/*              xstart = in_data + (cols - M)  + 2;                         */
/*                                                                          */
/*              // ------------------------------------------------- //     */
/*              //  Low pass filter.  Iterate for cols/2 iterations  //     */
/*              //  generating cols/2 low pass sample points with    //     */
/*              //  the low-pass quadrature mirror filter.           //     */
/*              // ------------------------------------------------- //     */
/*              for (i = 0; i < iters; i += 2)                              */
/*              {                                                           */
/*                  // --------------------------------------------- //     */
/*                  //  Initialize our sum to the rounding value     //     */
/*                  //  and reset our pointer.                       //     */
/*                  // --------------------------------------------- //     */
/*                  sum  = Qr;                                              */
/*                  xptr = in_data + i;                                     */
/*                                                                          */
/*                  // --------------------------------------------- //     */
/*                  //  Iterate over the taps in our QMF.            //     */
/*                  // --------------------------------------------- //     */
/*                  for (j = 0; j < M; j++)                                 */
/*                  {                                                       */
/*                      xdata = *xptr++;                                    */
/*                      hdata =  qmf[j];                                    */
/*                      prod  =  xdata * hdata;                             */
/*                      sum  += prod;                                       */
/*                      if (xptr > x_end) xptr = in_data;                   */
/*                  }                                                       */
/*                                                                          */
/*                  // --------------------------------------------- //     */
/*                  //  Adjust the Qpt of our sum and store result.  //     */
/*                  // --------------------------------------------- //     */
/*                  res    = (sum >> Qpt);                                  */
/*                  *out_data++ = res;                                      */
/*              }                                                           */
/*                                                                          */
/*              // ------------------------------------------------- //     */
/*              //  High pass filter.  Iterate for cols/2 iters      //     */
/*              //  generating cols/2 high pass sample points with   //     */
/*              //  the high-pass quadrature mirror filter.          //     */
/*              // ------------------------------------------------- //     */
/*              for (i = 0; i < iters ; i+=2)                               */
/*              {                                                           */
/*                  // --------------------------------------------- //     */
/*                  //  Initialize our sum and filter pointer.       //     */
/*                  // --------------------------------------------- //     */
/*                  sum  = Qr;                                              */
/*                  filt_ptr  = mqmf + (M - 1);                             */
/*                                                                          */
/*                  // --------------------------------------------- //     */
/*                  //  Set up our data pointer.  This is slightly   //     */
/*                  //  more complicated due to how the data wraps   //     */
/*                  //  around the edge of the buffer.               //     */
/*                  // --------------------------------------------- //     */
/*                  xptr = xstart;                                          */
/*                  xstart += 2;                                            */
/*                  if (xstart > x_end) xstart = in_data;                   */
/*                                                                          */
/*                  // --------------------------------------------- //     */
/*                  //  Iterate over the taps in our QMF.            //     */
/*                  // --------------------------------------------- //     */
/*                  for ( j = 0; j < M; j++)                                */
/*                  {                                                       */
/*                      xdata = *xptr++;                                    */
/*                      hdata = *filt_ptr--;                                */
/*                      prod  = xdata * hdata;                              */
/*                      if (xptr > x_end) xptr = in_data;                   */
/*                      sum  += prod;                                       */
/*                  }                                                       */
/*                                                                          */
/*                  // --------------------------------------------- //     */
/*                  //  Adjust the Qpt of our sum and store result.  //     */
/*                  // --------------------------------------------- //     */
/*                  res = (sum >> Qpt);                                     */
/*                  *out_data++ =  res;                                     */
/*              }                                                           */
/*          }                                                               */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The main idea in optimizing the code is to issue one set of reads   */
/*      to the x array and to perform low-pass and high pass filtering      */
/*      together to maximize the number of multiplies.  The last 6          */
/*      elements of the low-pass filter and the first 6 elements of the     */
/*      high pass filter use the same input. This is used to appropriately  */
/*      change the output pointer to the low pass filter after 6            */
/*      iterations. However for the first six iterations pointer            */
/*      wrap-around can occurr and hence this creates a dependency.         */
/*      Pre-reading those 6 values outside the array prevents the checks    */
/*      that introduce this dependency.  In addtion the input data is read  */
/*      as word wide quantities and the low-pass and high-pass filter       */
/*      coefficients are stored in registers allowing for the input loop    */
/*      to be completely unrolled. Thus the code has only one               */
/*      loop. A predication register is used to reset the low-pass output   */
/*      pointer after three iterations. The merging of the loops in this    */
/*      fashion allows for the maximum number of multiplies with the        */
/*      minimum number of reads.                                            */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      This routine assumes that the number of filter taps for the qmf     */
/*      and mqmf is 8.                                                      */
/*                                                                          */
/*      Both the filters are assumed to be word aligned.                    */
/*                                                                          */
/*      The input line is assumed to be word aligned so that LDWs may be    */
/*      performed.                                                          */
/*                                                                          */
/*      This code assumes that filter coefficients are maintained as        */
/*      shorts in Q15 format.                                               */
/*                                                                          */
/*      It also assumes that input data is an array of shorts (16 bit)      */
/*      (The input is assumed to be an array of shorts to allow for         */
/*      re-using this kernel to perform Multi Resolution Analysis as        */
/*      the output of this code will feedback again as input in the         */
/*      next stage.)                                                        */
/*                                                                          */
/*      Since the transform is a dyadic wavelet cols should be a multiple   */
/*      of 2.                                                               */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      This code has no bank conflicts.                                    */
/*                                                                          */
/*      This code is ENDIAN Neutral.                                        */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code masks interrupts for nearly its entire duration. As a     */
/*      result, the code is interrupt tolerant but not interruptible.       */
/*                                                                          */
/*      This code can implement the Daubechies D4 filterbank for            */
/*      analysis with 4 vansishing moments.  The length of the analyzing    */
/*      low-pass and high pass filters is 8 in that case.                   */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cols * 4 + 5                                                        */
/*                                                                          */
/*      For cols = 256, cycles = 1029.                                      */
/*      For cols = 512, cycles = 2058.                                      */
/*                                                                          */
/*  CODESIZE                                                                */
/*      640 bytes                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_WAVE_HORZ_H_
#define IMG_WAVE_HORZ_H_ 1

void IMG_wave_horz
(
    const short *restrict in_data,  // Row of input pixels  //
    const short *restrict qmf,      // Low-pass QMF filter  //
    const short *restrict mqmf,     // High-pass QMF filter //
    short       *restrict out_data, // Row of output data   //
    int                   cols      // Length of input.     //
);

#endif
/* ======================================================================== */
/*  End of file:  img_wave_horz.h                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
