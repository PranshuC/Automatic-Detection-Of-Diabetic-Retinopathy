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
/*  NAME                                                                    */
/*      IMG_idct_8x8 -- IEEE-1180 Compliant IDCT                            */
/*                                                                          */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      27-Jul-1999                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*          void IMG_idct_8x8(short idct_data[], unsigned num_idcts)        */
/*                                                                          */
/*      The IMG_idct_8x8 routine accepts a list of 8x8 DCT coeffient blocks */
/*      and performs IDCTs on each.  The array should be laid out           */
/*      equivalently to the C array idct_data[num_idcts+1][8][8].           */
/*                                                                          */
/*      The routine requires one 8x8-block's worth of extra storage at      */
/*      the end of the list of DCT blocks.  When processing 'num_idcts'     */
/*      blocks, an area of length 'num_idcts + 1' must be provided.  The    */
/*      contents of the extra block are ignored and overwritten with        */
/*      intermediate results by IMG_idct_8x8().                             */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The IMG_idct_8x8 algorithm performs an IEEE-1180 compliant IDCT,    */
/*      complete with rounding and saturation to signed 9-bit quantities.   */
/*      The input coefficients are assumed to be signed 12-bit cosine       */
/*      terms.                                                              */
/*                                                                          */
/*      Note:  This code guarantees correct operation, even in the case     */
/*      that 'num_idcts == 0'.  In that case, the function performs an      */
/*      early exit without storing any results or intermediate values.      */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The IDCTs cannot be performed completely in-place due to the        */
/*      transpose that each pass performs.  In order to save data memory,   */
/*      the horizontal pass works from the end of the array towards the     */
/*      beginning, writing its result one IDCT block later in memory,       */
/*      thus performing the IDCT nearly-in-place.  The vertical pass        */
/*      performs its IDCTs in the opposite direction, working from the      */
/*      start of the array towards the end, writing the result in-place.    */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      One extra block's worth of storage must be present after the list   */
/*      of IDCT input blocks.                                               */
/*                                                                          */
/*  NOTES                                                                   */
/*      The cosine terms have all been scaled by sqrt(2), so that the       */
/*      "c4" term is basically an even power of 2.                          */
/*                                                                          */
/*      The precision of the final results can be changed by modifying      */
/*      the constants at the top of the code and reassembling.  Usually,    */
/*      modifying the final-shift constants in the "Symbolic Constants"     */
/*      section is sufficient.                                              */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

