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
/*      IMG_mad_8x8  - 8x8 Minimum Absolute Differences                     */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      03-Jan-2001                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*      void IMG_mad_8x8                                                    */
/*      (                                                                   */
/*          const unsigned char *restrict refImg,                           */
/*          const unsigned char *restrict srcImg,                           */
/*          int pitch,                                                      */
/*          int                 h,                                          */
/*          int                 v,                                          */
/*          unsigned            *restrict match                             */
/*      )                                                                   */
/*                                                                          */
/*      refImg          Reference image.                                    */
/*      srcImg[64]      8x8 block image to look for.                        */
/*      pitch           Width of reference image.                           */
/*      h               Horiz. size of search area.                         */
/*      v               Vert.  size of search area.                         */
/*      match[2]        Result:                                             */
/*                          match[0] is packed x, y.                        */
/*                          match[1] is MAD value.                          */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This routine returns the location of the minimum absolute           */
/*      difference between a 8x8 search block and some block in a           */
/*      (h + 8) x (v + 8) search area. h and v are the sizes of the         */
/*      search space for the top left coordinate of the search block.       */
/*      refImg points to the top left pixel of the search area.             */
/*                                                                          */
/*           (0,0)          (h,0)      (h+8,0)                              */
/*             ;--------------+--------;                                    */
/*             ;    search    |        ;                                    */
/*             ;    space     |        ;                                    */
/*             ;              |        ;        search area                 */
/*             ;--------------+        ;        within reference image      */
/*           (0,v)          (h,v)      ;                                    */
/*             ;                       ;                                    */
/*             ;-----------------------;                                    */
/*           (0, v+8)                 (v+8,h+8)                             */
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
/*  ASSUMPTIONS                                                             */
/*      srcImg and refImg do not alias in memory                            */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

void IMG_mad_8x8_c
(
    const unsigned char *restrict refImg,
    const unsigned char *restrict srcImg,
    unsigned int pitch,
    unsigned int h,
    unsigned int v,
    unsigned int *restrict match
)
{
    int    i,         j,        x,     y;
    int              matx,      maty;
    unsigned int     matpos,    matval;

    matval  =   ~0U;               /* init. set to MAXPOS     */
    matx = maty = 0;

    /*--------------------------------------------------------*/
    /* mh: horizontal loop counter 0 .. H-1                   */
    /* mv: vertical   loop counter 0... V-1                   */
    /*  i: row        loop counter 0... 7                     */
    /*  i2:column     loop counter 0... 7                     */
    /* Perform absolute differnece and accumulate. Compare    */
    /* with previous mad and store off if less                */
    /*--------------------------------------------------------*/

    #ifndef NOASSUME
    _nassert((int)srcImg % 4 == 0);   /* Uncommenting causes perf loss */
    _nassert(pitch >  8);
    _nassert(h     >= 4);
    _nassert(v     >= 4);
    _nassert(v % 2 == 0);
   #endif

    for (x = 0; x < h ; x++)
    {
        for (y = 0; y < v ; y++)
        {
            unsigned acc = 0;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                     acc += abs(srcImg[i*8 + j] - refImg[(i+y)*pitch + x + j]);
                }
            }

            if (acc < matval)
            {
                matval = acc;
                matx   = x;
                maty   = y;
            }
        }
    }

    matpos    = (0xffff0000 & (matx << 16)) | (0x0000ffff & maty);
    match[0] = matpos;
    match[1] = matval;
}

/* ======================================================================== */
/*  End of file:  img_mad_8x8.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
