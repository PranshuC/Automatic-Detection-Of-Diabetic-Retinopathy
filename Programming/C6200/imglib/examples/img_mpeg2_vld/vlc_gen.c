/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      vlc_gen     -- MPEG-2 VLC random generator                          */
/*                                                                          */
/*  USAGE                                                                   */
/*      This code is called by the MPEG-2 VLD example code                  */
/*      vld_demo.                                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "c6000.h"
#include "global.h"

unsigned bitstream[NUM_MB*288];          /* bitstream buffer for NUM_MB MBs */
unsigned int *bsbuf;     /* aligned bitstream buffer for circular addressing*/
unsigned input[NUM_MB*6*65];                                /* input codes  */

int intra_dc_precision;
int cbp;

typedef struct vld_entry
{
    unsigned    code;
    char        run;
    char        len;
    short       lvl;
} vld_entry_t;

#define MAX_ENTRIES (1024)

vld_entry_t vld_tbl_0[MAX_ENTRIES];
vld_entry_t vld_tbl_1[MAX_ENTRIES];
vld_entry_t *vld_tbl;

/* MPEG-2 VLC tables for intra DC coding */
unsigned int dc_lum_len[12]      
    = {   3,   2,   2,   3,   3,   4,    5,    6,    7,    8,     9,     9 };
unsigned int dc_lum_code[12]     
    = { 0x4, 0x0, 0x1, 0x5, 0x6, 0xE, 0x1E, 0x3E, 0x7E, 0xFE, 0x1FE, 0x1FF };
unsigned int dc_chrom_len[12]    
    = {   2,   2,   2,   3,   4,   5,     6,    7,    8,     9,    10,    10 };
unsigned int dc_chrom_code[12]   
    = { 0x0, 0x1, 0x2, 0x6, 0xE, 0x1E, 0x3E, 0x7E, 0xFE, 0x1FE, 0x3FE, 0x3FF };

/* Reads the VLD table file and does stuff with it. */
int parse_tbl(FILE *f, vld_entry_t vld_tbl[], int max_entries)
{
    char buf[256];
    int i,rn,lv,ln,e=0;
    char *s;
    unsigned cd;
    
    while (fgets(buf,256,f))
    {
        i = sscanf(buf,"%d %d\n",&rn,&lv);
        
        if (i != 2) continue;
        
        s = buf;
        
        while (*s &&  isspace(*s)) s++;
        while (*s && !isspace(*s)) s++;
        while (*s &&  isspace(*s)) s++;
        while (*s && !isspace(*s)) s++;
        while (*s &&  isspace(*s)) s++;
        
        ln = cd = 0;

        while (*s && (*s == '0' || *s == '1'))
            cd |= (*s++ - '0') << (31-ln++);
        
        if (rn==-1 && lv==-1) ln--; /* EOB doesn't have sign bit attached */

        if (ln > 0)
        {
            if (e == max_entries)
            {
                fprintf(stderr,"Too many entries (max = %d)\n",
                    max_entries);
                exit(1);
            }
            vld_tbl[e].run   = rn;
            vld_tbl[e].lvl   = lv;
            vld_tbl[e].len   = ln;
            vld_tbl[e].code  = cd;
            e++;
        }
        
    }
    
    return e;
}

unsigned generate_code(vld_entry_t vld_tbl[], int e, int run, int lvl, int *len)
{
    int i;
    int s = 0;
    int esc = -1;
    
    if (run >= 0 && lvl < 0)
    {
        s   = 1;
        lvl = -lvl;
    }
    
    for (i=0; i<e; i++)
    {
        if (vld_tbl[i].lvl == -2) esc = i;
        if (vld_tbl[i].run == run &&
            vld_tbl[i].lvl == lvl)
            break;
    }
    
    if (i < e)
    {
        *len = vld_tbl[i].len + 1;
        return vld_tbl[i].code | (s << (31 - vld_tbl[i].len));
    }
    
    if (esc >= 0)
    {
        *len = 24; /* escape code */
        if (s) lvl = -lvl;
        return  vld_tbl[esc].code | 
            ((run & 0x003F) << 20) | 
            ((lvl & 0x0FFF) <<  8);
    }
    
    fprintf(stderr,"Unable to generate code for %d, %d\n",run,lvl);
    exit(1);

    return 0;
}

