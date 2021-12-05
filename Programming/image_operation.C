
#include <stdio.h>
void im2bw(unsigned char *bwim,unsigned char ths,int X,int Y)
{
int i;
for(i=0;i<X*Y;i++)
bwim[i]=bwim[i]>ths?255:0;
}
void imMinus(unsigned char *im1,unsigned char *im2,int row,int col)
{
short i,j;
for(i=0;i<row;i++)
for(j=0;j<col;j++)
if(im1[i*col+j]>=im2[i*col+j])
im1[i*col+j]=0;
else
im1[i*col+j]=(im2[i*col+j]-im1[i*col+j]);
}
void whitePixelCount(unsigned char *bwim,int X,int Y)
{
int i,j;
int NosOfWhitePixel, NosOfBlackPixel;
unsigned long n=1;
NosOfBlackPixel=0;
NosOfWhitePixel=0;
for(i=0;i<X;i++)
for(j=0;j<Y;j++)
{
if(bwim[i*Y+j]==255)
NosOfWhitePixel++;
else NosOfBlackPixel++;
}
printf("\nNos Of White Pixels are %d\n",NosOfWhitePixel);
printf("Nos Of Black Pixels are %d \n",NosOfBlackPixel);
}
void imAnd(unsigned char *im1,unsigned char *im2,int row,int col)
{
short i,j;
for(i=0;i<row;i++)
for(j=0;j<col;j++)
{
im1[i*col+j]=im1[i*col+j] | im2[i*col+j];
}
}
