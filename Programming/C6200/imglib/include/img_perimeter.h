/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.10    Sun Sep 29 03:31:27 2002 (UTC)              */
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
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      Perimeter  detection of a boundary image                            */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      06-Oct-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*      int IMG_perimeter(unsigned char *in_data, int  cols, unsigned char  */
/*      *out_data );                                                        */
/*                                                                          */
/*      in_data  : Input binary image                                       */
/*      cols     : Number of cols. Must be >= 3.                            */
/*      out_data : Output boundary image                                    */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This routine produces the IMG_perimeter of a binary image, by       */
/*      counting the number of boundary pixels of the input image.  It      */
/*      echoes the boundary pixels with a value of 0xFF and sets the        */
/*      other pixels as 0.  Detection of the boundary of a binary image     */
/*      is a segmentation problem and is done by examining spatial          */
/*      locality of the neighboring pixels.  This is done by using the      */
/*      four connectivity algorithm:                                        */
/*                                                                          */
/*                       pix_up                                             */
/*              pix_lft pix_cent pix_rgt                                    */
/*                       pix_dn                                             */
/*                                                                          */
/*      The output pixel at location pix_cent is echoed as a boundary       */
/*      pixel if pix_cent is non-zero and any one of its four neighbors     */
/*      is zero.  The four neighbors are shown and stand for the            */
/*      following:                                                          */
/*                                                                          */
/*              pix_up:  top pixel                                          */
/*              pix_lft: left pixel                                         */
/*              pix_rgt: right pixel                                        */
/*              pix_dn:  bottom pixel                                       */
/*                                                                          */
/*   CODE                                                                   */
/*      This is the C equivalent of the assembly code without               */
/*      restrictions: Note that the assembly code is hand optimized and     */
/*      restrictions may apply.                                             */
/*                                                                          */
/*      int IMG_perimeter(unsigned char *in_data, int  cols, unsigned char  */
/*      *out_data)                                                          */
/*      {                                                                   */
/*          int icols;                                                      */
/*          int count;                                                      */
/*                                                                          */
/*          unsigned char pix_lft, pix_rgt, pix_top;                        */
/*          unsigned char pix_bot, pix_cent;                                */
/*                                                                          */
/*          count = 0;                                                      */
/*                                                                          */
/*          for(icols = 1; icols < (cols-1); icols++ )                      */
/*          {                                                               */
/*              pix_lft = in_data[icols - 1];                               */
/*              pix_cent= in_data[icols + 0];                               */
/*              pix_rgt = in_data[icols + 1];                               */
/*                                                                          */
/*              pix_top = in_data[icols - cols];                            */
/*              pix_bot = in_data[icols + cols];                            */
/*                                                                          */
/*              if ( ( (pix_lft == 0) ||                                    */
/*                     (pix_rgt == 0) ||                                    */
/*                     (pix_top == 0) ||                                    */
/*                     (pix_bot == 0) ) && (pix_cent > 0) )                 */
/*              {                                                           */
/*                  out_data[icols] = pix_cent;                             */
/*                  count++;                                                */
/*              }                                                           */
/*              else                                                        */
/*              {                                                           */
/*                  out_data[icols] = 0;                                    */
/*              }                                                           */
/*          }                                                               */
/*                                                                          */
/*          return(count);                                                  */
/*      }                                                                   */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      No specific alignment is expected for the input or output array     */
/*      cols can be either even or odd.                                     */
/*                                                                          */
/*      This code expects three input lines each of cols pixels and         */
/*      produces one output line of cols - 1 pixels.                        */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts are expected for this kernel.  In addition        */
/*      notice that the # of loads that have be performed is reduced        */
/*      because of data re-use within the loop. This is also beneficial     */
/*      because bank conflicts can be eliminated Notice that pix_cent of    */
/*      the present iteration is the pix_lft of the next iteration. Hence   */
/*      these can be obtained by moves rather than fresh loads. The set of  */
/*      moves are indicated below                                           */
/*                                                                          */
/*      pix_cent ---> pix_lft                                               */
/*      pix_rgt  ---> pix_cent                                              */
/*                                                                          */
/*      In order for this to be done at the start of the kernel the values  */
/*      of pix_cent and pix_rgt are pre-loaded before the start of the      */
/*      loop                                                                */
/*                                                                          */
/*      This code is ENDIAN NEUTRAL                                         */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code masks interrupts for nearly its entire duration. As a     */
/*      result, the code is interrupt_tolerant but not interruptible        */
/*                                                                          */
/*  CYCLES                                                                  */
/*      3 * (cols - 2) + 28                                                 */
/*                                                                          */
/*      for cols = 720, cycles = 2182                                       */
/*      for cols = 200, cycles = 622                                        */
/*                                                                          */
/*  CODESIZE                                                                */
/*      352 bytes                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_PERIMETER_H_
#define IMG_PERIMETER_H_ 1

int IMG_perimeter(unsigned char *in_data, int  cols, unsigned char
*out_data );

#endif
/* ======================================================================== */
/*  End of file:  img_perimeter.h                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
