/* ======================================================================== */
/* C6000 functions                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2002 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifdef _TMS320C6X
# define SHL(x,y) ( (x) << (y) )
# define SHR(x,y) ( (x) >> (y) )

#else

# define SHL(x,y) ( ((y) & 32) ? 0 : ((x) << (y)) )
# define SHR(x,y) ( ((y) & 32) ? 0 : ((x) >> (y)) )

unsigned int _lmbd(int bit, unsigned num);
unsigned int _norm(unsigned int x); 
unsigned int _ext(int x, unsigned int a, unsigned int b); 
unsigned int _extu(unsigned int x, unsigned int a, unsigned int b); 
int _sshl(int x, unsigned int s);

#endif
