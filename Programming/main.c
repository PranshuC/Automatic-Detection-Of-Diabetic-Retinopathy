
#include <stdio.h>
#include <memory.h>
#include <time.h>
#define _TI_ENHANCED_MATH_H 1
#include <math.h>
#include "img_histogram.h"//c6000 IMGLIB
#define UINT8_MIN 0
#define UINT8_MAX 255
#define X image_width
#define Y image_height
#include "ColSpaceConv.c"
#include "CLAHE.c"
#include "image_operation.C"
#include "bwareaopen.c"
#include "MORPH.c"
#pragma DATA_ALIGN (hist, 4)
unsigned short hist[256];
/* scratch buffer needed by IMGLIB */
unsigned short t_hist[1024];
int main()
{
int i,j;
unsigned char *bwim,temp[256*256],*temp1;time_t start, stop;
long count;
DSK6713_init();
time(&start);
//rgb2gray(bwim,im);
IMG_histogram(im, X*Y, 1, t_hist, hist);
printf("%d ---\n",CLAHE(im,X,Y,UINT8_MIN,UINT8_MAX,8,4,128,5.21)); 
memcpy (temp, im, X*Y*sizeof(unsigned char));
morph_mask_Opening(im,temp,X,Y);
imMinus(im,temp,X,Y);
im2bw(im);
memset(temp,0,X*Y*sizeof(unsigned char));
bwareaopen(bwim,temp,X,Y);
time(&stop);
printf("Finished in about %.0f seconds. \n", difftime(stop, start));
return 0;
}
