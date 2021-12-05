/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.6     Sat Mar 16 04:03:27 2002 (UTC)              */
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
/*      IMG_yc_demux_le16 -- De-interleave a 4:2:2 LITTLE ENDIAN video stream */
/*                       into three separate LITTLE ENDIAN 16-bit planes    */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      15-Mar-2002                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This function is C callable, and is called as follows:              */
/*                                                                          */
/*      void IMG_yc_demux_le16                                              */
/*      (                                                                   */
/*          int n,                       // Number of luma pixels    //     */
/*          const unsigned char * yc,    // Interleaved luma/chroma  //     */
/*          short *restrict y,           // Luma plane (16-bit)      //     */
/*          short *restrict cr,          // Cr chroma plane (16-bit) //     */
/*          short *restrict cb           // Cb chroma plane (16-bit) //     */
/*      );                                                                  */
/*                                                                          */
/*      The input array 'yc' is expected to be an interleaved 4:2:2         */
/*      video stream.  The input is expected in LITTLE ENDIAN byte          */
/*      order within each 4-byte word.  This is consistent with reading     */
/*      the video stream from a word-oriented LITTLE ENDIAN device          */
/*      while the C6000 device is in a LITTLE ENDIAN configuration.         */
/*                                                                          */
/*      In other words, the expected pixel order is:                        */
/*                                                                          */
/*                  Word 0           Word 1          Word 2                 */
/*             +---------------+---------------+---------------+--          */
/*       Byte# | 0   1   2   3 | 4   5   6   7 | 8   9  10  11 |...         */
/*             | y0 cr0 y1 cb0 | y2 cr2 y3 cb2 | y4 cr4 y5 cb4 |...         */
/*             +---------------+---------------+---------------+--          */
/*                                                                          */
/*      The output arrays 'y', 'cr', and 'cb' are expected to not           */
/*      overlap.  The de-interleaved pixels are written as half-words       */
/*      in LITTLE ENDIAN order.                                             */
/*                                                                          */
/*      Please see the IMGLIB function IMB_yc_demux_be16 for code which     */
/*      handles input coming from a BIG ENDIAN device.                      */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This function reads the byte-oriented pixel data, zero-extends      */
/*      it, and then writes it to the appropriate result array.  Both       */
/*      the luma and chroma values are expected to be unsigned.             */
/*                                                                          */
/*      The data is expected to be in an order consistent with reading      */
/*      byte oriented data from a word-oriented peripheral that is          */
/*      operating in LITTLE ENDIAN mode, while the CPU is in LITTLE         */
/*      ENDIAN mode.  This function unpacks the byte-oriented data          */
/*      so that further processing may proceed in LITTLE ENDIAN mode.       */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      Input and output arrays are word aligned.                           */
/*      The input must be a multiple of 8 luma pixels long.                 */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The loop has been unrolled a total of 8 times to allow for          */
/*      processing 4 pixels in each datapath.                               */
/*                                                                          */
/*      Word-wide loads and stores maximize memory bandwidth                */
/*      utilization.                                                        */
/*                                                                          */
/*      The 40-bit shifter is used to exchange the luma bytes within        */
/*      each word, effectively giving leftward byte rotate.                 */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      C6211/C6711 Two Level Cache Considerations:                         */
/*                                                                          */
/*      In a C6211-style system, this routine will experience cache-        */
/*      related stalls, particularly if the incoming data is not present    */
/*      in L1D, as is typical when the inputs are provided via DMA.         */
/*                                                                          */
/*      [Note:  The following analysis applies primarily to the C6211       */
/*      and C6711.  It applies only in a general sense to the               */
/*      cache-based C64x DSPs, and so may not be accurate in that           */
/*      context.  Notably, the cache parameters on C64x differ from         */
/*      C6211 and C6711.  Please consult the C6000 Peripherals              */
/*      Reference Guide, SPRU190, for more information on the memory        */
/*      system.]                                                            */
/*                                                                          */
/*      There are two primary sources of L1D cache degradation in this      */
/*      code:  Read misses on the incoming YC data, and write misses on     */
/*      the outgoing Y, Cb, and Cr data.                                    */
/*                                                                          */
/*      Read misses cost 5 cycles apiece.  In the worst case (no input      */
/*      data in L1D), this results in n/16 cache misses, for a total        */
/*      of 5*n/16 stall cycles.  There is little that can be done to        */
/*      hide these.                                                         */
/*                                                                          */
/*      Write misses do not automatically stall the CPU, but rather         */
/*      are placed in a 4-entry write buffer, where the writes are          */
/*      subsequently processed by L2.  The L2 processes these at a rate     */
/*      of 1 word every 2 cycles.  Therefore, if none of the output         */
/*      arrays for this loop are in L1D, we are limited to one iteration    */
/*      every 8 cycles -- that is, 5 cycles stall per iteration, for a      */
/*      total of 5*n/4 stall cycles.  If the "y" buffer is brought into     */
/*      L1D prior to execution, this overhead drops to 1 stall per          */
/*      iteration.  If "y" and one of "Cr" or "Cb" are brought in, the      */
/*      overhead due to the store buffer alone drops to zero.               */
/*                                                                          */
/*      Read misses and write misses interact when a write miss is          */
/*      pending in the write buffer and a read miss occurs.  In the         */
/*      cases where both "Cr" and "Cb" are not in L1D (and thus the         */
/*      write buffer is saturated), one can show that this interaction      */
/*      does not change the overall throughput.  In the case where          */
/*      "y" and only one of "Cr" or "Cb" is in L1D, the write buffer        */
/*      will have one element present in it when read misses occur,         */
/*      and will, on average, lengthen read misses by 1 cycle.              */
/*                                                                          */
/*      For this hand-assembly version, it is safe to overlap the           */
/*      output "y" buffer with the input "yc" buffer as long as the two     */
/*      buffers start at the same location.  This effectively ensures       */
/*      that the stores to the "y" array hit in L1D, and so do not use      */
/*      the write buffer.  By further "touching" either "Cb" or "Cr"        */
/*      into L1D, the penalty associated with the write buffer can be       */
/*      reduced to zero, as discussed above.                                */
/*                                                                          */
/*      C620x/C670x Flat Memory Considerations:                             */
/*                                                                          */
/*      In the code below, accesses to the Cb and Cr arrays are allowed     */
/*      to parallelize.  In order to avoid bank conflicts, one must         */
/*      offset the Cb and Cr arrays by one word (two banks), or place       */
/*      them in independent memory blocks.                                  */
/*                                                                          */
/*      This code accesses all four memory banks every cycle.  If both      */
/*      input and output buffers are in the same memory block, then any     */
/*      any DMAs to that block will either stall until this code            */
/*      completes, or will force the CPU to stall unconditionally.          */
/*      To allow DMAs to occur with minimal impact to the CPU and vice      */
/*      versa, do one of the following:                                     */
/*                                                                          */
/*       -- Ensure that your DMAs go to a different memory block            */
/*          so that DMA and CPU run independently, or                       */
/*                                                                          */
/*       -- Set the DMA to lower priority than the CPU, and place your      */
/*          input and output buffers in different memory blocks, so         */
/*          that the DMA runs during gaps in this code's access pattern.    */
/*                                                                          */
/*      The code spends 1/3rd of its cycles reading from the input,         */
/*      1/3rd writing to 'y', and 1/3rd writing to 'Cb' and 'Cr' in         */
/*      parallel.                                                           */
/*                                                                          */
/*      Please consult the C6000 Peripherals Reference Guide, SPRU190,      */
/*      for additional information regarding memory system interactions.    */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code is interrupt tolerant, but not interruptible.             */
/*      Interrupts are masked for nearly its entire duration by branch      */
/*      delay slots.                                                        */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 3 * (n / 4) + 18.                                          */
/*      For n = 1024, cycles = 786.                                         */
/*                                                                          */
/*      This number includes 6 cycles of function call overhead.  The       */
/*      exact overhead will vary depending on compiler options used.        */
/*                                                                          */
/*  CODESIZE                                                                */
/*      224 bytes.                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_YC_DEMUX_LE16_H_
#define IMG_YC_DEMUX_LE16_H_ 1

void IMG_yc_demux_le16
(
    int n,                       // Number of luma pixels    //
    const unsigned char * yc,    // Interleaved luma/chroma  //
    short *restrict y,           // Luma plane (16-bit)      //
    short *restrict cr,          // Cr chroma plane (16-bit) //
    short *restrict cb           // Cb chroma plane (16-bit) //
);

#endif
/* ======================================================================== */
/*  End of file:  img_yc_demux_le16.h                                       */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
