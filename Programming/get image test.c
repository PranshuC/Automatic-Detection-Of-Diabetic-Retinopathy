
#include<stdio.h>
//char *in_name;
char *in_name ="aaa.bmp";
short **the_image;
long height, width;

int read_image_array(file_name, array)
{
char *file_name;
short **array;
{
int ok = 0;
if(is_a_tiff(file_name)){
read_tiff_image(file_name, array);
ok = 1;
}
if(is_a_bmp(file_name)){
read_bmp_image(file_name, array);
ok = 1;
}
if(ok == 0){
printf("\nERROR could not read file %s",
file_name);
exit(1);
}
} /* ends read_image_array */
};

int get_image_size(file_name, rows, cols)
{
char *file_name;
long *cols, *rows;
{
int is_bmp = 0,
is_tiff = 0,
result = 0;
struct bitmapheader bmph;
struct tiff_header_struct tiffh;
if(is_a_bmp(file_name)){
is_bmp = 1;
read_bm_header(file_name, &bmph);
*rows = bmph.height;
*cols = bmph.width;
} /* ends if is_a_bmp */
if(is_a_tiff(file_name)){
is_tiff = 1;
read_tiff_header(file_name, &tiffh);
*rows = tiffh.image_length;
*cols = tiffh.image_width;
} /* ends if is_a_bmp */
if(is_bmp == 1 || is_tiff == 1)
result = 1;
return(result);}

short **allocate_image_array(length, width)
{
long length, width;
{
int i;
short **the_array;
the_array = malloc(length * sizeof(short *));
for(i=0; i<length; i++){
the_array[i] = malloc(width * sizeof(short ));
if(the_array[i] == '\0'){
printf("\n\tmalloc of the_image[%d] failed", i);
} /* ends if */
} /* ends loop over i */
return(the_array);
} /* ends allocate_image_array */
}
void main(){

get_image_size(in_name, &height, &width);
the_image = allocate_image_array(height, width);
read_image_array(in_name, the_image);
}
}
