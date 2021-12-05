
#include "morph.h"
#include "stdlib.h"
#include <memory.h>
void mask_dilation(unsigned char *the_image,unsigned char *out_image,int row,int col)
{
int a, b, count, i, j, k,Y;
short max;
Y=col;
for(i=NN/2; i<row-NN/2; i++){
for(j=NN/2; j<col-NN/2; j++){
max = 0;
for(a=-NN/2; a<=NN/2; a++){
for(b=-NN/2; b<=NN/2; b++){
if(mask[a+NN/2][b+NN/2] == 1){
if(the_image[(i+a)*Y+(j+b)] > max)
max = the_image[(i+a)*Y+(j+b)];
}}}

out_image[i*Y+j] = max;
}}}
void mask_erosion(unsigned char *the_image,unsigned char *out_image,int row,int col)
{
int a, b, count, i, j, k,Y;
short min;
Y=col;
for(i=NN/2; i<row-NN/2; i++){
for(j=NN/2; j<col-NN/2; j++){
min = 255;
for(a=-NN/2; a<=NN/2; a++){
for(b=-NN/2; b<=NN/2; b++){
if(mask[a+NN/2][b+NN/2] == 1){
if(the_image[(i+a)*Y+(j+b)] < min)
min = the_image[(i+a)*Y+(j+b)];
}}} 
out_image[i*Y+j] = min;
}}}
void morph_mask_Opening(unsigned char *the_image,unsigned char *out_image,int row,int col)
{
short i,j;
unsigned char *temp;//[row][col];
temp=(unsigned char *)malloc(sizeof(char)*row*col);
mask_erosion(the_image,out_image,row,col);
memcpy (temp, out_image, row*col*sizeof(unsigned char));
mask_dilation(temp,out_image,row,col);
free(temp);
}
void morph_mask_Closing(unsigned char *the_image,unsigned char *out_image,int row,int col)
{
short i,j;
unsigned char *temp;//[row][col];
temp=(unsigned char *)malloc(sizeof(char)*row*col);
mask_dilation(the_image,out_image,row,col);
memcpy (temp, out_image, row*col*sizeof(unsigned char));
mask_erosion(temp,out_image,row,col);
free(temp);
}