int encode_bitstream(unsigned *input, int count, unsigned *bitstream,
                     vld_entry_t vld_tbl[], int e)
{
    unsigned code, dc_size, dc_val;
    unsigned op,p,w0;
    int len, i;
    short run,lvl;
    int total_bit = 0;
    int block=5;    
    int prev_run=-1; /* EOB */
    
    w0 = p = 0;
    
    for (i=0; i<count; i++)
    {
        if (block==-1) block=5;

        if (prev_run==-1 && (cbp&(1<<block)) == 0)
        {
            block--;
            i--;
            continue;
        }
        
        run = *input >> 16;
        lvl = *input & 0xFFFF;

        if (lvl==0) // generate intra DC code
        {
            dc_size = (float)rand() / 32768.0 * 12; // generate dc_size between 0..11
            dc_val = rand() & ((1<<dc_size)-1);     // generate DC value
            *input = (dc_size << 16) + dc_val;
            if (block>1)
            {
                len = dc_lum_len[dc_size] + dc_size;
                code = dc_lum_code[dc_size];
            }
            else
            { 
                len = dc_chrom_len[dc_size] + dc_size;
                code = dc_chrom_code[dc_size];
            }
            code = (code << dc_size) + dc_val;
            code = code << (32-len);
            block--;
            /* printf("outputting DC %3d/%-5d: %.8X [%2d]\n",
                    dc_size,dc_val,code,len); */
            prev_run=0; 
            /* prevent B-14 table modification (only done for inter) */
        }
        else
        {
            code = generate_code(vld_tbl, e, run, lvl, &len);
            /* MPEG-2 comp: modify the code of the _first_ coefficient if
             * run=0 and level=1: 11s becomes 1s. This is for Table B-14
             * (=vld_table_0), inter block and non-escape codes only.
             * Note: in case of intra the first symbol is always lvl=0 and
             * a DC code is generated, i.e. we don't need to check here if
             * it's inter because in intra we never get here */
            if (vld_tbl==vld_tbl_0 && prev_run==-1/*EOB*/ && run==0 
                    && (lvl==1 || lvl==-1) && len!=24)
            {
                code = code << 1;
                len = len - 1;
            }
            prev_run=run;
            /* printf("outputting %3d/%-5d: %.8X [%2d]\n",run,lvl,code,len); */
        }
        
        input++; 

        w0 |= code >> (op=p);
        
        total_bit += len;
        p         += len;
        if (p > 31)
        {
            *bitstream++ = w0;
            p -= 32;
            w0 = p ? code << (32 - op) : 0;
        }
    }
        
    if (p)
      *bitstream++ = w0;
    
    return total_bit;
}

int dequant(unsigned int *input, short *outi, int mb_type, 
        int intra_dc_precision, int dc_pred[3], int cbp, int qscl) 
{
    const unsigned char *zzptr; 
    const short *Wptr;
    int block; 
    unsigned int dc_size;
    int dc_diff, val, half_range;
    int i=0, cc;
    char run;
    short level;
    
    int neg, f1, f3, f5, qW, sum;
#ifdef _TMS320C6X
    int f4;
#else
    _int64 f4; 
#endif
    short W;
    unsigned char cnum;
   
    for (block=0; block<6; block++)
    {
        
    /* -------------------------------------------------------------------- 
     *  cbp: Bit 5 4 3 2 1 0 , if the corresponding bit is zero block no.
     *  (5-bit pos) is not coded 
     * -------------------------------------------------------------------- */
        if (!(cbp & (1 << (5-block))))              
            continue;
        
        run = sum = 0;
        zzptr = izigzag_tbl;

        if (mb_type==1) /* INTRA */
            Wptr=Warr; 
        else            /* INTER */
            Wptr=Warr_inter; 
        
    /* -------------------------------------------------------------------- 
     *  Intra DC dequantization 
     * -------------------------------------------------------------------- */

        if (mb_type==1) /* INTRA */
        {
            dc_size = (input[i] >> 16) & 0xFF;
            dc_diff = input[i] & 0xFFFF;
            i++;
               
            if (dc_size!=0)
            {
                half_range = 1 << (dc_size-1);
                if (dc_diff < half_range)
                    dc_diff = (dc_diff+1)-(2*half_range);
            }
            cc = (block<4) ? 0 : (block&1)+1;
            val = (dc_pred[cc]+= dc_diff); 

            outi[block*64+0] = val << (3-intra_dc_precision);

            sum += outi[block*64+0];

            Wptr++;
            zzptr++;
        }
        
    /* ----------------------------------------------------------------
     *  AC coefficients run-lengh expansion and DQ 
     * ---------------------------------------------------------------- */
        while (run!=-1) 
        {
            
            run   = (input[i] >> 16) & 0xFF;
            level =  input[i] & 0xFFFF;
            i++;

            if (run!=-1)
            {
                
                neg = (level < 0);      
                f1 = 2*level; 

                if (mb_type==0) /* INTER */
                {
                    if (neg) f1--;
                    else f1++;
                }
                    
                W = *(Wptr += run);  
                Wptr++;
                
                qW = qscl * W;
                f3 = f1 * qW;
                
                if (neg) f3 += 31;
                
                f4 = _sshl(f3, 15);
                f5 = (int)(f4 >> 20);
                
                sum += f5;
                
                cnum = *(zzptr += run); 
                zzptr++;
            
                outi[block*64+cnum] = f5;
            }
            
        } /* while */
    
    /* --------------------------------------------------------------------
     *  mismatch control: toggle last bit of last coefficient if sum of
     *  coefficents is even.
     * -------------------------------------------------------------------- */

     if ((sum&1)==0)
        {
            outi[block*64+63] ^= 1;
        }

    } /* for */ 

    return i;
}


