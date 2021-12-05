
#define TH 95 //65
static int COUNT=0;
static int DELREG[TH][2]={{0},{0}};
static unsigned char Flag;
int X,Y;
int indexing(unsigned char *in_img,unsigned char *out_img,int m,int n)
{
if(Flag==0)
return(0);
if(Flag==1){
if(out_img[m*Y+n]==0)
{
out_img[m*Y+n]=1;
if(in_img[m*Y+n]==255)
{
//printf("%d %d \n",m,n);
indexing(in_img,out_img,(m-1),(n-1));
indexing(in_img,out_img,(m-1),n);
indexing(in_img,out_img,(m-1),(n+1));
indexing(in_img,out_img,m,(n-1));
indexing(in_img,out_img,m,(n+1));
indexing(in_img,out_img,(m+1),(n-1));
indexing(in_img,out_img,(m+1),n);
indexing(in_img,out_img,(m+1),(n+1));
if(COUNT<=TH){
DELREG[COUNT][0]=m;
DELREG[COUNT][1]=n;
} 
COUNT++;
if(COUNT==TH){
Flag=0;
}
} 
}
}
return(0);
}
void bwareaopen(unsigned char *in_img,unsigned char *out_img,int row,int col)
{
int i,j,xx;
X=row;
Y=col;
for(i=1;i<X;i++)
{
for(j=1;j<Y;j++)
{
if(in_img[i*Y+j]==255 && out_img[i*Y+j]==0 )
{
Flag=1;
indexing(in_img,out_img,i,j);
//printf("end of cluster :::size=%d \n",COUNT);
//area delete if below threshold amount
if(COUNT<TH)
{
for(xx=0;xx<COUNT;xx++)
{
in_img[(DELREG[xx][0]) *Y +(DELREG[xx][1])]=0;
}
}
COUNT=0; 
}
}
}}
