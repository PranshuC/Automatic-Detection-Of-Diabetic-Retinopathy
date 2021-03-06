/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.13    Thu Nov 14 21:39:50 2002 (UTC)              */
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
/*  NAME                                                                    */
/*      IMG_idct_8x8 -- IEEE-1180 Compliant IDCT, Little Endian.            */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      27-Jul-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*          void IMG_idct_8x8(short idct_data[], unsigned num_idcts)        */
/*                                                                          */
/*      The IMG_idct_8x8 routine accepts a list of 8x8 DCT coeffient blocks */
/*      and performs IDCTs on each.  The array should be aligned to a       */
/*      32-bit boundary, and be laid out equivalently to the C array        */
/*      idct_data[num_idcts+1][8][8].                                       */
/*                                                                          */
/*      The routine requires one 8x8-block's worth of extra storage at      */
/*      the end of the list of DCT blocks.  The caller must provide room    */
/*      for 'num_idcts + 1' blocks of data in the idct_data[] array.        */
/*      The contents of the extra block are ignored and overwritten         */
/*      with intermediate results by IMG_idct_8x8().                        */
/*                                                                          */
/*      This code requires '62 + 168 * num_idcts' cycles to process         */
/*      'num_idcts' blocks, including 6 cycles of function call overhead.   */
/*      When 'num_idcts' is zero, the function takes an early exit and      */
/*      runs for only 35 cycles (again, including overhead).                */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The IMG_idct_8x8() algorithm performs an IEEE-1180 compliant IDCT,  */
/*      complete with rounding and saturation to signed 9-bit quantities.   */
/*      The input coefficients are assumed to be signed 12-bit cosine       */
/*      terms.                                                              */
/*                                                                          */
/*      void IMG_idct_8x8(short *idct_data, unsigned num_dcts)              */
/*      {                                                                   */
/*        const short c1 = 0x0B19, c2 = 0x0A74, c3 = 0x0968;                */
/*        const short c5 = 0x0649, c6 = 0x0454, c7 = 0x0235;                */
/*        const int   c4_shift = 11;                                        */
/*        const int   round1 = 256, round2 = 32768;                         */
/*        const int   trunc1 = 9, trunc2 = 16;                              */
/*        const short *i_ptr;                                               */
/*        short       *o_ptr;                                               */
/*        unsigned    i, j;                                                 */
/*        short X0, X1, X2, X3, X4, X5, X6, X7;   // Freq domain terms  //  */
/*        int   P0, P1, p0, p1, r0, r1;           // Even-half temp     //  */
/*        int   g0, g1, h1, h0;                   // Even-half result   //  */
/*        int   g2, g3, h3, h2;                   // Odd-half result    //  */
/*        int   x0, x1, x2, x3, x4, x5, x6, x7;   // Resulting samples  //  */
/*        int   x0t,x1t,x2t,x3t,x4t,x5t,x6t,x7t;  // Truncated result   //  */
/*        int   x0s,x1s,x2s,x3s,x4s,x5s,x6s,x7s;  // Saturated result   //  */
/*                                                                          */
/*        // ---------------------------------------------------------- //  */
/*        //  Avoid running the code if we don't have any IDCTs to do.  //  */
/*        // ---------------------------------------------------------- //  */
/*        if (!num_dcts) return;                                            */
/*                                                                          */
/*        // ---------------------------------------------------------- //  */
/*        //  Set up pointers.                                          //  */
/*        // ---------------------------------------------------------- //  */
/*        i_ptr = idct_data + num_dcts * 64 - 8;                            */
/*        o_ptr = idct_data + num_dcts * 64 + 7;                            */
/*                                                                          */
/*        for (j = 0; j < num_dcts; j++)                                    */
/*        {                                                                 */
/*          // -------------------------------------------------------- //  */
/*          //  Perform Horizontal 1-D IDCT on each 8x8 block.  Store   //  */
/*          //  out the results transposed.                             //  */
/*          // -------------------------------------------------------- //  */
/*          for (i = 0; i < 8; i++)                                         */
/*          {                                                               */
/*              // ---------------------------------------------------- //  */
/*              //  Load the freq-domain coefficients.                  //  */
/*              // ---------------------------------------------------- //  */
/*              X0 = i_ptr[0];                                              */
/*              X1 = i_ptr[1];                                              */
/*              X2 = i_ptr[2];                                              */
/*              X3 = i_ptr[3];                                              */
/*              X4 = i_ptr[4];                                              */
/*              X5 = i_ptr[5];                                              */
/*              X6 = i_ptr[6];                                              */
/*              X7 = i_ptr[7];                                              */
/*                                                                          */
/*              i_ptr -= 8;             // decr pointer to next row     //  */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Even part of decomp.  Add rounding to DC term.      //  */
/*              // ---------------------------------------------------- //  */
/*              P0 = (((int)X0) << c4_shift) + round1;                      */
/*              P1 = (((int)X4) << c4_shift);                               */
/*                                                                          */
/*              p0 = P0 + P1;                                               */
/*              p1 = P0 - P1;                                               */
/*                                                                          */
/*              r1 = X2*c6 - X6*c2;                                         */
/*              r0 = X2*c2 + X6*c6;                                         */
/*                                                                          */
/*              g0 = p0 + r0;                                               */
/*              g1 = p1 + r1;                                               */
/*              h1 = p1 - r1;                                               */
/*              h0 = p0 - r0;                                               */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Odd part of decomp.                                 //  */
/*              // ---------------------------------------------------- //  */
/*              g2 = (X1*c7 - X3*c5) + (X5*c3 - X7*c1);                     */
/*              g3 = (X1*c5 - X3*c1) + (X5*c7 + X7*c3);                     */
/*              h3 = (X1*c3 - X3*c7) - (X5*c1 + X7*c5);                     */
/*              h2 = (X1*c1 + X3*c3) + (X5*c5 + X7*c7);                     */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Final butterfly.                                    //  */
/*              // ---------------------------------------------------- //  */
/*              x0 = g0 + h2;                                               */
/*              x1 = g1 + h3;                                               */
/*              x2 = h1 + g3;                                               */
/*              x3 = h0 + g2;                                               */
/*              x4 = h0 - g2;                                               */
/*              x5 = h1 - g3;                                               */
/*              x6 = g1 - h3;                                               */
/*              x7 = g0 - h2;                                               */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Truncate to fit back into 16 bits.                  //  */
/*              // ---------------------------------------------------- //  */
/*              x0t = x0 >> trunc1;                                         */
/*              x1t = x1 >> trunc1;                                         */
/*              x2t = x2 >> trunc1;                                         */
/*              x3t = x3 >> trunc1;                                         */
/*              x4t = x4 >> trunc1;                                         */
/*              x5t = x5 >> trunc1;                                         */
/*              x6t = x6 >> trunc1;                                         */
/*              x7t = x7 >> trunc1;                                         */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Store the results transposed.                       //  */
/*              // ---------------------------------------------------- //  */
/*              o_ptr[ 0] = x0t;                                            */
/*              o_ptr[ 8] = x1t;                                            */
/*              o_ptr[16] = x2t;                                            */
/*              o_ptr[24] = x3t;                                            */
/*              o_ptr[32] = x4t;                                            */
/*              o_ptr[40] = x5t;                                            */
/*              o_ptr[48] = x6t;                                            */
/*              o_ptr[56] = x7t;                                            */
/*                                                                          */
/*              o_ptr--;                // decrement ptr to next column //  */
/*          }                                                               */
/*                                                                          */
/*          // -------------------------------------------------------- //  */
/*          //  Update output pointer to point to next block.           //  */
/*          // -------------------------------------------------------- //  */
/*                                                                          */
/*          o_ptr = o_ptr + 8 - 64;                                         */
/*        }                                                                 */
/*                                                                          */
/*        // ---------------------------------------------------------- //  */
/*        //  Reset our pointers for the vertical pass.                 //  */
/*        // ---------------------------------------------------------- //  */
/*        i_ptr = idct_data + 64;                                           */
/*        o_ptr = idct_data;                                                */
/*                                                                          */
/*        for (j = 0; j < num_dcts; j++)                                    */
/*        {                                                                 */
/*          // -------------------------------------------------------- //  */
/*          //  Perform Vertical 1-D IDCT on each 8x8 block.  Store     //  */
/*          //  out the results transposed.                             //  */
/*          // -------------------------------------------------------- //  */
/*          for (i = 0; i < 8; i++)                                         */
/*          {                                                               */
/*              // ---------------------------------------------------- //  */
/*              //  Load the freq-domain coefficients.                  //  */
/*              // ---------------------------------------------------- //  */
/*              X0 = i_ptr[0];                                              */
/*              X1 = i_ptr[1];                                              */
/*              X2 = i_ptr[2];                                              */
/*              X3 = i_ptr[3];                                              */
/*              X4 = i_ptr[4];                                              */
/*              X5 = i_ptr[5];                                              */
/*              X6 = i_ptr[6];                                              */
/*              X7 = i_ptr[7];                                              */
/*              i_ptr += 8;             // increment ptr to next row    //  */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Even part of decomp.  Add rounding term to DC.      //  */
/*              // ---------------------------------------------------- //  */
/*              P0 = (((int)X0) << c4_shift) + round2; // c4 is a shift //  */
/*              P1 = (((int)X4) << c4_shift);          // c4 is a shift //  */
/*                                                                          */
/*              p0 = P0 + P1;                                               */
/*              p1 = P0 - P1;                                               */
/*                                                                          */
/*              r1 = X2*c6 - X6*c2;                                         */
/*              r0 = X2*c2 + X6*c6;                                         */
/*                                                                          */
/*              g0 = p0 + r0;                                               */
/*              g1 = p1 + r1;                                               */
/*              h1 = p1 - r1;                                               */
/*              h0 = p0 - r0;                                               */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Odd part of decomp.                                 //  */
/*              // ---------------------------------------------------- //  */
/*              g2 = (X1*c7 - X3*c5) + (X5*c3 - X7*c1);                     */
/*              g3 = (X1*c5 - X3*c1) + (X5*c7 + X7*c3);                     */
/*              h3 = (X1*c3 - X3*c7) - (X5*c1 + X7*c5);                     */
/*              h2 = (X1*c1 + X3*c3) + (X5*c5 + X7*c7);                     */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Final butterfly.                                    //  */
/*              // ---------------------------------------------------- //  */
/*              x0 = g0 + h2;                                               */
/*              x1 = g1 + h3;                                               */
/*              x2 = h1 + g3;                                               */
/*              x3 = h0 + g2;                                               */
/*              x4 = h0 - g2;                                               */
/*              x5 = h1 - g3;                                               */
/*              x6 = g1 - h3;                                               */
/*              x7 = g0 - h2;                                               */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Truncate and saturate final results.                //  */
/*              // ---------------------------------------------------- //  */
/*              x0t = x0 >> trunc2;                                         */
/*              x1t = x1 >> trunc2;                                         */
/*              x2t = x2 >> trunc2;                                         */
/*              x3t = x3 >> trunc2;                                         */
/*              x4t = x4 >> trunc2;                                         */
/*              x5t = x5 >> trunc2;                                         */
/*              x6t = x6 >> trunc2;                                         */
/*              x7t = x7 >> trunc2;                                         */
/*                                                                          */
/*              x0s = x0t < -256 ? -256 : x0t > 255 ? 255 : x0t;            */
/*              x1s = x1t < -256 ? -256 : x1t > 255 ? 255 : x1t;            */
/*              x2s = x2t < -256 ? -256 : x2t > 255 ? 255 : x2t;            */
/*              x3s = x3t < -256 ? -256 : x3t > 255 ? 255 : x3t;            */
/*              x4s = x4t < -256 ? -256 : x4t > 255 ? 255 : x4t;            */
/*              x5s = x5t < -256 ? -256 : x5t > 255 ? 255 : x5t;            */
/*              x6s = x6t < -256 ? -256 : x6t > 255 ? 255 : x6t;            */
/*              x7s = x7t < -256 ? -256 : x7t > 255 ? 255 : x7t;            */
/*                                                                          */
/*              // ---------------------------------------------------- //  */
/*              //  Store the results transposed in the result area.    //  */
/*              // ---------------------------------------------------- //  */
/*              o_ptr[ 0] = x0s;                                            */
/*              o_ptr[ 8] = x1s;                                            */
/*              o_ptr[16] = x2s;                                            */
/*              o_ptr[24] = x3s;                                            */
/*              o_ptr[32] = x4s;                                            */
/*              o_ptr[40] = x5s;                                            */
/*              o_ptr[48] = x6s;                                            */
/*              o_ptr[56] = x7s;                                            */
/*                                                                          */
/*              o_ptr++;                // increment ptr to next column //  */
/*          }                                                               */
/*          // -------------------------------------------------------- //  */
/*          //  Update output pointer to point to next block.           //  */
/*          // -------------------------------------------------------- //  */
/*          o_ptr = o_ptr - 8 + 64;                                         */
/*        }                                                                 */
/*      }                                                                   */
/*                                                                          */
/*                                                                          */
/*      Note:  This code guarantees correct operation, even in the case     */
/*      that 'num_idcts == 0'.  In that case, the function runs for only    */
/*      35 cycles (counting 6 cycles of function-call overhead), due to     */
/*      early-exit code.  The early-exit case performs no accesses to the   */
/*      idct_data[] array.                                                  */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      All levels of looping are collapsed into single loops which are     */
/*      pipelined.  The outer loop focuses on 8-pt IDCTs, whereas the       */
/*      inner loop controls the column-pointer to handle jumps between      */
/*      IDCT blocks.                                                        */
/*                                                                          */
/*      For performance, portions of the code outside the loops have been   */
/*      inter-scheduled with the prolog and epilog code of the loops.       */
/*      Also, twin stack-pointers are used to accelerate stack accesses.    */
/*      Finally, pointer values and cosine term registers are reused        */
/*      between the horizontal and vertical loops to save the need for      */
/*      messy pointer and constant reinitialization.                        */
/*                                                                          */
/*      To save codesize, prolog and epilog collapsing have been performed  */
/*      to the extent that it does not impact performance.  Also, code      */
/*      outside the loops has been scheduled to pack as tightly into        */
/*      fetch packets as possible to avoid alignment padding NOPs.          */
/*                                                                          */
/*      The IDCTs cannot be performed completely in-place due to the        */
/*      transpose that each pass performs.  In order to save data memory,   */
/*      the horizontal pass works from the end of the array towards the     */
/*      begining, writing its result one IDCT block later in memory,        */
/*      thus performing the IDCT nearly-in-place.  The vertical pass        */
/*      performs its IDCTs in the opposite direction, working from the      */
/*      start of the array towards the end, writing the results in-place.   */
/*      A nice side effect of this is that the pointer values at the        */
/*      end of the horizontal loop are a fixed offset relative to their     */
/*      required values for the vertical loop, regardless of the number     */
/*      of IDCTs performed.  This makes the pointer reinitialization        */
/*      exceptionally cheap.                                                */
/*                                                                          */
/*      Additional section-specific optimization notes are provided below.  */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The input array must be aligned on a word boundary, and one         */
/*      extra block's worth of storage must be present after the list       */
/*      of IDCT input blocks.                                               */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts occur.  The code requires 16 words of stack       */
/*      space to save Save-On-Entry (SOE) registers, CSR, IRP, and a        */
/*      spill value.  For correct operation, the input array must be        */
/*      aligned to a word boundary.                                         */
/*                                                                          */
/*      Bank usage on C6201:                                                */
/*                                                                          */
/*          Horiz loop accesses: 1 of 4 banks for 80% of cycles             */
/*                               4 of 4 banks for 20% of cycles             */
/*                                                                          */
/*          Vert loop accesses:  1 of 4 banks for 73% of cycles             */
/*                               4 of 4 banks for 18% of cycles             */
/*                               0 of 4 banks for  9% of cycles             */
/*                                                                          */
/*      This code issues speculative loads beyond both ends of the          */
/*      input array.  This is in addition to the 1 block of scratch         */
/*      storage that the code requires.  Speculative loads may read         */
/*      up to 32 bytes before and 48 bytes after the input buffer.          */
/*      The caller must ensure that the input is placed in memory so        */
/*      that these speculative accesses do not cause incorrect program      */
/*      behavior.                                                           */
/*                                                                          */
/*  NOTES                                                                   */
/*      This is a LITTLE ENDIAN implementation.                             */
/*                                                                          */
/*      This code masks interrupts for nearly its entire duration.          */
/*      Interrupts are locked out for '53 + 168 * num_idcts' cycles.  As    */
/*      a result, the code is interrupt-tolerant, but not interruptible.    */
/*                                                                          */
/*      The cosine terms have all been scaled by sqrt(2), so that the       */
/*      "c4" term is basically an even power of 2.                          */
/*                                                                          */
/*      The precision of the final results can be changed by modifying      */
/*      the constants at the top of the code and reassembling.  Usually,    */
/*      modifying the final-shift constants in the "Symbolic Constants"     */
/*      section is sufficient.                                              */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 62 + 168 * num_idcts  (or 35 cycles if num_idcts == 0)     */
/*                                                                          */
/*      For num_idcts == 6,   cycles = 1070.                                */
/*      For num_idcts == 24,  cycles = 4094.                                */
/*                                                                          */
/*  CODESIZE                                                                */
/*      1344 bytes                                                          */
/*                                                                          */
/*  SOURCE                                                                  */
/*      Even-Odd Decomposition IDCT.                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_IDCT_8X8_H_
#define IMG_IDCT_8X8_H_ 1

void IMG_idct_8x8(short idct_data[], unsigned num_idcts);

#endif
/* ======================================================================== */
/*  End of file:  img_idct_8x8.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