/* ======================================================================== */
/*  IMG_idct_8x8   -- Natural C version of IMG_idct_8x8().                  */
/* ======================================================================== */
void IMG_idct_8x8_c(short *idct_data, unsigned num_dcts)
{
    /* -------------------------------------------------------------------- */
    /*  Set up the cosine coefficients c1..c6.  c4 is replaced by a shift.  */
    /* -------------------------------------------------------------------- */
    const short c1 = 0x0B19, c2 = 0x0A74, c3 = 0x0968;
    const short c5 = 0x0649, c6 = 0x0454, c7 = 0x0235;
    const int   c4_shift = 11;

    /* -------------------------------------------------------------------- */
    /*  Rounding terms.  First pass is Q9, second pass is Q16.              */
    /*  These rouding terms are added to the DC component so that they      */
    /*  propagate to all of the outputs with a single ADD.  Clever, eh?     */
    /* -------------------------------------------------------------------- */
    const int round1 = 256, round2 = 32768;

    /* -------------------------------------------------------------------- */
    /*  Truncation amounts for the two passes.  Each pass truncates         */
    /*  slightly differently due to the need to retain precision between    */
    /*  the horizontal and vertical passes.                                 */
    /* -------------------------------------------------------------------- */
    const int trunc1 = 9, trunc2 = 16;

    /* -------------------------------------------------------------------- */
    /*  Input and output pointers, loop control.                            */
    /* -------------------------------------------------------------------- */
    short *i_ptr;
    short *o_ptr;
    unsigned    i, j;

    /* -------------------------------------------------------------------- */
    /*  Intermediate calculations.                                          */
    /* -------------------------------------------------------------------- */
    short X0, X1, X2, X3, X4, X5, X6, X7;   /* Frequency domain terms.      */
    int   P0, P1, p0, p1, r0, r1;           /* Even-half intermediate.      */
    int   g0, g1, h1, h0;                   /* Even-half results.           */
    int   g2, g3, h3, h2;                   /* Odd-half results.            */
    int   x0, x1, x2, x3, x4, x5, x6, x7;   /* Sample-domain results.       */
    int   x0t,x1t,x2t,x3t,x4t,x5t,x6t,x7t;  /* Truncated results.           */
    int   x0s,x1s,x2s,x3s,x4s,x5s,x6s,x7s;  /* Saturated/Truncated results. */

    /* -------------------------------------------------------------------- */
    /*  Avoid running the code if we don't have any IDCTs to do.            */
    /* -------------------------------------------------------------------- */
    if (!num_dcts) return;

    /* -------------------------------------------------------------------- */
    /*  Loop over the provided DCT blocks started at the end of the array   */
    /*  and working towards the beginning.  We initialize our input ptr     */
    /*  to point at the last row of the last valid 8x8 block.  The output   */
    /*  pointer is initialized to point to the rightmost column of the      */
    /*  scratch block which exists just after the last valid 8x8 block.     */
    /*  The horizontal pass ends up sliding the data set "forward" by a     */
    /*  whole block as a result.                                            */
    /* -------------------------------------------------------------------- */
    i_ptr = idct_data + num_dcts * 64 - 8;
    o_ptr = idct_data + num_dcts * 64 + 7;

    for (j = 0; j < num_dcts; j++)
    {
        /* ---------------------------------------------------------------- */
        /*  Perform Horizontal 1-D IDCT on each 8x8 block.  Store out       */
        /*  the results transposed.                                         */
        /* ---------------------------------------------------------------- */
        for (i = 0; i < 8; i++)
        {
            /* ------------------------------------------------------------ */
            /*  Load the freq-domain coefficients.                          */
            /* ------------------------------------------------------------ */
            X0 = i_ptr[0];
            X1 = i_ptr[1];
            X2 = i_ptr[2];
            X3 = i_ptr[3];
            X4 = i_ptr[4];
            X5 = i_ptr[5];
            X6 = i_ptr[6];
            X7 = i_ptr[7];

            i_ptr -= 8;             /* decrement pointer to next row    */

            /* ------------------------------------------------------------ */
            /*  Even part of decomposition (upper-half of flow graph).      */
            /*  (Note: rounding term is added to the "DC" value so that     */
            /*  it gets automagically applied to the whole IDCT.)           */
            /* ------------------------------------------------------------ */
            P0 = (((int)X0) << c4_shift) + round1;  /* c4 replaced by shift */
            P1 = (((int)X4) << c4_shift);           /* c4 replaced by shift */

            p0 = P0 + P1;
            p1 = P0 - P1;

            r1 = X2*c6 - X6*c2;
            r0 = X2*c2 + X6*c6;

            g0 = p0 + r0;
            g1 = p1 + r1;
            h1 = p1 - r1;
            h0 = p0 - r0;

            /* ------------------------------------------------------------ */
            /*  Odd part of decomposition (lower-half of flow graph).       */
            /* ------------------------------------------------------------ */
            g2 = (X1*c7 - X3*c5) + (X5*c3 - X7*c1);
            g3 = (X1*c5 - X3*c1) + (X5*c7 + X7*c3);
            h3 = (X1*c3 - X3*c7) - (X5*c1 + X7*c5);
            h2 = (X1*c1 + X3*c3) + (X5*c5 + X7*c7);

            /* ------------------------------------------------------------ */
            /*  Final butterfly.                                            */
            /* ------------------------------------------------------------ */
            x0 = g0 + h2;
            x1 = g1 + h3;
            x2 = h1 + g3;
            x3 = h0 + g2;
            x4 = h0 - g2;
            x5 = h1 - g3;
            x6 = g1 - h3;
            x7 = g0 - h2;

            /* ------------------------------------------------------------ */
            /*  Truncate to fit back into 16 bits before storing.           */
            /* ------------------------------------------------------------ */
            x0t = x0 >> trunc1;
            x1t = x1 >> trunc1;
            x2t = x2 >> trunc1;
            x3t = x3 >> trunc1;
            x4t = x4 >> trunc1;
            x5t = x5 >> trunc1;
            x6t = x6 >> trunc1;
            x7t = x7 >> trunc1;

            /* ------------------------------------------------------------ */
            /*  Store the results transposed in the result area.            */
            /* ------------------------------------------------------------ */
            o_ptr[ 0] = x0t;
            o_ptr[ 8] = x1t;
            o_ptr[16] = x2t;
            o_ptr[24] = x3t;
            o_ptr[32] = x4t;
            o_ptr[40] = x5t;
            o_ptr[48] = x6t;
            o_ptr[56] = x7t;

            o_ptr--;                /* decrement pointer to next column */
        }

        /* ---------------------------------------------------------------- */
        /*  Update output pointer to point to next block.  Input pointer    */
        /*  is already pointing to next block, so it doesn't need an        */
        /*  update.                                                         */
        /* ---------------------------------------------------------------- */
        o_ptr = o_ptr + 8 - 64;
    }

    /* -------------------------------------------------------------------- */
    /*  Reset our pointers for the vertical pass.  We start off with our    */
    /*  input pointer pointing to the first row of the 2nd block, and       */
    /*  our output pointer pointing to the leftmost column of the 1st       */
    /*  block.  As a result, the output data slides "back" by one block,    */
    /*  ultimately landing in-place.                                        */
    /* -------------------------------------------------------------------- */
    i_ptr = idct_data + 64;
    o_ptr = idct_data;

    for (j = 0; j < num_dcts; j++)
    {
        /* ---------------------------------------------------------------- */
        /*  Perform Vertical 1-D IDCT on each 8x8 block.  Store out the     */
        /*  results transposed.                                             */
        /* ---------------------------------------------------------------- */
        for (i = 0; i < 8; i++)
        {
            /* ------------------------------------------------------------ */
            /*  Load the freq-domain coefficients.                          */
            /* ------------------------------------------------------------ */
            X0 = i_ptr[0];
            X1 = i_ptr[1];
            X2 = i_ptr[2];
            X3 = i_ptr[3];
            X4 = i_ptr[4];
            X5 = i_ptr[5];
            X6 = i_ptr[6];
            X7 = i_ptr[7];
            i_ptr += 8;             /* increment pointer to next row    */

            /* ------------------------------------------------------------ */
            /*  Even part of decomposition (upper-half of flow graph).      */
            /*  (Note: rounding term is added to the "DC" value so that     */
            /*  it gets automagically applied to the whole IDCT.)           */
            /* ------------------------------------------------------------ */
            P0 = (((int)X0) << c4_shift) + round2;  /* c4 replaced by shift */
            P1 = (((int)X4) << c4_shift);           /* c4 replaced by shift */

            p0 = P0 + P1;
            p1 = P0 - P1;

            r1 = X2*c6 - X6*c2;
            r0 = X2*c2 + X6*c6;

            g0 = p0 + r0;
            g1 = p1 + r1;
            h1 = p1 - r1;
            h0 = p0 - r0;

            /* ------------------------------------------------------------ */
            /*  Odd part of decomposition (lower-half of flow graph).       */
            /* ------------------------------------------------------------ */
            g2 = (X1*c7 - X3*c5) + (X5*c3 - X7*c1);
            g3 = (X1*c5 - X3*c1) + (X5*c7 + X7*c3);
            h3 = (X1*c3 - X3*c7) - (X5*c1 + X7*c5);
            h2 = (X1*c1 + X3*c3) + (X5*c5 + X7*c7);

            /* ------------------------------------------------------------ */
            /*  Final butterfly.                                            */
            /* ------------------------------------------------------------ */
            x0 = g0 + h2;
            x1 = g1 + h3;
            x2 = h1 + g3;
            x3 = h0 + g2;
            x4 = h0 - g2;
            x5 = h1 - g3;
            x6 = g1 - h3;
            x7 = g0 - h2;

            /* ------------------------------------------------------------ */
            /*  Truncate and saturate final results.                        */
            /* ------------------------------------------------------------ */
            x0t = x0 >> trunc2;
            x1t = x1 >> trunc2;
            x2t = x2 >> trunc2;
            x3t = x3 >> trunc2;
            x4t = x4 >> trunc2;
            x5t = x5 >> trunc2;
            x6t = x6 >> trunc2;
            x7t = x7 >> trunc2;

            x0s = x0t < -256 ? -256 : x0t > 255 ? 255 : x0t;
            x1s = x1t < -256 ? -256 : x1t > 255 ? 255 : x1t;
            x2s = x2t < -256 ? -256 : x2t > 255 ? 255 : x2t;
            x3s = x3t < -256 ? -256 : x3t > 255 ? 255 : x3t;
            x4s = x4t < -256 ? -256 : x4t > 255 ? 255 : x4t;
            x5s = x5t < -256 ? -256 : x5t > 255 ? 255 : x5t;
            x6s = x6t < -256 ? -256 : x6t > 255 ? 255 : x6t;
            x7s = x7t < -256 ? -256 : x7t > 255 ? 255 : x7t;

            /* ------------------------------------------------------------ */
            /*  Store the results transposed in the result area.            */
            /* ------------------------------------------------------------ */
            o_ptr[ 0] = x0s;
            o_ptr[ 8] = x1s;
            o_ptr[16] = x2s;
            o_ptr[24] = x3s;
            o_ptr[32] = x4s;
            o_ptr[40] = x5s;
            o_ptr[48] = x6s;
            o_ptr[56] = x7s;

            o_ptr++;                /* increment pointer to next column */
        }
        /* ---------------------------------------------------------------- */
        /*  Update output pointer to point to next block.  (See above.)     */
        /* ---------------------------------------------------------------- */
        o_ptr = o_ptr - 8 + 64;
    }
}

/* ======================================================================== */
/*  End of file:  img_idct_8x8.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
