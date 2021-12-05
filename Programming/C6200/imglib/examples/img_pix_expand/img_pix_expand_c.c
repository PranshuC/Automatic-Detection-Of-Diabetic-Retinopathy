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


/* ========================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                   */
/*                                                                            */
/*  NAME                                                                      */
/*      IMG_pix_expand                                                        */
/*                                                                            */
/*  REVISION DATE                                                             */
/*      06-Nov-1999                                                           */
/*                                                                            */
/*  USAGE                                                                     */
/*      This routine is C-callable and can be called as:                      */
/*                                                                            */
/*      void IMG_pix_expand(int cols,unsigned char *in_data,short *out_data) ; */
/*                                                                            */
/*          cols = length of array                                            */
/*          in_data = pointer to input array of unsigned chars                */
/*          out_data = pointer to output array of shorts                      */
/*                                                                            */
/*                                                                            */
/*  DESCRIPTION                                                               */
/*      The code takes an array of unsigned chars (pixels) and zero           */
/*      extends them up to 16 bits to form shorts                             */
/*                                                                            */
/*  ASSUMPTIONS                                                               */
/*      This code is endian neutral.                                          */
/*      Input data is assumed to be unsigned                                  */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ========================================================================== */

void IMG_pix_expand_c(int cols, const unsigned char *in_data, short *out_data)
{
    int j;

    for (j = 0; j < cols; j++)
    {
        out_data[j] = (short) in_data[j];
    }
}

/* ========================================================================== */
/*  End of file:  img_pix_expand.c                                            */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ========================================================================== */
