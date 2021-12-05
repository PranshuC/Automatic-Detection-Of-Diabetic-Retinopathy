/* ======================================================================== */
/*  global.h                                                                */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#include "IMG_mpeg2_vld_intra.h" /* define IMG_mpeg2_vld structure        */

#define BSBUF_SIZE  512          /* bitstream buffer size in 32-bit words */
#define NUM_MB      2

typedef struct 
{
    /*  The IMG_mpeg2_vlf structure must occur as first element */
    IMG_mpeg2_vld   mpeg2_vld_cont;
    
    /* Here, user-defined elements may follow */

} MPEG2DEC_INFO;

/* ------------------------------------------------------------------------ */
/*  MPEG-2 quantization and inverse zigzag tables                           */
/* ------------------------------------------------------------------------ */
extern const short Warr[64];
extern const short Warr_inter[64];
extern const unsigned char izigzag_tbl[64];
extern const unsigned char izigzag_tbl1[64];

