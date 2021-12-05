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
/*      IMG_median_3x3                                                     */
/*                                                                         */
/*                                                                         */
/*  REVISION DATE                                                          */
/*      12-Dec-1999                                                        */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine is C-callable and can be called as:                   */
/*                                                                         */
/*      void IMG_median_3x3                                                */
/*      (                                                                  */
/*          unsigned char   *in_data,                                      */
/*          int             cols,                                          */
/*          unsigned char   *out_data                                      */
/*      );                                                                 */
/*                                                                         */
/*      in_data  : Pointer to input array of unsigned chars                */
/*      cols     : Width of in_data                                        */
/*      out_data : Pointer to output array of unsigned chars               */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      The routine performs a 3x3 median filtering algorithm. It comes    */
/*      under the class of non-linear signal processing algorithms.        */
/*      Rather than replace the gray level at a pixel by a weighted        */
/*      average of the nine pixels including and surrounding it, the gray  */
/*      level at each pixel is replaced by the median of the nine values.  */
/*      The median of a set of nine numbers is the middle element so that  */
/*      half of the elements in the list are larger and half are smoother. */
/*      Median filtering removes the effect of extreme values from data.   */
/*      Using a wide mask to reduce the effect of noise results in         */
/*      unacceptable blurring of sharp edges in the original image.        */
/*                                                                         */
/*      void IMG_median_3x3()                                              */
/*      {                                                                  */
/*          read new vertical column of data into moving 3 column window   */
/*                                                                         */
/*          l00 = *IN1++;                                                  */
/*          l10 = *IN2++;                                                  */
/*          l20 = *IN3++;                                                  */
/*                                                                         */
/*          The data is pre-arranged as follows                            */
/*              l00, l01, l02           MAX                                */
/*              l10, l11, l12           MED                                */
/*              l20, l21, l22           MIN                                */
/*                                                                         */
/*          First sort l00,l10,l20 into MAX, MED, MIN                      */
/*                                                                         */
/*          Compare l00,l10 and sort into MAX, MIN                         */
/*          Compare l10,l20 and sort into MAX, MIN                         */
/*          Compare l00,l10 and sort into MAX, MIN                         */
/*                                                                         */
/*          3 compares and 6 moves                                         */
/*          Now sort l00,l01,l02 to find out minimum of max                */
/*              sort l10,l11,l12 to find ot median of median               */
/*              sort l20,l21,l22 to find out maximum of min                */
/*                                                                         */
/*          Now sort these three values, the middle value                  */
/*          of these three is the minimum.                                 */
/*      }                                                                  */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================= */

void IMG_median_3x3_c(unsigned char * in_data, int cols, unsigned char * out_data)
{
    unsigned char   * line0, * line1, *line2;
    int    l00_0, l01_0, l02_0, l10_0, l11_0, l12_0, l20_0, l21_0, l22_0;
    int    l00_1, l10_1, l10_2, l10_3, l20_1, l20_2;
    int    t0_1, t1_1, t1_2;
    int    i, minmax_0, maxmin_0, medmed_0;
    int    minmax_1, medmed_1, medmed_2, medmed_3;

    line0 = in_data;
    line1 = line0 + cols;
    line2 = line1 + cols;

    /*    l00   l01  l02        max's           */
    /*    l10   l11  l12        med's           */
    /*    l20   l21  l22        min's           */

    l01_0 = l11_0 = l21_0 = 127;
    l02_0 = l12_0 = l22_0 = 127;

    minmax_0 = l02_0;
    maxmin_0 = l22_0;

    for (i = 0; i < cols; i++)
    {
         l00_0 = *line0++;
         l10_0 = *line1++;
         l20_0 = *line2++;

         // sort l00_0, l10_0, l20_0

         l20_1 = l20_0; l10_1 = l10_0;
         if (l20_0 > l10_0) { l10_1 = l20_0;  l20_1 = l10_0; } // swap

         l00_1 = l00_0; l10_2 = l10_1;
         if (l10_1 > l00_0) { l00_1 = l10_1;  l10_2 = l00_0; } // swap

         l20_2 = l20_1;  l10_3 = l10_2;
         if (l20_1 > l10_2) { l10_3 = l20_1;  l20_2 = l10_2; } // swap

         if (l01_0 < minmax_0) minmax_0 = l01_0;    /* Compute min of max */
         if (l00_1 < minmax_0) minmax_0 = l00_1;

         if (l21_0 > maxmin_0) maxmin_0 = l21_0;    /* Comput max of min  */
         if (l20_2 > maxmin_0) maxmin_0 = l20_2;

         // sort l10 l11 l12

         t0_1 = l10_3;                /* Compute median of median */
         t1_1 = l11_0;
         if (l10_3 > l11_0)
         {
           t0_1 = l11_0;
           t1_1 = l10_3;
         }
         t1_2 = t1_1;
         if (t1_1 > l12_0)
         {
           t1_2 = l12_0;
         }

         medmed_0  = t1_2;
         if (t0_1 > t1_2) { medmed_0 = t0_1; } // swap

         // minmax, medmed, maxmin

         medmed_1 = medmed_0; minmax_1 = minmax_0;
         if (minmax_0 > medmed_0) { medmed_1 = minmax_0; minmax_1 = medmed_0; }

         medmed_2 = medmed_1;
         if (medmed_1 > maxmin_0) { medmed_2 = maxmin_0; }

         medmed_3 = medmed_2;
         if (minmax_1 > medmed_2) { medmed_3 = minmax_1; }

         /* Move data along by one column              */

         minmax_0 = l01_0;
         l12_0 = l11_0;
         maxmin_0 = l21_0;
         l01_0 = l00_1;
         l11_0 = l10_3;
         l21_0 = l20_2;

         *out_data++ = medmed_3;
     }
}
/*============================================================================*/
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/*============================================================================*/
