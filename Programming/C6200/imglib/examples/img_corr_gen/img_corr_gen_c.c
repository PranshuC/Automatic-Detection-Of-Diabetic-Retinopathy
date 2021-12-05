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


/* ======================================================================= */
/*                                                                         */
/*  TEXAS INSTRUMENTS, INC.                                                */
/*                                                                         */
/*  NAME                                                                   */
/*      IMG_corr_gen                                                       */
/*                                                                         */
/*  REVISION DATE                                                          */
/*      20-Sep-1999                                                        */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine is C callable, and has the following C prototype:     */
/*                                                                         */
/*      void IMG_corr_gen                                                  */
/*      (                                                                  */
/*          short *in_data,      //  Input: pixel array           //       */
/*          short *h,            //  Input: 1-by-M mask           //       */
/*          short *out_data,     //  Output: correlation sum      //       */
/*          int M,               //  Input: # of filter taps      //       */
/*          int cols             //  Input: Width of input image  //       */
/*      );                                                                 */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      This routine performs a generalized correlation with a 1 by M      */
/*      tap filter. It can be called repetitively to form an arbitrary     */
/*      M by n 2D generalized correlation kernel.  The correlation sum     */
/*      is currently stored as half words.  The input pixel, and mask      */
/*      data is assumed to come in as shorts.  No restrictions are         */
/*      placed cols and M.                                                 */
/*                                                                         */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================= */

void IMG_corr_gen_c(short *x, short *h, short *y, int  m, int x_dim )
{

     int            i, j, sum, iters;
     short          *xptr, *hptr;

  /*========================================================================*/
  /* Set xptr and hptr to point to the start of the image and filter arrays */
  /* At the start of evry iteration hptr resets to the start of the filter  */
  /* while xptr advances by 1 pixel. sum is used to read the previous sum   */
  /* for this output pixel that was computed for a previous line as in the  */
  /* case of a 2D filter. The convolution sum is then accumulated into the  */
  /* existing sum. This read from memory and accumulation into the previous */
  /* sum allows this kernel to be used for a 2D generalized correlation     */
  /* kernel. The correlation sum is then computed for the m tap filter and  */
  /* accumulated into the existing sum. This is then stored out back into   */
  /* memory.                                                                */
  /*========================================================================*/

     xptr  = x;
     hptr  = h;

     iters  = (x_dim - m) ;

     for (j = 0; j < iters; j++)
     {
          sum =  y[j];

          for (i = 0; i < m; i++)
          {
               sum += xptr[i + j] * hptr[i];
          }

          y[j] = sum;
     }
}

/*============================================================================*/
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/*============================================================================*/
