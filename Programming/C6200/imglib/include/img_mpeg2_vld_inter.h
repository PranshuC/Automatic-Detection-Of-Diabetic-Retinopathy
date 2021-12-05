/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  IMGLIB  DSP Image/Video Processing Library                              */
/*                                                                          */
/*      Release:        Revision 1.04b                                      */
/*      CVS Revision:   1.9     Sun Sep 29 03:31:27 2002 (UTC)              */
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
/*      IMG_mpeg2_vld_inter                                                 */
/*                                                                          */
/*  PLATFORM                                                                */
/*      C6200                                                               */
/*                                                                          */
/*  REVISION DATE                                                           */
/*      03-Dec-2001                                                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*        void IMG_mpeg2_vld_inter                                          */
/*        (                                                                 */
/*            const short   *restrict Wptr,                                 */
/*            short         *restrict outi,                                 */
/*            IMG_mpeg2_vld *restrict Mpeg2v                                */
/*        );                                                                */
/*                                                                          */
/*        Wptr:       Pointer to the quantization matrix.  Must be in       */
/*                    the scan order corresponding to the inverse scan      */
/*                    matrix.                                               */
/*                                                                          */
/*        outi[6*64]: Pointer to the output array where the decoded and     */
/*                    dequantized IDCT coefficients are placed.             */
/*                    Elements assumed to be set to zero prior to           */
/*                    function call.  The routine assumes 6 8x8 blocks      */
/*                    per MB, i.e. 4:2:0 format.                            */
/*                                                                          */
/*        Mpeg2v:     Pointer to the context object, which includes the     */
/*                    input buffer variables.  The structure Mpeg2v is      */
/*                    defined as follows:                                   */
/*                                                                          */
/*          #ifndef IMG_MPEG2_VLD_STRUCT_                                   */
/*          #define IMG_MPEG2_VLD_STRUCT_ 1                                 */
/*                                                                          */
/*          typedef struct {                                                */
/*            unsigned int  *bsbuf;      // pointer to bitstream buffer     */
/*            unsigned int  next_wptr;   // next word to read from buffer   */
/*            unsigned int  bptr;        // bit position within word        */
/*            unsigned int  word1;       // word aligned buffer             */
/*            unsigned int  word2;       // word aligned buffer             */
/*            unsigned int  top0;        // top 32 bits of bitstream        */
/*            unsigned int  top1;        // next 32 bits of bitstream       */
/*            const unsigned char *scan; // inverse zigzag scan matrix      */
/*            unsigned int  intravlc;    // intra_vlc_format                */
/*            unsigned int  quant_scale; // quant_scale                     */
/*            unsigned int  dc_prec;     // intra_dc_precision              */
/*            unsigned int  cbp;         // coded_block_pattern             */
/*            unsigned int  fault;       // fault condition (returned)      */
/*            unsigned int  reserved;                                       */
/*          } IMG_mpeg2_vld;                                                */
/*                                                                          */
/*          #endif                                                          */
/*                                                                          */
/*        All variables in this structure must have the layout as shown     */
/*        since they are being accessed by this routine through             */
/*        appropriate offsets.  Other variables may be appended to the      */
/*        structure.                                                        */
/*                                                                          */
/*        The routine sets the fault flag Mpeg2v.fault to 1 if an invalid   */
/*        VLC code was encountered or the total run for a block exceeded    */
/*        63.  In theses cases the decoder has to resynchronize.            */
/*                                                                          */
/*      The routine requires proprietary variable length decoding           */
/*      look-up tables.  The tables are based on Table B-14 and B-15 in     */
/*      the MPEG-2 standard text.                                           */
/*                                                                          */
/*      The required lookup tables for this routine are provided in         */
/*      IMGLIB and are linked in automatically when linking against         */
/*      IMGLIB.                                                             */
/*                                                                          */
/*      Before calling the routine the bitstream varaibles in Mpeg2v have   */
/*      to be initialized.  If bsbuf[] is a circular buffer of size         */
/*      BSBUF_SIZE words and bsptr contains the number of bits in the       */
/*      buffer that already have been consumed, then next_wptr, bptr,       */
/*      word1, word2, top0 and top1 are initialized as follows:             */
/*                                                                          */
/*      1. nextwptr:  bsptr may not be a multiple of 32 (e.g. it points     */
/*         to bit number 40 in the bitstream), therefore set it to the      */
/*         next lower multiple of 32 by shift left by 5 (divide by 32).     */
/*         In case of 40 this yields 1.  Since memory is addressed in       */
/*         units of bytes this needs to be multiplied by 4 to get the       */
/*         address pointer.  Use next_wptr as index into bsbuf[].           */
/*                                                                          */
/*             next_wptr = (bsptr >> 5);                                    */
/*                                                                          */
/*      2. bptr:  bptr is the bit pointer which points to the current       */
/*         bit WITHIN the word pointed to by next_wptr.                     */
/*                                                                          */
/*            bptr = bsptr & 31;                                            */
/*            bptr_cmpl = 32 - bptr;                                        */
/*                                                                          */
/*      3. word1 and word2:  read next 3 words from the bitstream buffer    */
/*         (word0 is a temporary variable).                                 */
/*                                                                          */
/*            word0 = bsbuf[next_wptr];                                     */
/*            next_wptr = (next_wptr + 1) & (BSBUF_SIZE-1);                 */
/*                                                                          */
/*            word1 = bsbuf[next_wptr];                                     */
/*            next_wptr = (next_wptr + 1) & (BSBUF_SIZE-1);                 */
/*                                                                          */
/*            word2 = bsbuf[next_wptr];                                     */
/*            next_wptr = (next_wptr + 1) & (BSBUF_SIZE-1);                 */
/*                                                                          */
/*      4. top0 and top1:  Shift words word0, word1, word2 by bptr to       */
/*         the left so that the current bit becomes the MSB in word0.       */
/*         word0 can simply be shifted by bptr; the then empty LSBs of      */
/*         word0 have to be filled with the MSBs of word1.  To do that      */
/*         the required MSBs are brought into the position of empty         */
/*         LSBs of word0 by shifting word1 to the right by (32-bptr).       */
/*         The result is then copied into word0 by an addition.  Rather     */
/*         than overwriting word0, top0 is used to hold the new bit         */
/*         aligned word.  The same procedure is used to obtain top1.        */
/*         top0 and top1 contain the next 64 bits of the bitstream.         */
/*                                                                          */
/*            s1 = SHL(word0, bptr);                                        */
/*            s2 = SHR(word1, bptr_cmpl);  // unsigned right-shift          */
/*            top0 = s1 + s2;                                               */
/*                                                                          */
/*            s3 = SHL(word1, bptr);                                        */
/*            s4 = SHR(word2, bptr_cmpl);  //  unsigned right-shift         */
/*            top1 = s3 + s4;                                               */
/*                                                                          */
/*        SHL/SHR are macros that simulate C6000 shift functions if the     */
/*        routine is to be run on a PC or UNIX platform. They are           */
/*        defined as follows:                                               */
/*                                                                          */
/*            #ifdef _TMS320C6X                                             */
/*            # define SHL(x,y) ( (x) << (y) )                              */
/*            # define SHR(x,y) ( (x) >> (y) )                              */
/*            #else                                                         */
/*            # define SHL(x,y) ( ((y) & 32) ? 0 : ((x) << (y)) )           */
/*            # define SHR(x,y) ( ((y) & 32) ? 0 : ((x) >> (y)) )           */
/*            #endif                                                        */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      Decodes all coefficients of an MPEG-2 non-intra coded macroblock.   */
/*                                                                          */
/*      Checks coded block pattern (cbp), performs DC and AC decoding       */
/*      inlcuding, variable length decode, run-length expansion,            */
/*      inverse zigzag, dequantization, saturation and mismatch             */
/*      control.                                                            */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*    - bitstream buffer is set to 512 32-bit words (=2048 bytes),          */
/*      buffer needs to be aligned at a 2048 boundary because it is         */
/*      circular.  If this needs to be changed, AMR register setup has to   */
/*      be modified  and alignment changed accordingly. Register B7 is      */
/*      used as the address pointer to the bitstream buffer in              */
/*      circular addressing mode with a size of 2^(10+1) bytes = 2048       */
/*      bytes = 512 words. Accordingly, AMR is set to 0x000A0004. Note      */
/*      that the AMR register is set to zero on exit.                       */
/*                                                                          */
/*    - zigzag matrix (Zptr) is 64 bytes circularly addressed and needs to  */
/*      be aligned at a 64 byte boundary (serves protection from random     */
/*      stores into memory)                                                 */
/*                                                                          */
/*    - inside the routine word1 (next_wptr-2) and word2 (next_wptr-1)      */
/*      are reconstructed from the bitstream buffer and therfore have to    */
/*      be kept alive in the bitstream buffer. For instance, in a double    */
/*      buffering scheme the bitstream buffer can only be updated when      */
/*      next_wptr-2 (and not next_wptr) has crossed the half buffer         */
/*      boundary.                                                           */
/*                                                                          */
/*    - look-up tables len and len_c should be offset against each other    */
/*      so that they start in different memory banks to avoid bank          */
/*      conflicts.                                                          */
/*                                                                          */
/*  NOTES                                                                   */
/*    - little endian                                                       */
/*                                                                          */
/*    - non-interruptible                                                   */
/*                                                                          */
/*    - 4:2:0 color format supported only                                   */
/*                                                                          */
/*    - Wptr is allowed to overrun once (to detect total run overrun), so   */
/*      maximum overrun that can occur is 66 (Error mark). Therefore,       */
/*      in memory 66+1 half-words behind the weighting matrix should be     */
/*      valid (e.g. no cache or peripherals). No memory is overwritten,     */
/*      only loads occurr.                                                  */
/*                                                                          */
/*  CYCLES                                                                  */
/*      10 * S + 48 * CB + 15 * NCB + 60                                    */
/*      where S:   Number of symbols in MB, CB: Number of coded blocks,     */
/*            NCB: Number of not-coded blocks, and CB+NCB=6                 */
/*                                                                          */
/*  CODE SIZE                                                               */
/*      1376 bytes                                                          */
/*                                                                          */
/*  DATA SIZE                                                               */
/*      2176 bytes for the lookup tables                                    */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMG_MPEG2_VLD_INTER_H_
#define IMG_MPEG2_VLD_INTER_H_ 1

#ifndef IMG_MPEG2_VLD_STRUCT_
#define IMG_MPEG2_VLD_STRUCT_ 1

typedef struct {
  unsigned int  *bsbuf;      // pointer to bitstream buffer
  unsigned int  next_wptr;   // next word to read from buffer
  unsigned int  bptr;        // bit position within word
  unsigned int  word1;       // word aligned buffer
  unsigned int  word2;       // word aligned buffer
  unsigned int  top0;        // top 32 bits of bitstream
  unsigned int  top1;        // next 32 bits of bitstream
  const unsigned char *scan; // inverse zigzag scan matrix
  unsigned int  intravlc;    // intra_vlc_format
  unsigned int  quant_scale; // quant_scale
  unsigned int  dc_prec;     // intra_dc_precision
  unsigned int  cbp;         // coded_block_pattern
  unsigned int  fault;       // fault condition (returned)
  unsigned int  reserved;
} IMG_mpeg2_vld;

#endif

void IMG_mpeg2_vld_inter
(
    const short   *restrict Wptr,
    short         *restrict outi,
    IMG_mpeg2_vld *restrict Mpeg2v
);

#endif
/* ======================================================================== */
/*  End of file:  img_mpeg2_vld_inter.h                                     */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