#define MAXRUN (63)

int vlc_gen(IMG_mpeg2_vld *mpeg2_vld_cont, int *mb_type, short *output_gen)
{
    int e, block, k;
    int i, mb, pos, r, run, lvl, ent;
    int tot_coef = 63;
    FILE *table;
    int a, intra, intra_vlc_format; 
    unsigned int next_wptr;
    int dc_pred[3]={0,0,0};
    
    printf("\nGenerating random VLC codes for %d MBs...\n", NUM_MB);
    
    table = fopen("table_0.txt","rb"); 
    e = parse_tbl(table,vld_tbl_0,MAX_ENTRIES);
    fclose(table);

    table = fopen("table_1.txt","rb"); 
    e = parse_tbl(table,vld_tbl_1,MAX_ENTRIES);
    fclose(table);

    srand(time(0));                                          /* random seed */
    
    i = 0;
        
    intra = rand() & 1;                /* choose intra or non-intra MB type */

    if (!intra)
    {
        vld_tbl = vld_tbl_0;
    }
    else /* intra */
    {
        intra_vlc_format = rand() & 1;   
        /* randomly choose table 0 or 1 for intra blocks */
        if (intra_vlc_format==0)
            vld_tbl = vld_tbl_0;
        else /* intra_vlc_format==1 */
            vld_tbl = vld_tbl_1;
    }

    cbp = rand() & 0x3F;                     /* random cbp */
        
    for (mb=0; mb<NUM_MB; mb++)
    {
        for (block=0; block<6; block++)
        {
            if (!(cbp & (1 << (5-block))))              
                continue;
                
            r = 0;

            while (r <= tot_coef)
            {
                ent = rand() & 127;         /* random table index */
                    
                if (r==0 && intra)
                {
                    run = 0;        /* intra DC coeff takes 1 position      */
                    lvl = 0;        /* indicates to insert an intra DC code */
                                    /* (lvl=0 not allowed for AC coeffs)    */
                }
                else if (ent < e && vld_tbl[ent].run >= 0)
                {
                    run = vld_tbl[ent].run;
                    lvl = vld_tbl[ent].lvl;
                    if (rand() & 1) lvl = -lvl;
                } 
                else
                {
                    run = (rand() % MAXRUN);    /* change to increase range */
                    lvl = (rand() & 4095) - 2048;
                    if (lvl == -2048) lvl = -2047;
                    if (lvl == 0)     lvl = 1;
                }
                
                /* If there's not enough room for this symbol and 
                   its run of zeros, pick EOB instead. */
                if (run >= (tot_coef - r))
                {
                    /* send EOB */
                    run = lvl = -1;
                    r = tot_coef + 1;
                } else
                {
                    /* there was room, so advance our position */
                    r += run + 1;
                }
                
                /* insert the desired symbol into our list */
                input[i++] = (run << 16) | (lvl & 0xFFFF);
            }
        } 
    }
            
    r = encode_bitstream(input,i,bitstream,vld_tbl,e);

    /* make next_wptr random to test circular buffer */
    next_wptr = rand() & (BSBUF_SIZE-1);  

    /* copy into aligned buffer */
    for (k=0; k<6*64; k++) 
       mpeg2_vld_cont->bsbuf[(next_wptr+k)&(BSBUF_SIZE-1)] = bitstream[k];
            
    *mb_type = intra; 
    mpeg2_vld_cont->next_wptr = next_wptr;
    mpeg2_vld_cont->intravlc = intra_vlc_format;
    mpeg2_vld_cont->dc_prec = rand() & 3; 
    mpeg2_vld_cont->cbp = cbp;
    mpeg2_vld_cont->quant_scale = (a=rand()) ? (a&31)*2 : 2; /* quant_scale_type=0 */
       
    for (i=0; i<NUM_MB*6*64; i++) 
            output_gen[i] = 0;

    for (pos=0, mb=0; mb<NUM_MB; mb++)
    {
        pos += dequant(input+pos, output_gen+(mb*6*64), 
                *mb_type, mpeg2_vld_cont->dc_prec, dc_pred, 
                cbp, mpeg2_vld_cont->quant_scale); 
    }

    return r;
}

