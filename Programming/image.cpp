
#include<stdio.h>
void main()
{
char *in_name;
short **the_image;
long height, width;
get_image_size(in_name, &height, &width);
the_image = allocate_image_array(height, width);
read_image_array(in_name, the_image);
}
