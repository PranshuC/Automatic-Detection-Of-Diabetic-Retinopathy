/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_pix_sat_d.c -- Example file for IMG_pix_sat                     */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C62x IMGLIB           */
/*      IMG_pix_sat function. All inputs to the function contain random     */
/*      data. The assembly version of IMG_pix_sat is called and the         */
/*      output is compared with the reference C code using mem_compare()    */
/*      in support.c.                                                       */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
    
#include <stdio.h>
#include <stdlib.h>

/* Header file for the memory compare function */
int mem_compare(const void *ptr1, const char *name1,
                const void *ptr2, const char *name2, int len);

/* Header file for the DSPLIB function */
#include "IMG_pix_sat.h"

/* Header file for the C function */
void IMG_pix_sat_c(int n, short * in_data, unsigned char * out_data);

/* ======================================================================== */
/*  IMGLIB function-specific alignments. Refer to the                       */
/*  TMS320C62x IMG Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(in_data, 4);
#pragma DATA_ALIGN(out_data_asm, 2);
#pragma DATA_ALIGN(out_data_c, 2);



/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define N    (1024)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int     test = 0;

short  in_data[N] = 
{
    -0x247A, -0x7DCF, -0x431F,  0x3342,  0x0F00, -0x1ABB,  0x6872, -0x0A3B,
     0x2BA7, -0x1BD4, -0x0CF8, -0x2B19,  0x1D1A,  0x0B56,  0x4C10,  0x5BB6,
     0x4735, -0x3387, -0x5AD1, -0x7DBC,  0x5EF0,  0x347D, -0x1E34,  0x37A3,
    -0x45A3, -0x3500, -0x29A7, -0x6830, -0x2258, -0x141D, -0x2AC7, -0x0DF2,
    -0x31BC, -0x2EF0, -0x4108, -0x2C8F, -0x5F4A, -0x5A1A, -0x3018,  0x755E,
    -0x608A, -0x7F1F, -0x3140,  0x6720, -0x7354,  0x1260,  0x6321,  0x1BDE,
    -0x6042, -0x0BB3,  0x140F,  0x7209, -0x7C12, -0x505A, -0x279D,  0x0FF9,
     0x56C3,  0x1BAD, -0x58CD, -0x0CBA, -0x3304, -0x7B57,  0x7E90, -0x26F7,
     0x6214, -0x3EC2,  0x4989, -0x1266, -0x2744,  0x1F79,  0x2E59, -0x084E,
     0x2FC5, -0x3C55,  0x076C,  0x11BF, -0x752D,  0x2E60,  0x5433,  0x04B7,
     0x44E8, -0x37A4, -0x5147,  0x5B26,  0x0CD3,  0x160D,  0x4147,  0x63FD,
    -0x777C, -0x3709,  0x0B1E, -0x07D4, -0x5297,  0x19E2, -0x1537,  0x578A,
     0x4888, -0x52F0, -0x6F79,  0x293B, -0x12DD,  0x4A44,  0x237C,  0x27EE,
     0x6071, -0x1F9A, -0x3B5E,  0x1689, -0x7830, -0x570C,  0x50A0,  0x47E6,
     0x66FA,  0x6928, -0x2506,  0x308F,  0x017E,  0x2ED4,  0x1317,  0x739C,
     0x5E4A, -0x3E58,  0x6221,  0x57B7, -0x177E, -0x69C2, -0x0DA7,  0x6257,
     0x0570,  0x009E, -0x76EE, -0x14DC, -0x262A, -0x3712, -0x084E, -0x13DB,
    -0x7E0E,  0x5B95,  0x2979, -0x2304, -0x1ABC, -0x4F23,  0x7B59,  0x7A73,
    -0x339B,  0x3E4D, -0x5BAD,  0x080B,  0x2500,  0x53E0, -0x3DCD,  0x6258,
     0x15B3,  0x7E09,  0x5156,  0x7F27, -0x4EC1,  0x72A7, -0x2C9D, -0x3F29,
    -0x1A24,  0x2E7E, -0x6265, -0x40C9,  0x7B91,  0x00ED,  0x0A67, -0x5DCD,
    -0x55A0, -0x5356, -0x41C3, -0x5889,  0x227F,  0x0CE8,  0x282B, -0x47F8,
     0x2A6C,  0x5C66,  0x5722, -0x68FD,  0x0ACD,  0x66BB,  0x4D53,  0x1DB8,
     0x1449,  0x5A73, -0x087B,  0x3A82, -0x48D6,  0x7DB2,  0x20FB, -0x6296,
    -0x29EF,  0x28FC,  0x3FBE, -0x78A4, -0x4683, -0x7FEE, -0x5A8B,  0x62CF,
    -0x3303, -0x0B04, -0x3B83, -0x4158,  0x332C, -0x50F6, -0x6C5C,  0x718A,
    -0x5403, -0x5941,  0x24C9, -0x544C,  0x5DDD,  0x41DC, -0x0EAD, -0x58D5,
     0x2944,  0x4ABE, -0x7AC3, -0x0D23,  0x3725, -0x1D76,  0x7052,  0x2D69,
    -0x4728, -0x2DA2, -0x37B5, -0x19DE,  0x137F,  0x259B, -0x52F6, -0x1F97,
     0x49CF,  0x54DA,  0x01DC,  0x6BCF,  0x66D1, -0x45C6, -0x56E5,  0x6E97,
     0x1F25,  0x2C4A,  0x583B,  0x305F, -0x18B4,  0x3AB8,  0x02CF,  0x25EF,
    -0x5C6F,  0x681D,  0x6509,  0x0EB3, -0x18B8, -0x3631,  0x533A, -0x3BC5,
    -0x6F3E, -0x1B55,  0x31FF,  0x728B,  0x4126, -0x6B95, -0x3C76,  0x5436,
    -0x6049,  0x5049, -0x7DBD,  0x5D47,  0x75B2,  0x6676,  0x4A5F, -0x77A5,
     0x59BA,  0x3C51, -0x4780,  0x26D5, -0x2210,  0x282C,  0x69F4,  0x4677,
    -0x5657, -0x2523, -0x51CD, -0x043B,  0x3A62,  0x27E9,  0x0D92, -0x3FF5,
     0x33D3, -0x348D,  0x1F70,  0x1E85,  0x4008,  0x72CA,  0x3FF6, -0x08F9,
    -0x6760, -0x2A1E, -0x3428, -0x141F,  0x287D,  0x587F, -0x6186, -0x77BC,
     0x3FA6, -0x4D85,  0x5FA7,  0x27F9, -0x2597,  0x75BE,  0x461A,  0x76C2,
     0x67A9, -0x6D7D,  0x4195,  0x4E0A, -0x727F,  0x3651, -0x65EE, -0x0065,
    -0x2DEF,  0x0822, -0x3C59,  0x7092,  0x6607, -0x14BF, -0x68A9, -0x70AF,
    -0x70D7,  0x565B,  0x0A49, -0x4278,  0x1B63, -0x232E, -0x1FE0, -0x42EF,
    -0x00A1, -0x0373, -0x3F52, -0x1852,  0x2228, -0x45CC,  0x3796,  0x673C,
    -0x77C7,  0x66DA,  0x1A01, -0x79D2, -0x5DDC,  0x144F,  0x6AF3,  0x3CB1,
     0x007F,  0x7D52,  0x11F0, -0x0DE5, -0x0C44,  0x0405, -0x5462, -0x6B04,
     0x3D65, -0x7ED5, -0x5012,  0x4FB7,  0x20F9, -0x74B5,  0x383B, -0x353F,
    -0x0AF9,  0x5B6E,  0x33D4,  0x37B1, -0x620C,  0x63F6, -0x3AA8,  0x6596,
     0x78B8,  0x045E,  0x61B3, -0x06EA,  0x057B,  0x0FAA,  0x7202, -0x1D59,
    -0x69FD,  0x3CAA, -0x32AC,  0x60F2, -0x6A52,  0x2F82,  0x6F95,  0x5766,
    -0x6BEB,  0x09E3,  0x0532, -0x2BCA, -0x4589, -0x25F9,  0x1A1A,  0x14DE,
    -0x3E4F, -0x2708, -0x6321,  0x03E2,  0x1363, -0x292E,  0x74C4, -0x01E3,
    -0x07C8,  0x378B, -0x59A3,  0x56FF, -0x36BA,  0x14D2, -0x0579, -0x3B71,
     0x15B1,  0x4BBA, -0x16B4,  0x207D,  0x75B4, -0x5394, -0x7BBC, -0x34D5,
     0x29FC,  0x1399,  0x3AD4,  0x06A8,  0x3DD8,  0x0DEF, -0x5C47,  0x73A5,
     0x0156,  0x7B07, -0x582A, -0x228D,  0x1DCB, -0x4F70, -0x03B9, -0x7EF6,
     0x7CB4,  0x1098, -0x2DC8,  0x4D98,  0x1BCC, -0x2F03, -0x5468,  0x597E,
    -0x18FA, -0x1EA3, -0x44E2,  0x7614,  0x3D18,  0x37F4,  0x2D5F,  0x4E24,
    -0x5654, -0x4FF9,  0x5E7B,  0x4766,  0x0048,  0x1E87,  0x7FBA, -0x7877,
     0x46F0,  0x2F51,  0x15A9,  0x7E90,  0x5F35,  0x7CD5,  0x233C, -0x40AD,
    -0x27DB,  0x600E,  0x56B1, -0x2246,  0x4266, -0x1048,  0x6F49, -0x2B94,
     0x137F, -0x62D7, -0x7692, -0x09FA, -0x6C12,  0x7683,  0x3030,  0x1427,
    -0x2396, -0x0439, -0x28F1, -0x4DDC,  0x376E, -0x019D, -0x2695,  0x4594,
    -0x5BC3, -0x177F, -0x5BB7, -0x3317,  0x75BC, -0x7F1A, -0x4282, -0x5086,
    -0x4637,  0x0DD1,  0x0522, -0x45DE,  0x6A2C,  0x0B6B, -0x74C8, -0x3FCB,
    -0x22D8,  0x3855, -0x1B7E, -0x3E30, -0x60D8, -0x4865,  0x1885, -0x69FF,
    -0x4E73,  0x473A, -0x378C,  0x7EE9, -0x1BE9, -0x3123, -0x3880,  0x3328,
    -0x7E59, -0x7DE7,  0x07B2, -0x1D53,  0x1FF0,  0x0BB6,  0x524E,  0x52D4,
     0x6845, -0x3E03,  0x281E,  0x5BBB, -0x24D0, -0x37A9, -0x7EFA, -0x7FCB,
    -0x2875,  0x28B4,  0x63B3, -0x0A03,  0x04E0,  0x753A,  0x5A1D, -0x68D1,
    -0x1E0B, -0x1174, -0x10F9,  0x50C9,  0x65AB,  0x767A, -0x3F20, -0x2B62,
     0x05E2, -0x2E6B,  0x766A, -0x678A,  0x2EAE, -0x322F, -0x1613,  0x79D3,
     0x6306,  0x7236,  0x46C8,  0x23F7,  0x03DE, -0x7476,  0x3A23,  0x4407,
    -0x2B79, -0x35D9,  0x2693, -0x13C9, -0x26A2,  0x2565, -0x16E7,  0x3AD4,
    -0x6D02,  0x3161,  0x7A74, -0x1904,  0x210A,  0x2B83, -0x69E8, -0x38D1,
    -0x465C,  0x725B,  0x50E9,  0x0D98, -0x5513, -0x436A,  0x767E, -0x343C,
     0x376D,  0x43D8,  0x22C3,  0x32B6, -0x1D14,  0x683B, -0x1078, -0x0C2B,
     0x579A,  0x731E, -0x49C1, -0x4F00,  0x1F52, -0x2FC9, -0x6116, -0x5695,
    -0x6D7C, -0x4851,  0x4B5C, -0x5D4F, -0x2EC7,  0x5C57,  0x78F5,  0x14E2,
    -0x25F1, -0x2E1E,  0x5578, -0x5194, -0x49F5, -0x01B9,  0x70A3, -0x5668,
     0x3048,  0x3C89,  0x6C33,  0x522F,  0x5266, -0x4BD0,  0x62C5,  0x7ED1,
    -0x1DFA,  0x0E97, -0x439A,  0x3D9B,  0x32CE, -0x14ED,  0x1945, -0x304B,
    -0x6797,  0x242C,  0x384B,  0x7D39, -0x1995, -0x790F,  0x71B2,  0x2EAB,
    -0x2FBE, -0x1150, -0x0BFF,  0x44F6, -0x4AF1,  0x5D74,  0x0B63,  0x5D1D,
     0x520E,  0x5DB6, -0x57BC, -0x7C86, -0x56AC, -0x447C, -0x3402,  0x2B44,
    -0x4697, -0x5FD7,  0x0582, -0x0C5A, -0x428E,  0x0B60,  0x7691, -0x6298,
     0x1531,  0x45EF, -0x4682, -0x2A01, -0x7220,  0x751E, -0x09AD,  0x2480,
     0x5491,  0x4D8E,  0x2E4D,  0x7F88,  0x2363, -0x38CF, -0x6D36,  0x1936,
     0x2167, -0x778F, -0x3532, -0x4EEE,  0x3F76,  0x2B72,  0x1B3B,  0x4EBF,
    -0x6DC3,  0x538D,  0x5FBC, -0x2629, -0x5AA8, -0x329D,  0x715C,  0x5F16,
     0x1316,  0x5BB4, -0x6E04, -0x623A, -0x33DA,  0x6EF0, -0x0D65,  0x7FCB,
    -0x3416, -0x4E77, -0x74CD, -0x7B38, -0x4D74,  0x4FAB,  0x70CC,  0x4F9C,
    -0x4A80, -0x35CC,  0x0A89,  0x03F3,  0x4A9C, -0x6875, -0x055A, -0x0B1F,
    -0x5345,  0x7232, -0x2EF5,  0x196F,  0x6F30, -0x6A5A,  0x6B51, -0x482C,
    -0x61D0, -0x06C3, -0x1581,  0x70B8,  0x4323, -0x431E, -0x25DB,  0x5BAB,
     0x41DE,  0x0432, -0x52C2, -0x0659,  0x2756,  0x4289,  0x55EB, -0x3377,
    -0x6261,  0x740B,  0x6626,  0x49BB, -0x67E7, -0x491B,  0x0E1D, -0x02BC,
    -0x2450, -0x1CB0, -0x630B,  0x40D6, -0x51E0, -0x6B84, -0x29D8, -0x77E1,
     0x3B25,  0x025F,  0x63AF,  0x02C4,  0x068A,  0x2B2F,  0x431B,  0x27A3,
    -0x6129, -0x0552, -0x14C6,  0x14C5,  0x68F8, -0x4357,  0x071C,  0x33F1,
    -0x0073, -0x6C5F,  0x5ECA,  0x1863, -0x543A, -0x433E,  0x02B4,  0x2A00,
    -0x024E, -0x69FB, -0x666E, -0x451A, -0x1D2F, -0x6EEF, -0x7DDE, -0x7586,
    -0x1020,  0x3837, -0x66D5,  0x76C8,  0x111D, -0x1120, -0x7CEE, -0x66E9,
     0x7AF5, -0x1104, -0x75C6,  0x2EBF,  0x53BA,  0x4AB1,  0x4E6E,  0x40AD,
    -0x50C4,  0x40B4,  0x66AD, -0x0A08, -0x52D7,  0x6322, -0x2478, -0x67B2,
     0x5943,  0x70F7,  0x6D36,  0x165E,  0x5A43,  0x0098,  0x587D, -0x19B3,
     0x4ECE, -0x1607, -0x6219,  0x1303, -0x22B7,  0x6A11, -0x2F79,  0x5A31,
     0x2F17,  0x7B0D,  0x0E8C,  0x52F1, -0x307A,  0x16ED, -0x6C0F,  0x5C28,
     0x5D3E,  0x466D, -0x7630,  0x31EB, -0x3CD2,  0x1BD9,  0x097F,  0x32C5,
     0x745E,  0x2BF5,  0x0C49, -0x7AF6, -0x6E37, -0x1EFA, -0x1496,  0x667F,
     0x1A2C,  0x37ED, -0x17A7, -0x28A4, -0x6A5C,  0x68E8,  0x6977, -0x2C62,
    -0x787A,  0x4B18,  0x1221,  0x4F19, -0x6DFD,  0x1710, -0x69A2,  0x0808,
    -0x4C7B, -0x56E6, -0x3B1B, -0x2296,  0x3DFB, -0x1A78, -0x3D9B, -0x7247,
    -0x3A17, -0x4ADD,  0x17DD,  0x1E13, -0x7330, -0x1615, -0x7EF6, -0x4AB4,
    -0x102A,  0x7112,  0x3FF7,  0x71EF,  0x5EB5,  0x36C2,  0x3FA0, -0x415F,
    -0x5C2E, -0x6246,  0x45DF, -0x348B,  0x3095, -0x2DC4, -0x29FF, -0x6FAC,
    -0x441C,  0x79D6, -0x6881,  0x5F45,  0x205C,  0x551E,  0x20E7, -0x45CD,
    -0x176D, -0x7CD4, -0x759A,  0x63FB, -0x5130,  0x73D5, -0x1219,  0x2DDD,
     0x7C24,  0x0EDC,  0x0886,  0x146C,  0x48A5,  0x2E15, -0x5222, -0x24E4,
    -0x76AE,  0x0154, -0x0F96, -0x273B, -0x1E16,  0x19EB,  0x1066, -0x6269,
     0x0AE7, -0x7377,  0x4441,  0x30E4, -0x6CB4,  0x3D3D, -0x790A, -0x77FB,
    -0x5367,  0x61F3,  0x422C, -0x7F38, -0x7100, -0x492E, -0x1D93, -0x46EB,
    -0x38B1, -0x455F, -0x5F46, -0x473F,  0x513F,  0x2688, -0x59D7,  0x5AD1,
    -0x1D35, -0x1BC9, -0x6622, -0x7846, -0x7F8E,  0x6222,  0x6F3F, -0x0082,
     0x3A65, -0x7826,  0x4C94,  0x2FC7, -0x59EF, -0x7DDF, -0x39FF, -0x4A7E,
    -0x6308,  0x344B, -0x2610, -0x2822,  0x4F3B, -0x347F,  0x17E7,  0x4D22,
     0x03DC,  0x0329,  0x17A7,  0x2A90,  0x0EC8,  0x49BE, -0x0946,  0x75A1,
     0x63CC,  0x3009,  0x37F7,  0x0258,  0x5055,  0x1481, -0x21EB,  0x707A,
     0x3863, -0x3B85, -0x6193,  0x1F10, -0x0E8B, -0x658D, -0x6B65, -0x6EB8
};

unsigned char  out_data_asm[N];
unsigned char  out_data_c[N];

int main()
{   

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG62x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_pix_sat(N, in_data, out_data_asm);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_pix_sat_c(N, in_data, out_data_c);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(out_data_asm, "out_data_asm", out_data_c, "out_data_c", sizeof(out_data_c));
    if (test == 0) 
    {
      printf("out_data_asm vs. out_data_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  img_pix_sat_d.c                                           */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
