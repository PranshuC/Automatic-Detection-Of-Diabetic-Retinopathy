/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      IMG_perimeter_d.c -- Example file for IMG_perimeter                 */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C62x IMGLIB           */
/*      IMG_perimeter function. All inputs to the function contain random   */
/*      data. The assembly version of IMG_perimeter is called and the       */
/*      output is compared with the reference C code using mem_compare()    */
/*      in support.c.                                                       */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Header file for the memory compare function */
int mem_compare(const void *ptr1, const char *name1,
                const void *ptr2, const char *name2, int len);

/* Header file for the DSPLIB function */
#include "IMG_perimeter.h"

/* Header file for the C function */
void IMG_perimeter_c(unsigned char * in_data, int cols, unsigned char * out_data);

/* ======================================================================== */
/*  IMGLIB function-specific alignments. Refer to the                       */
/*  TMS320C62x IMG Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(in_data, 8);

/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define COLS (720)
#define N    (COLS*3)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int     test = 0;
unsigned char  in_data[N] = 
{
     0xC5,  0xCC,  0xB4,  0x52,  0x5C,  0x78,  0xB4,  0x88,
     0x96,  0x4A,  0xBE,  0xBE,  0x40,  0x0C,  0x5E,  0xE2,
     0xFE,  0x1E,  0x76,  0x40,  0x84,  0x46,  0x92,  0x2E,
     0x12,  0x96,  0xEA,  0x13,  0x43,  0x89,  0x54,  0x85,
     0x22,  0xDE,  0x39,  0x43,  0x33,  0x58,  0x91,  0xF3,
     0x2C,  0x91,  0x50,  0x15,  0x1D,  0x8A,  0xEF,  0x57,
     0x15,  0x09,  0x4A,  0x09,  0xBC,  0xFC,  0xAB,  0xF4,
     0x4A,  0x70,  0x16,  0xAC,  0x36,  0xB4,  0x4A,  0x73,
     0xF1,  0xD6,  0x8E,  0xC1,  0x23,  0x7B,  0xB5,  0x2A,
     0x02,  0xA7,  0xDB,  0x46,  0x67,  0x5B,  0x18,  0x39,
     0x4A,  0xDA,  0xD9,  0x9E,  0xCC,  0x57,  0x54,  0x62,
     0x8F,  0x4A,  0x97,  0x4D,  0xEC,  0xF4,  0x13,  0x9C,
     0xC7,  0x4E,  0x79,  0xE8,  0x88,  0xCE,  0xA0,  0xD9,
     0xA6,  0x8C,  0x58,  0x40,  0x17,  0x94,  0x70,  0x31,
     0x79,  0x98,  0x1F,  0x50,  0x27,  0x3E,  0xEF,  0x29,
     0xBD,  0x8E,  0xAE,  0x39,  0x9E,  0x2C,  0x75,  0xD7,
     0x9C,  0x3D,  0x8E,  0x63,  0x0F,  0x3C,  0x8E,  0xB6,
     0xFD,  0xE7,  0xD2,  0xE3,  0xA8,  0x81,  0x94,  0x16,
     0xA3,  0xDF,  0xD3,  0x63,  0x8E,  0x7A,  0x6A,  0x9B,
     0x73,  0x7C,  0xE4,  0x41,  0xE2,  0x12,  0xAE,  0x8F,
     0xBF,  0xF3,  0xE8,  0xD5,  0xE0,  0xC2,  0xE3,  0xBD,
     0x47,  0xA0,  0xC4,  0xBD,  0x44,  0x53,  0x5B,  0x18,
     0x4A,  0x47,  0xF9,  0x8B,  0x23,  0xAB,  0x50,  0xB2,
     0x04,  0x89,  0x5C,  0x61,  0x58,  0x0C,  0x5E,  0x6F,
     0xB5,  0x90,  0xBB,  0x05,  0xB5,  0xD5,  0xF8,  0x79,
     0x2E,  0x9B,  0x74,  0x31,  0x17,  0xB8,  0x9A,  0x8C,
     0xF3,  0x8B,  0x88,  0x62,  0xAE,  0xBD,  0x77,  0x47,
     0xC2,  0x12,  0x51,  0x06,  0xBF,  0x27,  0x3B,  0xFE,
     0x7D,  0x74,  0x2F,  0x4D,  0x75,  0x8A,  0xD8,  0xDC,
     0x5B,  0x08,  0xBD,  0x70,  0x3A,  0xDB,  0xE2,  0xEF,
     0x5A,  0xDE,  0xA9,  0x77,  0xA7,  0x61,  0xE0,  0x28,
     0x09,  0xEA,  0x4A,  0x68,  0xDF,  0xBF,  0x03,  0xA4,
     0xD4,  0x23,  0x20,  0xC8,  0xCB,  0x0D,  0x54,  0x2F,
     0x23,  0xCD,  0xD0,  0xE5,  0xC0,  0x50,  0xCC,  0x9C,
     0xCE,  0x04,  0x07,  0x32,  0x81,  0x20,  0x2D,  0x7C,
     0x9F,  0x41,  0x8C,  0x98,  0xCC,  0x4B,  0x3C,  0xAD,
     0xCF,  0x2A,  0x21,  0xBF,  0x3B,  0xD9,  0x07,  0x8B,
     0xB7,  0x8A,  0x66,  0x71,  0x5C,  0x69,  0x45,  0xB2,
     0x40,  0x9F,  0xB7,  0x6F,  0xA3,  0x62,  0x48,  0xF8,
     0xE2,  0x62,  0x31,  0x9B,  0x14,  0xD9,  0x2B,  0xB4,
     0x75,  0x75,  0x65,  0xE1,  0x32,  0x43,  0xBC,  0x67,
     0x6C,  0xB1,  0xF1,  0x16,  0x22,  0x6C,  0x60,  0xA4,
     0x7A,  0x31,  0x34,  0xEF,  0xAF,  0x11,  0xAB,  0x65,
     0x3A,  0xCD,  0x85,  0x33,  0x3C,  0xAA,  0xA3,  0x01,
     0xBB,  0xA5,  0xF1,  0xAA,  0x93,  0xC5,  0xB2,  0xBA,
     0x7F,  0x40,  0x37,  0x79,  0xEE,  0x3E,  0x2B,  0x09,
     0x04,  0x5F,  0xEF,  0x60,  0x7C,  0x35,  0xC8,  0xF6,
     0x65,  0x26,  0x0C,  0xC9,  0x22,  0x7E,  0x06,  0x58,
     0x6F,  0xD7,  0xA9,  0x9F,  0x43,  0x99,  0x52,  0x26,
     0xFE,  0x8E,  0xE7,  0xC9,  0xDE,  0xEE,  0x10,  0x0A,
     0x38,  0xEA,  0xF4,  0x5E,  0x4E,  0x26,  0x20,  0xF5,
     0x02,  0x85,  0xC2,  0xEB,  0xB8,  0x07,  0x1A,  0xBC,
     0x95,  0x6F,  0xB0,  0xBB,  0x32,  0x17,  0xBE,  0xBC,
     0x22,  0xAF,  0xA3,  0x89,  0x6D,  0xAC,  0x32,  0x13,
     0x03,  0xDD,  0x28,  0xF8,  0xCE,  0xBD,  0x74,  0x35,
     0xF6,  0x92,  0x0C,  0xFF,  0x9F,  0x03,  0x18,  0x9C,
     0xAB,  0x51,  0x9C,  0xA8,  0x5D,  0x61,  0xCE,  0xCE,
     0xD2,  0xF7,  0x00,  0xB7,  0xAE,  0xAC,  0x56,  0x28,
     0xCE,  0x78,  0x95,  0x36,  0x3B,  0x18,  0xA5,  0xE2,
     0x14,  0x07,  0xE9,  0x84,  0x1F,  0x82,  0x89,  0xFB,
     0x5A,  0x1A,  0x04,  0x32,  0x51,  0xD5,  0xE9,  0x4D,
     0x9C,  0x10,  0xF9,  0xD5,  0x1E,  0x6B,  0xCD,  0x74,
     0x1A,  0x56,  0x52,  0x1A,  0x60,  0x59,  0xB1,  0x18,
     0x97,  0x04,  0xC8,  0x81,  0xF6,  0xD3,  0x5F,  0xFF,
     0x1D,  0xCE,  0xDB,  0x7D,  0xAC,  0xC7,  0x4C,  0x42,
     0xC8,  0x30,  0x54,  0x56,  0x98,  0xE5,  0xE2,  0x7E,
     0x49,  0x46,  0x7A,  0xB0,  0xB9,  0xFF,  0x5B,  0xD4,
     0xEB,  0xBB,  0x9E,  0x8E,  0x8D,  0x9A,  0xDB,  0x75,
     0x45,  0x83,  0x25,  0x80,  0x16,  0x0A,  0x31,  0xD1,
     0xF5,  0xE0,  0x87,  0xE8,  0xE4,  0x2D,  0x53,  0xA0,
     0x0E,  0x63,  0xBD,  0x8D,  0x67,  0x39,  0xF5,  0xAD,
     0x3B,  0xFD,  0xB0,  0x02,  0x04,  0x91,  0xB5,  0xCC,
     0xFD,  0x80,  0x86,  0x39,  0x3B,  0xBB,  0x5A,  0x4E,
     0xC2,  0xE5,  0x81,  0x24,  0x63,  0xF4,  0x4B,  0x1A,
     0x0F,  0xDD,  0x03,  0x5D,  0x4E,  0xCD,  0xCE,  0xE1,
     0x54,  0x15,  0x8D,  0xBD,  0x9C,  0x70,  0x05,  0xF5,
     0xA9,  0x5A,  0x17,  0x2A,  0x42,  0xD7,  0x86,  0xE4,
     0x04,  0xB5,  0xFB,  0x17,  0x57,  0x72,  0x5C,  0x6B,
     0x33,  0x08,  0xA5,  0xEC,  0x01,  0x00,  0xC7,  0x3C,
     0x51,  0x83,  0xC0,  0x1C,  0x00,  0x7C,  0x7A,  0xBE,
     0x3F,  0x88,  0x21,  0x93,  0x3E,  0x1E,  0x46,  0x4B,
     0x0A,  0x15,  0xAE,  0x33,  0x4B,  0x40,  0x0B,  0x49,
     0x1D,  0x15,  0xBB,  0x32,  0xDD,  0x0B,  0x8E,  0x3D,
     0x98,  0x11,  0xBC,  0x82,  0xEA,  0x48,  0x4D,  0x19,
     0x08,  0xEB,  0x21,  0xEA,  0x1F,  0xB9,  0xB9,  0x13,
     0x6D,  0x44,  0x08,  0x20,  0x14,  0x98,  0x52,  0xD6,
     0x54,  0x12,  0xFD,  0xDD,  0xED,  0x9A,  0xF9,  0x21,
     0x9C,  0xBC,  0x72,  0x0C,  0xA3,  0x11,  0x7D,  0xD7,
     0x4C,  0xDC,  0xA7,  0x1F,  0x9E,  0x2E,  0xAE,  0xFF,
     0xED,  0x44,  0xD0,  0xEB,  0xF5,  0x76,  0xB2,  0x81,
     0x98,  0x94,  0xBE,  0xDE,  0x02,  0xD0,  0x0F,  0x30,
     0xF3,  0xF8,  0x24,  0xE3,  0xE0,  0x51,  0xCF,  0x50,
     0x6E,  0x22,  0x8A,  0x11,  0xA8,  0x18,  0xA8,  0xEC,
     0xDF,  0xBA,  0x45,  0x19,  0x55,  0x31,  0x48,  0x1A,
     0xDF,  0xAB,  0x76,  0x7A,  0x8D,  0x59,  0x29,  0xE0,
     0x39,  0xCF,  0xEE,  0xAA,  0x84,  0xF0,  0x19,  0x80,
     0xF6,  0x71,  0xEC,  0xD2,  0xC4,  0x2E,  0x4B,  0xCD,
     0xA0,  0xE9,  0x2A,  0x1A,  0xA3,  0xF2,  0x5F,  0x93,
     0x8E,  0x7E,  0x66,  0x56,  0xB3,  0x01,  0xC8,  0x4D,
     0x3D,  0x58,  0x7D,  0x97,  0xB1,  0xA5,  0x44,  0x15,
     0x06,  0xD5,  0x95,  0x25,  0xF8,  0x0E,  0xAC,  0x3B,
     0x9C,  0x43,  0x50,  0x99,  0xC3,  0x97,  0x14,  0xF8,
     0x49,  0x31,  0xCD,  0x0A,  0x1F,  0x53,  0x7F,  0x79,
     0xFB,  0x04,  0x81,  0xA3,  0xC2,  0xFD,  0xA5,  0xE8,
     0xB9,  0xA0,  0x2D,  0xBE,  0xA8,  0x54,  0x04,  0x10,
     0x5D,  0x1A,  0x0F,  0x39,  0xAA,  0x7E,  0x72,  0x13,
     0x04,  0x72,  0x35,  0xAE,  0x15,  0xC8,  0x92,  0xE7,
     0xE3,  0x49,  0x52,  0x5F,  0xA0,  0x8C,  0x08,  0x70,
     0xF5,  0x4A,  0xB8,  0xC6,  0xF7,  0x78,  0x4B,  0x96,
     0x2F,  0xDE,  0x33,  0x20,  0x8A,  0xBC,  0xA0,  0x43,
     0xB5,  0x50,  0x54,  0xEE,  0x39,  0xF8,  0xAB,  0xFF,
     0xF1,  0x23,  0x4A,  0xEF,  0xE9,  0xC7,  0x14,  0x9D,
     0x27,  0xCE,  0xF6,  0x19,  0x1C,  0xF6,  0x36,  0xC6,
     0xC5,  0x81,  0xE5,  0xF8,  0x23,  0x3B,  0x87,  0x07,
     0x3A,  0xBD,  0xFF,  0x20,  0x9E,  0x54,  0x42,  0xC7,
     0x0D,  0xEB,  0x54,  0x9D,  0x87,  0x03,  0x61,  0xAE,
     0x6E,  0xFF,  0x15,  0xCE,  0x43,  0x3E,  0x20,  0x1C,
     0x75,  0x6B,  0x01,  0xBF,  0xAF,  0x40,  0x86,  0xB2,
     0xE5,  0x56,  0xBA,  0xD2,  0x87,  0xF0,  0x22,  0x6B,
     0x3C,  0x10,  0x2F,  0x90,  0x12,  0x26,  0xE0,  0x6D,
     0xAD,  0x54,  0x32,  0x79,  0x58,  0xEF,  0x49,  0x73,
     0x7E,  0x89,  0x68,  0x05,  0xE5,  0xDB,  0xA8,  0xD8,
     0x38,  0xD5,  0x53,  0x66,  0x6E,  0xCE,  0x8F,  0x1B,
     0x18,  0x21,  0xEB,  0x2D,  0x9A,  0x79,  0x03,  0x62,
     0x12,  0xB1,  0x39,  0xB2,  0x61,  0xDB,  0x0F,  0x7F,
     0xD9,  0xFA,  0xBE,  0x04,  0xA3,  0xC2,  0xDF,  0xE8,
     0x56,  0xCB,  0x19,  0x22,  0xC5,  0x38,  0x9E,  0xB3,
     0x1E,  0xB1,  0x12,  0x87,  0x49,  0x25,  0x2B,  0x80,
     0x83,  0xA4,  0x8E,  0x5A,  0xC6,  0x50,  0x79,  0x48,
     0x17,  0xCC,  0x2B,  0x19,  0xB0,  0x12,  0xC6,  0x78,
     0x91,  0x0B,  0x26,  0x49,  0xEF,  0x9D,  0x26,  0xB7,
     0x3F,  0x98,  0x0C,  0x82,  0x67,  0xD9,  0xD9,  0xA6,
     0x7E,  0xA2,  0x14,  0x23,  0x24,  0x2F,  0xB2,  0x47,
     0xD3,  0x5B,  0x7E,  0x22,  0x18,  0x77,  0x62,  0xC4,
     0xB3,  0x4B,  0x3D,  0xB5,  0xFF,  0x7B,  0xDC,  0x20,
     0x49,  0x07,  0x3B,  0x0D,  0x33,  0x1C,  0x28,  0xB8,
     0xE5,  0xB3,  0x0A,  0xDB,  0x0F,  0x49,  0xE6,  0x86,
     0x24,  0xC3,  0x5B,  0x3A,  0xAB,  0x48,  0x6F,  0xD8,
     0x6D,  0x88,  0xBE,  0x6E,  0xAC,  0xD6,  0x12,  0x7E,
     0x91,  0xFE,  0x45,  0xFC,  0x21,  0xC6,  0x0A,  0x8B,
     0x2D,  0x22,  0x33,  0xF0,  0x5D,  0x86,  0x04,  0x59,
     0xAC,  0xED,  0xC8,  0x59,  0x15,  0xF9,  0xB1,  0xF0,
     0x76,  0x9A,  0x30,  0xC4,  0x09,  0xAD,  0x9A,  0xE1,
     0x72,  0x90,  0x14,  0x0E,  0xF1,  0x1E,  0xC4,  0x4F,
     0x8E,  0x9E,  0xDC,  0xC2,  0x67,  0x54,  0xE4,  0x25,
     0xA3,  0xB5,  0xC0,  0x4E,  0x13,  0x8E,  0xC5,  0x8B,
     0xD3,  0x83,  0xDD,  0x6D,  0x71,  0x02,  0x54,  0x20,
     0x1B,  0x88,  0x13,  0x83,  0x21,  0x27,  0x3F,  0xE6,
     0x8A,  0x56,  0x38,  0x27,  0xC7,  0xE3,  0x54,  0xBA,
     0x7C,  0xAB,  0x36,  0xA3,  0x49,  0x5D,  0xF8,  0xAA,
     0xD4,  0xFC,  0x06,  0x1B,  0x3D,  0xE7,  0xD5,  0xE2,
     0x1E,  0xE5,  0x14,  0xE3,  0x42,  0x94,  0x87,  0xAF,
     0x71,  0x93,  0x86,  0xBD,  0x19,  0xA2,  0x87,  0xB9,
     0x95,  0xE7,  0x03,  0xDB,  0x2F,  0xD1,  0xAF,  0xBF,
     0x31,  0xB3,  0x29,  0x1F,  0xA7,  0x13,  0xC4,  0x09,
     0x5E,  0xAD,  0x76,  0x5E,  0xA7,  0xE3,  0x29,  0x42,
     0x49,  0x3F,  0x99,  0x75,  0x30,  0x8F,  0x6E,  0x94,
     0x6C,  0x7F,  0x5C,  0x98,  0xA0,  0x84,  0xBD,  0xD4,
     0xE4,  0xCC,  0xE1,  0x84,  0x79,  0x82,  0x48,  0xDF,
     0xB5,  0x9C,  0xF5,  0x4A,  0xED,  0x23,  0xBD,  0x96,
     0xAC,  0xF6,  0x2E,  0xE1,  0x53,  0xC5,  0x72,  0x95,
     0xE9,  0x0B,  0x2A,  0x18,  0x86,  0x4C,  0x57,  0x5E,
     0x22,  0x0F,  0xD6,  0x30,  0xEE,  0xA0,  0x2E,  0x9D,
     0xFB,  0x28,  0xBE,  0x4D,  0xE6,  0xE5,  0x4A,  0x23,
     0x03,  0xFF,  0x98,  0xC8,  0xE9,  0xDF,  0x10,  0x7F,
     0x32,  0xCF,  0xE7,  0x9C,  0x1B,  0xB0,  0xF8,  0x41,
     0x09,  0x57,  0x44,  0x80,  0x57,  0x75,  0x27,  0xBE,
     0xCE,  0x63,  0x25,  0x9E,  0x5E,  0xD0,  0xDD,  0x6E,
     0xC5,  0xCC,  0x3F,  0x70,  0x33,  0x99,  0xE4,  0x35,
     0x90,  0xF0,  0x60,  0x2E,  0x53,  0x3F,  0xDA,  0x86,
     0x78,  0xBB,  0x94,  0xF0,  0xFB,  0x63,  0xB1,  0xA7,
     0xA9,  0x33,  0xA2,  0xE7,  0x3F,  0x65,  0xF6,  0x83,
     0x0B,  0x65,  0x7B,  0x34,  0x71,  0x43,  0x8F,  0x02,
     0xD8,  0x4E,  0xFE,  0x45,  0xDC,  0x58,  0x6A,  0x42,
     0x96,  0xEC,  0x94,  0x35,  0xB0,  0xE9,  0x23,  0x1D,
     0xC7,  0x63,  0x78,  0x51,  0x90,  0x62,  0x2C,  0xEC,
     0x22,  0x5E,  0x3C,  0xD4,  0x14,  0x74,  0x09,  0xDF,
     0xE8,  0x6F,  0xF4,  0xCB,  0x39,  0xDB,  0x44,  0x2F,
     0x83,  0x4B,  0xB5,  0x65,  0xB6,  0xF0,  0x5E,  0x10,
     0x5C,  0x0D,  0x92,  0x2A,  0xCF,  0x62,  0x9E,  0x8C,
     0x84,  0xB1,  0x0A,  0x0C,  0x61,  0x90,  0xD0,  0x5B,
     0xB4,  0x07,  0xEA,  0x06,  0x82,  0x39,  0x34,  0x73,
     0xBB,  0x3B,  0xEA,  0x9C,  0x4F,  0x1E,  0xA5,  0x2F,
     0x8C,  0xDD,  0x1A,  0xA9,  0x2F,  0x0B,  0xA8,  0xF1,
     0xBE,  0x23,  0xDE,  0xE6,  0x18,  0x8E,  0x87,  0x89,
     0x54,  0x9B,  0x9C,  0xE2,  0x38,  0x7C,  0xBF,  0x1C,
     0x9D,  0x2F,  0x99,  0x9D,  0x91,  0xB6,  0x14,  0xD8,
     0xBE,  0xB2,  0x44,  0x5D,  0x2A,  0x74,  0x56,  0x97,
     0xC1,  0x99,  0x48,  0x4D,  0x97,  0x8C,  0x6E,  0x84,
     0xAF,  0xB3,  0xF0,  0x18,  0xBB,  0xAB,  0xAB,  0xC8,
     0xAA,  0xE6,  0x1A,  0xFA,  0xF7,  0x7E,  0x49,  0xCC,
     0xB0,  0x0C,  0xC5,  0xB4,  0xFB,  0x3C,  0x29,  0x9E,
     0x24,  0xE7,  0x6F,  0x6A,  0xF6,  0xAD,  0x4A,  0x00,
     0x23,  0x33,  0xF9,  0xBB,  0xDE,  0x1C,  0x10,  0x06,
     0xEA,  0x71,  0x59,  0x3B,  0x2B,  0x32,  0xA0,  0x45,
     0xDD,  0xE0,  0x8D,  0x25,  0x67,  0xB9,  0x97,  0x6A,
     0xD4,  0x97,  0x6A,  0x36,  0xA5,  0xB8,  0x2B,  0x75,
     0xC5,  0x41,  0x7A,  0xA3,  0x15,  0x32,  0x6F,  0xDC,
     0x01,  0x5D,  0xFD,  0x88,  0x97,  0xFC,  0xA4,  0xC8,
     0xB4,  0x3F,  0x2D,  0x82,  0x70,  0xDA,  0xCE,  0xBC,
     0xA8,  0x30,  0x63,  0xD5,  0x04,  0x0F,  0x97,  0x4C,
     0xAB,  0x4D,  0x56,  0x38,  0xC0,  0x00,  0x4A,  0x0C,
     0x6C,  0xAB,  0xB1,  0x4D,  0x26,  0x74,  0x1B,  0x5D,
     0x16,  0x54,  0x70,  0xDE,  0x5D,  0x2E,  0x48,  0xB6,
     0x6D,  0x6B,  0x18,  0x30,  0x7B,  0x5D,  0x9F,  0xAE,
     0xC0,  0xAB,  0x77,  0x25,  0x17,  0x55,  0xC3,  0x7C,
     0x73,  0xF2,  0xFA,  0x6B,  0xCC,  0xAF,  0x3C,  0x88,
     0xA1,  0x3E,  0xB6,  0xA1,  0xA5,  0x98,  0x9A,  0x53,
     0xDB,  0xE9,  0x2F,  0x4C,  0x88,  0x99,  0x55,  0x55,
     0xAC,  0x49,  0xAD,  0x61,  0x2C,  0xB3,  0xC4,  0x5C,
     0x40,  0x3E,  0x28,  0x5D,  0x46,  0xD2,  0x87,  0x9B,
     0x4C,  0x47,  0x70,  0x35,  0x2F,  0x27,  0x08,  0xD0,
     0xF1,  0xE2,  0xE0,  0xEC,  0x55,  0xD3,  0x26,  0xF9,
     0x3D,  0x02,  0xFD,  0x22,  0x92,  0xAE,  0x22,  0x74,
     0xAC,  0x30,  0xD4,  0xE5,  0x9A,  0xAE,  0xC7,  0xD7,
     0xD0,  0xE7,  0x66,  0xD6,  0x76,  0x9C,  0x62,  0x86,
     0x48,  0x46,  0x6B,  0xE6,  0x3F,  0x3F,  0x9E,  0xC8,
     0xEE,  0xFF,  0xE8,  0x2C,  0x1B,  0x33,  0x50,  0x26,
     0x2E,  0xC8,  0x7D,  0xBD,  0xD0,  0xE8,  0xB0,  0xE0,
     0x57,  0x31,  0x23,  0x03,  0x09,  0x14,  0xAD,  0xB3,
     0xCE,  0xE1,  0xF5,  0xC6,  0xE4,  0x14,  0x0D,  0x71,
     0xEA,  0xBE,  0x29,  0x04,  0x36,  0x3D,  0x12,  0xA1,
     0x7F,  0x77,  0xB2,  0xC2,  0x6B,  0x28,  0xAC,  0xFC,
     0x01,  0x0B,  0x19,  0x13,  0xE4,  0x60,  0xF6,  0xB6,
     0x6A,  0xED,  0x25,  0x2A,  0x2B,  0xAF,  0x8F,  0x3F,
     0x26,  0x2B,  0x62,  0x85,  0x69,  0x6A,  0xB2,  0xC4,
     0x6B,  0xDD,  0x52,  0x20,  0x21,  0xD2,  0x46,  0xAD,
     0xB0,  0xC7,  0x02,  0x62,  0x2A,  0x84,  0x0F,  0x3C,
     0x18,  0xA8,  0x28,  0x62,  0xC1,  0x7F,  0x01,  0xC7,
     0x07,  0x4F,  0x46,  0x4D,  0x68,  0x00,  0x41,  0x37,
     0x4A,  0xCD,  0x67,  0x0B,  0x48,  0xC4,  0xBF,  0x10,
     0xA5,  0x91,  0xED,  0xB3,  0xBB,  0xDB,  0x76,  0xDB,
     0xE4,  0x5B,  0x7E,  0xD7,  0xA1,  0x3D,  0x52,  0xA3,
     0x11,  0x21,  0xAF,  0x9A,  0x49,  0xDA,  0x56,  0x2A,
     0xA9,  0xFD,  0x11,  0x65,  0x8C,  0xA6,  0x41,  0x98,
     0x65,  0x39,  0x82,  0x10,  0x24,  0x6B,  0x0D,  0xA4,
     0x60,  0x5E,  0x22,  0x9A,  0x0F,  0xE8,  0xBE,  0xC5,
     0x26,  0xE9,  0x2E,  0xF9,  0x54,  0xBF,  0xD6,  0x53,
     0xA1,  0xF9,  0x02,  0xFA,  0x57,  0x38,  0x40,  0x90,
     0x72,  0xD2,  0xFC,  0xF3,  0x74,  0xDD,  0x0E,  0x20,
     0xFB,  0x90,  0x85,  0xDD,  0x94,  0x10,  0x6F,  0xD2,
     0xD5,  0xBC,  0x94,  0x63,  0xF5,  0x99,  0x43,  0x91,
     0x58,  0xCB,  0x1C,  0xAB,  0x7B,  0x31,  0x79,  0xFC,
     0x4A,  0xB2,  0x5C,  0xF6,  0x63,  0xA1,  0xD3,  0x43,
     0xCC,  0xD2,  0xDE,  0x89,  0x72,  0xE4,  0xFD,  0x16,
     0x2D,  0x96,  0xE8,  0x2D,  0x42,  0x25,  0x09,  0xC8,
     0x33,  0xDB,  0x94,  0x29,  0x98,  0x26,  0x1A,  0x36,
     0x35,  0x3D,  0x49,  0xAB,  0x5F,  0xB9,  0xCF,  0xC7,
     0x4B,  0xF1,  0x50,  0xA6,  0x20,  0x8C,  0x5B,  0x84,
     0xA7,  0x88,  0x54,  0x69,  0xA8,  0x7C,  0x57,  0x69,
     0x31,  0x88,  0x35,  0xA2,  0xC3,  0xF3,  0x8A,  0xF5,
     0x65,  0x19,  0x2A,  0x15,  0xF0,  0xAA,  0x61,  0x88,
     0x6F,  0x8A,  0x63,  0x01,  0x34,  0x4F,  0xA1,  0x4A,
     0x94,  0x84,  0x8F,  0x1C,  0x2B,  0x11,  0xFA,  0x2C,
     0xF6,  0xFE,  0x38,  0x01,  0xE0,  0x38,  0x5F,  0x0C,
     0xDC,  0x2F,  0xF4,  0x45,  0xFF,  0x22,  0xD9,  0x3E,
     0xC2,  0x71,  0x56,  0x92,  0x59,  0xFB,  0x43,  0x73,
     0xA1,  0x38,  0x2C,  0xD5,  0xCD,  0x90,  0xBA,  0x80,
     0x00,  0xC3,  0x28,  0x1C,  0x46,  0x56,  0xED,  0x91,
     0xA3,  0x0D,  0x2F,  0x93,  0x75,  0xB5,  0x51,  0x39,
     0xE0,  0x04,  0xC1,  0x23,  0xB4,  0x1D,  0x26,  0x1D,
     0xE3,  0x94,  0x72,  0x32,  0x8A,  0x2B,  0xC5,  0x23,
     0x8C,  0x77,  0x62,  0x71,  0x14,  0x7A,  0x08,  0xC1,
     0xCD,  0x19,  0xAC,  0xFB,  0xB7,  0x54,  0xC1,  0x81,
     0xDA,  0xFC,  0x0D,  0xB9,  0x54,  0x7C,  0x1D,  0xBE,
     0xED,  0x51,  0x16,  0xB3,  0x77,  0xCB,  0x93,  0x74,
     0xA9,  0x58,  0xB5,  0xDD,  0xCE,  0x2E,  0x5B,  0x46,
     0xC5,  0x26,  0x73,  0x19,  0x83,  0xE1,  0xDD,  0xF0,
     0x16,  0x3D,  0x46,  0x40,  0x11,  0x63,  0x4F,  0xF9,
     0x7A,  0x2A,  0x78,  0x7F,  0x78,  0xB2,  0xD9,  0xEE
};


unsigned char  out_data_asm[COLS];
unsigned char  out_data_c[COLS];

int main()
{   
/* ======================================================================== */
/*  Clear output arrays. This is for comparison only.                       */ 
/* ======================================================================== */   
    memset(out_data_asm, 0, sizeof(out_data_asm));
    memset(out_data_c,   0, sizeof(out_data_c)  );

/* ======================================================================== */
/*  Call hand-coded assembly version (located in IMG62x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    IMG_perimeter(in_data+COLS, COLS, out_data_asm);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    IMG_perimeter_c(in_data+COLS, COLS, out_data_c);

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
/*  End of file:  img_perimeter_d.c                                         */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
