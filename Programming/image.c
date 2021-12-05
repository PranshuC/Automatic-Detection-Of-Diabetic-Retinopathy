
#include<stdio.h>
void main()
{
long height, width;
get_image_size("C:\Users\Pranshu\Desktop\DP.bmp", &height, &width);
unsigned long the_image[width][height];
the_image = allocate_image_array(height, width);
read_image_array("C:\Users\Pranshu\Desktop\DP.bmp", the_image);
int i,j;
for(i=0;i<=width;i++)
for(j=0;j<=height;j++)
{
	printf("%d",the_image[i][j]);
}
}"C:\Users\Pranshu\Desktop\DP.bmp"
