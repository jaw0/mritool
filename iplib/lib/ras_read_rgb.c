#include <stdio.h>

/*****************************************************************
		ras_read_rgb.c
	reads in a file, fp, of depth deep, and len long,
	into the 3  8bit arrays, onefor each R G B component
	color map xlation takes place for 8bit deep input
*****************************************************************/
void
ras_read_rgb(fp,R,G,B,depth,len,rmap,gmap,bmap)
	FILE *fp;
	unsigned char *R,*G,*B;
	int depth,len;
	unsigned char *rmap,*gmap,*bmap;
{
	int i,n;
	unsigned char r,g,b,f;

switch (depth){
	
	case 1:
		for (i=0;i<len/8;i++){
			fread(&f,1,1,fp);
			for (n=0;n<8;n++)
				R[i*8+n]=G[i*8+n]=B[i*8+n]=((f&(1<<n))==0)?0:0xFF;
		}
		return;
	case 8:
		for (i=0;i<len;i++){
			fread(&f,1,1,fp);
			if (rmap !=NULL){
				R[i]=rmap[f];
				G[i]=gmap[f];
				B[i]=bmap[f];
			}else{
				R[i]=G[i]=B[i]=f;
			}
		}
		return;
	case 24:
		for (i=0;i<len;i++){
			fread(&(R[i]),1,1,fp);
			fread(&(G[i]),1,1,fp);
			fread(&(B[i]),1,1,fp);
		}
		return;
	default:
		return;
	}
}
/***********************************************************************/

