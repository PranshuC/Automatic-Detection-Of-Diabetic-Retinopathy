/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      vld_demo    -- Demo for MPEG-2 VLD routines                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This code contains an example which explains how to call the        */
/*      MPEG-2 VLD intra and inter routines.                                */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#include <stdio.h>
#include <stdlib.h>

#define SHL(x,y) ( (x) << (y) )
#define SHR(x,y) ( (x) >> (y) )

/* ------------------------------------------------------------------------ */
/*  IMGLIB Prototypes                                                       */
/* ------------------------------------------------------------------------ */
#include "IMG_mpeg2_vld_intra.h" 
#include "IMG_mpeg2_vld_inter.h" 

#include "global.h"                     /* MPEG-2 bitstream context         */

/* ------------------------------------------------------------------------ */
/*  Generates random MPEG-2 MB bitstream                                    */
/* ------------------------------------------------------------------------ */
int vlc_gen(IMG_mpeg2_vld *mpeg2_vld_cont, int *mb_type, short *output_gen);

/* ------------------------------------------------------------------------ */
/*  Main                                                                    */
/* ------------------------------------------------------------------------ */
int main()
{
	MPEG2DEC_INFO Mpeg2v;               /* MPEG-2 bitstream info            */
    unsigned int *bsbuf;                /* memory bitstream buffer          */
    int bits_enc, bits_dec;             /* number of bits encoded, decoded  */
    int dc_dct_pred[3];                 /* intra DC predictors              */
    int i,mb;                           /* counters                         */
    int mb_type;                        /* macroblock type: 0=INTER,1=INTRA */
    short output_gen[NUM_MB*6*64];/* output coefs from VLC random generator */
    short output_vld[6*64];       /* output coefs from VLD routine          */
    int error=0;
    
    /* -------------------------------------------------------------------- */
    /*  variables for bitstream management                                  */
    /* -------------------------------------------------------------------- */
    unsigned int word0, word1, word2;     /* word-aligned bitstream buffers */
    unsigned int top0, top1;              /* bit-aligned bitstream buffers  */
    unsigned int next_wptr, old_next_wptr;/* index, next word in bitstream  */
    unsigned int bptr=0,old_bptr, bptr_cmpl;/* bit position of bitstream    */
    unsigned int s1, s2, s3, s4;          /* temporaray variables           */
    
    /* -------------------------------------------------------------------- */
    /* allocate bitstream buffer for circular addressing of size            */
    /* BSBUF_SIZE, aligned at BSBUF_SIZE (BSBUF_SIZE in 32-bit words)       */
    /* -------------------------------------------------------------------- */
    bsbuf = (unsigned int*) memalign(4*BSBUF_SIZE, 4*BSBUF_SIZE); 

    Mpeg2v.mpeg2_vld_cont.bsbuf = bsbuf;
    Mpeg2v.mpeg2_vld_cont.fault = 0;    

    while(!Mpeg2v.mpeg2_vld_cont.fault)
    {
    /* -------------------------------------------------------------------- */
    /*  Fill bitstream buffer bsbuf with random VLC codes for TWO MPEG-2    */
    /*  4:2:0 macroblocks (the same coding paramters are used for both      */
    /*  MB's. Only the block data of the macroblocks is written. Any other  */
    /*  syntax elements are omitted. The purpose is to demonstrate the      */
    /*  chaining capability of the VLD routine. The routine returns the     */
    /*  current state of the bitstream so that other routines using the     */
    /*  same bitstream management can be called consecutively using the     */
    /*  returned bistream state of a previously called routine as input     */
    /*  and starting parsing the bitstream at the position where the        */
    /*  previous routine stopped.                                           */
    /*  The routine also sets next_wptr, mb_type, intra_vlc_format,         */
    /*  quant_scale, cbp (coded block pattern) and intra_dc_precision in    */
    /*  the Mpeg2v structure to                                             */
    /*  random values. bptr is always 0. scan is set to the inverse zigzag  */
    /*  matrix 0. The expected DCT coefficient blocks are written to        */
    /*  output_gen. The routine returns the number of bits written to the   */
    /*  bitstream buffer. The bitstream buffer is filled starting always at */
    /*  word boundaries (i.e. bptr is assumed to be 0) at index next_wptr.  */
    /* -------------------------------------------------------------------- */

    bits_enc = vlc_gen(&Mpeg2v.mpeg2_vld_cont, &mb_type, output_gen); 
    
   /* --------------------------------------------------------------------- */
   /*  Print out some bitstream information                                 */
   /* --------------------------------------------------------------------- */
    printf("Coding paramters for the %d MB's:\n", NUM_MB);
    if (mb_type==1)
    {
        printf("mb_type=INTRA, "); 
        printf("intra_vlc_format=%1d, ",   Mpeg2v.mpeg2_vld_cont.intravlc);
        printf("intra_dc_precision=%1d, ", Mpeg2v.mpeg2_vld_cont.dc_prec);
    }
    else
    {
        printf("mb_type=INTER, "); 
    }
    printf("cbp=%2Xh, ",               Mpeg2v.mpeg2_vld_cont.cbp);
    printf("quant_scale=%2d\n",        Mpeg2v.mpeg2_vld_cont.quant_scale);
    printf("Bitstream info:\n");
    printf("bits=%4d, ",               bits_enc);
    printf("next_wptr=%d\n",           Mpeg2v.mpeg2_vld_cont.next_wptr);
            
   /* --------------------------------------------------------------------- */
   /*  Setup the bitstream and MB context for the first call of the VLD     */
   /*  routine. Bitstream is assumed to have been stored as 32-bit sized    */
   /*  elements.                                                            */
   /* --------------------------------------------------------------------- */
    next_wptr = Mpeg2v.mpeg2_vld_cont.next_wptr;
    
    bptr_cmpl = 32 - bptr;      

    word0 = bsbuf[next_wptr];            /* read first 32 bits of bitstream */
    next_wptr = (next_wptr + 1) & (BSBUF_SIZE-1);  /* circularly inc. index */

    word1 = bsbuf[next_wptr];           /* read second 32 bits of bitstream */
    next_wptr = (next_wptr + 1) & (BSBUF_SIZE-1);  /* circularly inc. index */

    word2 = bsbuf[next_wptr];            /* read third 32 bits of bitstream */
    next_wptr = (next_wptr + 1) & (BSBUF_SIZE-1);  /* circularly inc. index */

    /* -------------------------------------------------------------------- */
    /*  setup the bit aligned bitstream buffer top0 and top1. In this case  */
    /*  top0 is identical to word0 and top1 identical to word1. However,    */
    /*  if bptr != 0, top0 and top1 are obtained as follows:                */
    /* -------------------------------------------------------------------- */
    s1 = SHL(word0, bptr);
    s2 = SHR(word1, bptr_cmpl);                     /* unsigned right-shift */
    top0 = s1 + s2;                         

    s3 = SHL(word1, bptr);
    s4 = SHR(word2, bptr_cmpl);                     /* unsigned right-shift */
    top1 = s3 + s4;
           
    /* -------------------------------------------------------------------- */
    /*  save next_wptr and bptr to be able to calculate how many bits       */
    /*  were decoded by the VLD routine                                     */
    /* -------------------------------------------------------------------- */
    old_next_wptr = next_wptr;
    old_bptr      = bptr;

    /* -------------------------------------------------------------------- */
    /*  assign the determined bitstream info to the Mpeg2v structure        */
    /* -------------------------------------------------------------------- */
    Mpeg2v.mpeg2_vld_cont.next_wptr = next_wptr;
    Mpeg2v.mpeg2_vld_cont.bptr  = bptr;
    Mpeg2v.mpeg2_vld_cont.word1 = word1;
    Mpeg2v.mpeg2_vld_cont.word2 = word2;		
    Mpeg2v.mpeg2_vld_cont.top0  = top0;			
    Mpeg2v.mpeg2_vld_cont.top1  = top1;		
    Mpeg2v.mpeg2_vld_cont.scan  = izigzag_tbl;

    /* -------------------------------------------------------------------- */
    /*  in this example assume that the initial intra DC predictors are zero*/
    /* -------------------------------------------------------------------- */
    dc_dct_pred[0] = 0;
    dc_dct_pred[1] = 0;
    dc_dct_pred[2] = 0;

      /* ----------------------------------------------------------------- */
      /*  decode MB's                                                      */
      /* ----------------------------------------------------------------- */
      for (mb=0; mb<NUM_MB; mb++)
      {
        /* ---------------------------------------------------------------- */
        /*  clear output buffer and call VLD routine                        */
        /* ---------------------------------------------------------------- */
        for (i=0; i<6*64; i++) 
                output_vld[i] = 0;

        if (mb_type==1) /* INTRA */
            IMG_mpeg2_vld_intra(Warr, output_vld, &Mpeg2v.mpeg2_vld_cont, 
            					dc_dct_pred);
        else            /* INTER */
            IMG_mpeg2_vld_inter(Warr_inter, output_vld, &Mpeg2v.mpeg2_vld_cont);

        /* ---------------------------------------------------------------- */
        /*  Verify output                                                   */
        /* ---------------------------------------------------------------- */
        bits_dec = ((Mpeg2v.mpeg2_vld_cont.next_wptr<<5) + Mpeg2v.mpeg2_vld_cont.bptr) 
            - ((old_next_wptr<<5) + old_bptr);
        if (bits_dec<0) 
            bits_dec += BSBUF_SIZE<<5;
               
        printf("Decoded %4d bits, ", bits_dec);
        printf("Fault flag : %d\n", Mpeg2v.mpeg2_vld_cont.fault);

        for (error=0, i=0; i<6*64; i++)
            if (output_gen[mb*6*64+i] != output_vld[i])
            	error=1;

        if (error)
        {
             printf("Error in macroblock (should:is):\n");
             for (i=0; i<6*64; i++)
             {
               if(i%8==0) printf("\n");
               if(i%64==0) printf("\n");
               printf("%d:%d, ", output_gen[mb*6*64+i], output_vld[i]);
             }
        }
            
        old_next_wptr = Mpeg2v.mpeg2_vld_cont.next_wptr;
        old_bptr      = Mpeg2v.mpeg2_vld_cont.bptr;
        
        if (Mpeg2v.mpeg2_vld_cont.fault)
            mb=NUM_MB;                   /* force for mb loop to terminate */
        
      }   /* for mb */
    }   /* while */ 

    free(bsbuf);
    return 0;
}
