/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
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
/*          Copyright (C) 2002 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */


/* ======================================================================== */
/*                                                                          */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_wave_vert : Vertical Pass of Wavelet Transform                  */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      23-Oct-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*      void IMG_wave_vert                                                  */
/*      (                                                                   */
/*          const short *restrict                                           */
/*                      *restrict in_data,                                  */
/*          const short *restrict qmf,                                      */
/*          const short *restrict mqmf,                                     */
/*          short       *restrict out_ldata,                                */
/*          short       *restrict out_hdata,                                */
/*          int cols                                                        */
/*      );                                                                  */
/*                                                                          */
/*      in_data   : Array of row pointers. Data must be word aligned.       */
/*      qmf[8]    : Low pass QMF filter. Must be 8 taps and word aligned.   */
/*      mqmf[8]   : High pass QMF filter. Must be 8 taps and word aligned.  */
/*      out_ldata : Low pass output data                                    */
/*      out_hdata : High pass output data                                   */
/*      cols      : Length of rows to process. Must be a multiple of 2.     */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The benchmark performs the vertical pass of 2D wavelet              */
/*      transform. It performs a vertical filter on 8 rows which are        */
/*      pointed to by the pointers contained in an array of pointers.       */
/*      It produces two lines worth of output, one being the low-pass       */
/*      and the other being the high pass result. Instead of performing     */
/*      a transpose on the column and re-using the wave_horz kernel,        */
/*      the vertical filter is traversed over the entire width of the       */
/*      line and the low pass and high pass filtering kernels are           */
/*      performed together.                                                 */
/*                                                                          */
/*      This implies that the low-pass and highpass filters be              */
/*      overlapped in execution so that the input data array may be         */
/*      loaded once and both filters can be exceuted in parallel.           */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      Since the wavelet transform is dyadic, cols is assumed to be a      */
/*      multiple of 2.                                                      */
/*                                                                          */
/*      The mqmf filter is constructed from the qmf as follows:             */
/*                                                                          */
/*          status = -1;                                                    */
/*          for (i = 0; i < 8; i++)                                         */
/*          {                                                               */
/*              status = status * -1;                                       */
/*              hdata  = qmf[i] * status;                                   */
/*              filter[i] = hdata;                                          */
/*          }                                                               */
/*                                                                          */
/*      The kernels assume that the number of filter taps is exactly        */
/*      8.  In addition data that is loaded for producing out_ldata[0]      */
/*      and out_hdata[0] is not identical. The data loaded for              */
/*      producing out_hdata[0] produces results at the location             */
/*                                                                          */
/*          out_lstart = o_im + ((rows >> 1) - 3) * cols                    */
/*          out_hstart = o_im + (rows >> 1) * cols                          */
/*                                                                          */
/*      Where o_im is start of output image, rows is # of rows of the       */
/*      input image, and cols is # of cols of the output image. Hence       */
/*      after three calls to the kernel in which the following output       */
/*      lines get filled the low-pass pointer should be reset to point to   */
/*      the start. Since the wavelet transform is dyadic rows is assumed    */
/*      to be a multiple of 2 as well, just like cols.                      */
/*                                                                          */
/*      The following table illustrates how ylptr and yhptr need to be      */
/*      updated at the start of each call to this function:                 */
/*                                                                          */
/*      Call#        out_ldata                  out_hdata                   */
/*        1          out_lstart                 out_hstart                  */
/*        2          out_lstart + cols          out_hstart + cols           */
/*        3          out_lstart + 2*cols        out_hstart + 2*cols         */
/*                                                                          */
/*      At this point ylptr wraps around to become o_im, while yhptr        */
/*      proceeds as usual:                                                  */
/*                                                                          */
/*        4          o_im                       out_hstart + 3*cols         */
/*                                                                          */
/*      In addition the kernel accepts a pointer to an array of             */
/*      pointers for each input line so that a working buffer of 10         */
/*      lines can be used to effectively mix DMA's and processing as        */
/*      shown below:                                                        */
/*                                                                          */
/*      ihptr                        LINE BUFFER                            */
/*      ptr0   ---->|-------------------------------------------------|     */
/*      ptr1   ---->|-------------------------------------------------|     */
/*      ...                                                                 */
/*      ptr7   ---->|-------------------------------------------------|     */
/*                                                                          */
/*      At the start of the kernel 8 input lines are filled to the          */
/*      first 8 lines and processing begins.  In the background the next    */
/*      two lines are fetched.  The pointers are moved up by 2 namely       */
/*      ptr[i] = ptr[i+2] and the last two lines now point to lines 9       */
/*      and 10 and processing starts again.  In the background the next     */
/*      two lines are brought in the first two lines of the line            */
/*      buffer.  Pointers move up again by 2 but now the last two           */
/*      pointers to line 0 and 1.  This pattern then repeats.               */
/*                                                                          */
/*      The first line to begin filtering is always obtained from           */
/*      ptr[0], the next from ptr[1] and so on.                             */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      In order to eliminate bank conflicts succesive lines in the line    */
/*      buffer or the pointers to these lines are seperated by exactly one  */
/*      word so that loads to any succesive lines may be parallelized       */
/*      together.                                                           */
/*                                                                          */
/*      This code is a LITTLE ENDIAN implementation.                        */
/*                                                                          */
/*  BIBLIOGRAPHY                                                            */
/*      Mallat, Stephane. "A Wavelet Tour of Signal Processing", pg. 309.   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#define Qpt 15
#define Qr 16384
#define Qs 32767

void IMG_wave_vert_c
(
    const short *restrict
    const       *restrict in_data,   /* Array of row pointers */
    const short *restrict qmf,       /* Low pass QMF filter   */
    const short *restrict mqmf,      /* High pass QMF filter  */
    short       *restrict out_ldata, /* Low pass output data  */
    short       *restrict out_hdata, /* High pass output data */
    int cols                     /* Length of rows to process */
)
{
   int   i, iters, j;
   int   sum_h, sum_l;
   int   prod_h, prod_l;
   short res_h, res_l;
   short xdata, hdata, ldata;
   const short *restrict filt_ptr;
   const int M = 8;

   /*-------------------------------------------------------------------*/
   /* iters: variable for the # of loop iterations. Both the low pass   */
   /* and the high pass filters produce iters points, which is also     */
   /* the width of the input line. The low-pass filter reads filter     */
   /* coefficients from qmf and the high pass filter reads filter coeff-*/
   /* icients from the conjugate mirror filter. In addition note that   */
   /* the low-pass filter the coefficients are read in increasing order */
   /* while the high pass the filter coefficients are read in the       */
   /* opposite order.                                                   */
   /*-------------------------------------------------------------------*/

   iters = cols;

   /*--------------------------------------------------------------------*/
   /* Low-Pass filter: in_data contains 8 pointers which point to input  */
   /* lines. The filters are placed vertically and input data is read    */
   /* from 8 seperate lines. Hence data-reuse is not possible when       */
   /* traversing horizontally. sum_l is initialized to Qr and contains   */
   /* the low-pass FIR sum at the end of the j loop. sum_h contains the  */
   /* accumulator result for the high pass filter in a similar fashion.  */
   /* M is assumed to be 8 by all kernels and is # filter taps for D4    */
   /*--------------------------------------------------------------------*/

   for ( i = 0; i < iters; i++)
   {
       sum_l    = Qr;
       filt_ptr = qmf;

       for ( j = 0; j < M; j++)
       {
           xdata   =  in_data[j][i];
           ldata   =  *filt_ptr++;
           prod_l  =  xdata * ldata;
           sum_l  +=  prod_l;
       }

       res_l    = (sum_l >> Qpt);
       *out_ldata++ = res_l;
   }

   /*------------------------------------------------------------------*/
   /* High-Pass filter: in_data contains 8 pointers which point to input */
   /* lines. The filters are placed vertically and input data is read  */
   /* from 8 seperate lines. Hence data-reuse is not possible when     */
   /* traversing horizontally. sum_h is initialized to Qr and contains */
   /* the low-pass FIR sum at the end of the j loop. sum_h contains the*/
   /* accumulator result for the high pass filter in a similar fashion.*/
   /* M is # filter taps and is assumed to be 8 by all kenels          */
   /*------------------------------------------------------------------*/

   for ( i = 0; i < iters; i++)
   {
       sum_h    = Qr;
       filt_ptr = mqmf + M - 1;

       for ( j = 0; j < M; j++)
       {
           xdata   =  in_data[j][i];
           hdata   =  *filt_ptr--;
           prod_h  =  xdata * hdata;
           sum_h  +=  prod_h;
       }

       res_h    = (sum_h >> Qpt);
       *out_hdata++ = res_h;
    }
}

/* ======================================================================== */
/*  End of file:  img_wave_vert.c                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
