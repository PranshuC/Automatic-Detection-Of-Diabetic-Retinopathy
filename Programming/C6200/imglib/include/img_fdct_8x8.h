/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.6     Thu Nov 14 21:39:22 2002 (UTC)              */
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
/*      IMG_fdct_8x8 -- 8x8 Block FDCT With Rounding, Endian Neutral        */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      28-Oct-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*          void IMG_fdct_8x8(short fdct_data[], unsigned num_fdcts)        */
/*                                                                          */
/*      The fdct routine accepts a list of 8x8 pixel blocks and performs    */
/*      FDCTs on each.  The array should be laid out identically to         */
/*      "fdct_data[num_fdcts][8][8]".  All operations in this array are     */
/*      performed entirely in-place.                                        */
/*                                                                          */
/*      Input values are stored in shorts, and may be in the range          */
/*      [-512,511].  Larger input values may result in overflow.            */
/*                                                                          */
/*      This code requires '48 + 160 * num_fdcts' cycles to process         */
/*      'num_fdcts' blocks, including function call overhead.  When         */
/*      'num_fdcts' is zero, an early exit is taken and the function        */
/*      runs for only 13 cycles (again, including call overhead).           */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The IMG_fdct_8x8 function implements a Chen FDCT. Output values are */
/*      rounded, providing improved accuracy.  Input terms are expected     */
/*      to be signed 11Q0 values, producing signed 15Q0 results.  (A        */
/*      smaller dynamic range may be used on the input, producing a         */
/*      correspondingly smaller output range.  Typical applications         */
/*      include processing signed 9Q0 and unsigned 8Q0 pixel data,          */
/*      producing signed 13Q0 or 12Q0 outputs, respectively.)  No           */
/*      saturation is performed.                                            */
/*                                                                          */
/*      void IMG_fdct_8x8(short *dct_data, unsigned num_fdcts)              */
/*      {                                                                   */
/*        // -------------------------------------------------------- //    */
/*        //  Set up the cosine coefficients c0..c7.                  //    */
/*        // -------------------------------------------------------- //    */
/*        const unsigned short c1 = 0x2C62, c3 = 0x25A0;                    */
/*        const unsigned short c5 = 0x1924, c7 = 0x08D4;                    */
/*        const unsigned short c0 = 0xB505, c2 = 0x29CF;                    */
/*        const unsigned short c6 = 0x1151;                                 */
/*                                                                          */
/*        // -------------------------------------------------------- //    */
/*        //  Intermediate calculations.                              //    */
/*        // -------------------------------------------------------- //    */
/*        short f0, f1, f2, f3,                                             */
/*              f4, f5, f6, f7;       // Spatial domain samples.      //    */
/*        int   g0, g1, h0, h1,                                             */
/*              p0, p1;               // Even-half intermediate.      //    */
/*        short r0, r1;               // Even-half intermediate.      //    */
/*        int   P0, P1, R0, R1;       // Even-half intermediate.      //    */
/*        short g2, g3, h2, h3;       // Odd-half intermediate.       //    */
/*        short q0a,s0a,q0, q1,                                             */
/*              s0, s1;               // Odd-half intermediate.       //    */
/*        short Q0, Q1, S0, S1;       // Odd-half intermediate.       //    */
/*        int   F0, F1, F2, F3,                                             */
/*              F4, F5, F6, F7;       // Freq. domain results.        //    */
/*        int   F0r,F1r,F2r,F3r,                                            */
/*              F4r,F5r,F6r,F7r;      // Rounded, truncated results.  //    */
/*                                                                          */
/*        // -------------------------------------------------------- //    */
/*        //  Input and output pointers, loop control.                //    */
/*        // -------------------------------------------------------- //    */
/*        unsigned i, j;                                                    */
/*        short    *dct_io_ptr;                                             */
/*                                                                          */
/*        // -------------------------------------------------------- //    */
/*        //  Outer vertical loop -- Process each 8x8 block.          //    */
/*        // -------------------------------------------------------- //    */
/*        dct_io_ptr = dct_data;                                            */
/*        for (i = 0; i < num_fdcts; i++)                                   */
/*        {                                                                 */
/*            // ---------------------------------------------------- //    */
/*            //  Perform Vert 1-D FDCT on columns within each block. //    */
/*            // ---------------------------------------------------- //    */
/*            for (j = 0; j < 8; j++)                                       */
/*            {                                                             */
/*                // ------------------------------------------------ //    */
/*                //  Load the spatial-domain samples.                //    */
/*                // ------------------------------------------------ //    */
/*                f0 = dct_io_ptr[ 0];                                      */
/*                f1 = dct_io_ptr[ 8];                                      */
/*                f2 = dct_io_ptr[16];                                      */
/*                f3 = dct_io_ptr[24];                                      */
/*                f4 = dct_io_ptr[32];                                      */
/*                f5 = dct_io_ptr[40];                                      */
/*                f6 = dct_io_ptr[48];                                      */
/*                f7 = dct_io_ptr[56];                                      */
/*                                                                          */
/*                // ------------------------------------------------ //    */
/*                //  Stage 1:  Separate into even and odd halves.    //    */
/*                // ------------------------------------------------ //    */
/*                g0 = f0 + f7;               h2 = f0 - f7;                 */
/*                g1 = f1 + f6;               h3 = f1 - f6;                 */
/*                h1 = f2 + f5;               g3 = f2 - f5;                 */
/*                h0 = f3 + f4;               g2 = f3 - f4;                 */
/*                                                                          */
/*                // ------------------------------------------------ //    */
/*                //  Stage 2                                         //    */
/*                // ------------------------------------------------ //    */
/*                p0 = g0 + h0;               r0 = g0 - h0;                 */
/*                p1 = g1 + h1;               r1 = g1 - h1;                 */
/*                q1 = g2;                    s1 = h2;                      */
/*                                                                          */
/*                s0a= h3 + g3;               q0a= h3 - g3;                 */
/*                s0 = (s0a * c0 + 0x7FFF) >> 16;                           */
/*                q0 = (q0a * c0 + 0x7FFF) >> 16;                           */
/*                                                                          */
/*                // ------------------------------------------------ //    */
/*                //  Stage 3                                         //    */
/*                // ------------------------------------------------ //    */
/*                P0 = p0 + p1;               P1 = p0 - p1;                 */
/*                R1 = c6 * r1 + c2 * r0;     R0 = c6 * r0 - c2 * r1;       */
/*                                                                          */
/*                Q1 = q1 + q0;               Q0 = q1 - q0;                 */
/*                S1 = s1 + s0;               S0 = s1 - s0;                 */
/*                                                                          */
/*                // ------------------------------------------------ //    */
/*                //  Stage 4                                         //    */
/*                // ------------------------------------------------ //    */
/*                F0 = P0;                    F4 = P1;                      */
/*                F2 = R1;                    F6 = R0;                      */
/*                                                                          */
/*                F1 = c7 * Q1 + c1 * S1;     F7 = c7 * S1 - c1 * Q1;       */
/*                F5 = c3 * Q0 + c5 * S0;     F3 = c3 * S0 - c5 * Q0;       */
/*                                                                          */
/*                // ------------------------------------------------ //    */
/*                //  Store the frequency domain results.             //    */
/*                // ------------------------------------------------ //    */
/*                dct_io_ptr[ 0] = F0;                                      */
/*                dct_io_ptr[ 8] = F1 >> 13;                                */
/*                dct_io_ptr[16] = F2 >> 13;                                */
/*                dct_io_ptr[24] = F3 >> 13;                                */
/*                dct_io_ptr[32] = F4;                                      */
/*                dct_io_ptr[40] = F5 >> 13;                                */
/*                dct_io_ptr[48] = F6 >> 13;                                */
/*                dct_io_ptr[56] = F7 >> 13;                                */
/*                                                                          */
/*                dct_io_ptr++;                                             */
/*            }                                                             */
/*            // ---------------------------------------------------- //    */
/*            //  Update pointer to next 8x8 FDCT block.              //    */
/*            // ---------------------------------------------------- //    */
/*            dct_io_ptr += 56;                                             */
/*        }                                                                 */
/*                                                                          */
/*        // -------------------------------------------------------- //    */
/*        //  Perform Horizontal 1-D FDCT on each 8x8 block.          //    */
/*        // -------------------------------------------------------- //    */
/*        dct_io_ptr = dct_data;                                            */
/*        for (i = 0; i < 8 * num_fdcts; i++)                               */
/*        {                                                                 */
/*            // ---------------------------------------------------- //    */
/*            //  Load the spatial-domain samples.                    //    */
/*            // ---------------------------------------------------- //    */
/*            f0 = dct_io_ptr[0];                                           */
/*            f1 = dct_io_ptr[1];                                           */
/*            f2 = dct_io_ptr[2];                                           */
/*            f3 = dct_io_ptr[3];                                           */
/*            f4 = dct_io_ptr[4];                                           */
/*            f5 = dct_io_ptr[5];                                           */
/*            f6 = dct_io_ptr[6];                                           */
/*            f7 = dct_io_ptr[7];                                           */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Stage 1:  Separate into even and odd halves.        //    */
/*            // ---------------------------------------------------- //    */
/*            g0 = f0 + f7;               h2 = f0 - f7;                     */
/*            g1 = f1 + f6;               h3 = f1 - f6;                     */
/*            h1 = f2 + f5;               g3 = f2 - f5;                     */
/*            h0 = f3 + f4;               g2 = f3 - f4;                     */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Stage 2                                             //    */
/*            // ---------------------------------------------------- //    */
/*            p0 = g0 + h0;               r0 = g0 - h0;                     */
/*            p1 = g1 + h1;               r1 = g1 - h1;                     */
/*            q1 = g2;                    s1 = h2;                          */
/*                                                                          */
/*            s0a= h3 + g3;               q0a= h3 - g3;                     */
/*            q0 = (q0a * c0 + 0x7FFF) >> 16;                               */
/*            s0 = (s0a * c0 + 0x7FFF) >> 16;                               */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Stage 3                                             //    */
/*            // ---------------------------------------------------- //    */
/*            P0 = p0 + p1;               P1 = p0 - p1;                     */
/*            R1 = c6 * r1 + c2 * r0;     R0 = c6 * r0 - c2 * r1;           */
/*                                                                          */
/*            Q1 = q1 + q0;               Q0 = q1 - q0;                     */
/*            S1 = s1 + s0;               S0 = s1 - s0;                     */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Stage 4                                             //    */
/*            // ---------------------------------------------------- //    */
/*            F0 = P0;                    F4 = P1;                          */
/*            F2 = R1;                    F6 = R0;                          */
/*                                                                          */
/*            F1 = c7 * Q1 + c1 * S1;     F7 = c7 * S1 - c1 * Q1;           */
/*            F5 = c3 * Q0 + c5 * S0;     F3 = c3 * S0 - c5 * Q0;           */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Round and truncate values.                          //    */
/*            //                                                      //    */
/*            //  Note: F0 and F4 have different rounding since no    //    */
/*            //  MPYs have been applied to either term.  Also, F0's  //    */
/*            //  rounding is slightly different to offset the        //    */
/*            //  truncation effects from the horizontal pass (which  //    */
/*            //  does not round).                                    //    */
/*            // ---------------------------------------------------- //    */
/*            F0r = (F0 + 0x0006) >>  3;                                    */
/*            F1r = (F1 + 0x7FFF) >> 16;                                    */
/*            F2r = (F2 + 0x7FFF) >> 16;                                    */
/*            F3r = (F3 + 0x7FFF) >> 16;                                    */
/*            F4r = (F4 + 0x0004) >>  3;                                    */
/*            F5r = (F5 + 0x7FFF) >> 16;                                    */
/*            F6r = (F6 + 0x7FFF) >> 16;                                    */
/*            F7r = (F7 + 0x7FFF) >> 16;                                    */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Store the results                                   //    */
/*            // ---------------------------------------------------- //    */
/*            dct_io_ptr[0] = F0r;                                          */
/*            dct_io_ptr[1] = F1r;                                          */
/*            dct_io_ptr[2] = F2r;                                          */
/*            dct_io_ptr[3] = F3r;                                          */
/*            dct_io_ptr[4] = F4r;                                          */
/*            dct_io_ptr[5] = F5r;                                          */
/*            dct_io_ptr[6] = F6r;                                          */
/*            dct_io_ptr[7] = F7r;                                          */
/*                                                                          */
/*            // ---------------------------------------------------- //    */
/*            //  Update pointer to next FDCT row.                    //    */
/*            // ---------------------------------------------------- //    */
/*            dct_io_ptr += 8;                                              */
/*        }                                                                 */
/*                                                                          */
/*        return;                                                           */
/*      }                                                                   */
/*                                                                          */
/*                                                                          */
/*      Note:  This code guarantees correct operation, even in the case     */
/*      that 'num_fdcts == 0'.  In this case, the function runs for only    */
/*      13 cycles (counting 6 cycles of function-call overhead), due to     */
/*      early-exit code.  The early-exit case performs no accesses to the   */
/*      fdct_data[] array and minimal access to the stack.                  */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The loop nest in the vertical pass has been collapsed into a        */
/*      single-level loop.  Both vertical and horizontal loops have         */
/*      been software pipelined.                                            */
/*                                                                          */
/*      For performance, portions of the code outside the loops have been   */
/*      inter-scheduled with the prolog and epilog code of the loops.       */
/*      Also, twin stack-pointers are used to accelerate stack accesses.    */
/*      Finally, pointer values and cosine term registers are reused        */
/*      between the horizontal and vertical loops to reduce the impact of   */
/*      pointer and constant reinitialization.                              */
/*                                                                          */
/*      To save codesize, prolog and epilog collapsing have been performed  */
/*      to the extent that it does not impact performance.  Also, code      */
/*      outside the loops has been scheduled to pack as tightly into        */
/*      fetch packets as possible to avoid alignment padding NOPs.          */
/*                                                                          */
/*      To reduce register pressure and save some code, the horizontal      */
/*      loop uses the same pair of pointer register for both reading and    */
/*      writing.  The pointer increments are on the LDs to permit prolog    */
/*      and epilog collapsing, since LDs can be speculated.                 */
/*                                                                          */
/*      Additional section-specific optimization notes are provided below.  */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      Stack is aligned to a word boundary.                                */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      No bank conflicts occur, regardless of fdct_data[]'s alignment.     */
/*                                                                          */
/*      The code requires 16 words of stack space to save Save-On-Entry     */
/*      (SOE) registers, CSR, IRP, and a spill value.                       */
/*                                                                          */
/*      Bank usage on C6201:  1 of 4 banks for 40% of loop cycles           */
/*                            2 of 4 banks for 60% of loop cycles           */
/*                                                                          */
/*      Nearly every cycle of this function performs at least one           */
/*      memory access.                                                      */
/*                                                                          */
/*      This code issues speculative loads beyond both ends of the          */
/*      input array.  Speculative loads may read up to 16 bytes before      */
/*      and 128 bytes after the input buffer.  The caller must ensure       */
/*      that the input is placed in memory so that these speculative        */
/*      accesses do not cause incorrect program behavior.                   */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code masks interrupts for nearly its entire duration.          */
/*      Interrupts are locked out for '40 + 160 * num_fdcts' cycles.  As    */
/*      a result, the code is interrupt-tolerant, but not interruptible.    */
/*                                                                          */
/*      The cosine terms have all been scaled by sqrt(2), so that the       */
/*      "c4" term is basically an even power of 2.                          */
/*                                                                          */
/*      The code is completely ENDIAN NEUTRAL.                              */
/*                                                                          */
/*  CYCLES                                                                  */
/*      cycles = 48 + 160 * num_fdcts                                       */
/*                                                                          */
/*      For num_fdcts =  6, cycles = 1008.                                  */
/*      For num_fdcts = 24, cycles = 3888.                                  */
/*                                                                          */
/*  CODESIZE                                                                */
/*      1216 bytes.                                                         */
/*                                                                          */
/*  SOURCE                                                                  */
/*      Chen FDCT.                                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_FDCT_8X8_H_
#define IMG_FDCT_8X8_H_ 1

void IMG_fdct_8x8(short fdct_data[], unsigned num_fdcts);

#endif
/* ======================================================================== */
/*  End of file:  img_fdct_8x8.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
