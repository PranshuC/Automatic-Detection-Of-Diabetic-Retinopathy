
//x,y should defined
#include "data.h"
//int Y=255;
//int X=255;
void rgb2gray(unsigned char *bw,const unsigned char im[][Y*for_RGB])
{
int i,j;
for(i=0;i<X;i++)
{
for(j=0;j<Y;j++)
bw[i*Y+j]=((0.21*(*(*(im+i)+j))+0.71*(*(*(im+i)+(j+Y)))+0.07*(*(*(im+i)+(j+2*Y))))+0.5);
}
}
