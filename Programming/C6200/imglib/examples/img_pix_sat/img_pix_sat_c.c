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
/*      IMG_pix_sat                                                        */
/*                                                                         */
/*                                                                         */
/*  REVISION DATE                                                          */
/*      10-Apr-1998                                                        */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine is C callable, and has the following C prototype:     */
/*                                                                         */
/*      void IMG_pix_sat(int n, short *in_data, unsigned char *out_data);  */
/*                                                                         */
/*      n        : Length of input array                                   */
/*      in_data  : Pointer to array of signed 16 bit numbers               */
/*      out_data : Pointer to array of clipped, unsigned 8 bit numbers.    */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      This code performs the saturation of 16 bit signed numbers to 8    */
/*      bit unsigned numbers. If the data is over 255 it is saturated to   */
/*      255, if it is less than 0 it is saturated to 0.                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================= */

void IMG_pix_sat_c(int n, short *in_data, unsigned char * out_data)
{
    int j;
    unsigned char pel;
    short pixel;

    for (j = 0; j < n; j++)
    {
        pixel = in_data[j];
        pel = (unsigned char) pixel;
        if (pixel > 0xff) pel = 0xff;
        if (pixel < 0x00) pel = 0x00;
        out_data[j] = pel;
    }
}
/*============================================================================*/
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/*============================================================================*/
