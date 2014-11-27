#include <stdio.h>

/********************************************************************/

void
ras_read_bin(fp,X,depth,len)
	/* read in image, convert to binary -1/0 form, and store in an 8bit array */

	FILE *fp;
	unsigned char *X;
	int depth,len;
{
	int i,n;
	unsigned char buf;

if (depth ==1){
	for (i=0;i<len/8;i++){
		fread(&buf,1,1,fp);
		for (n=0;n<8;n++) X[i*8+n] = ((buf &(1<<n))==0)?0:0xFF ;
	}
}else{ /* assume depth 8 */

	for (i=0;i<len;i++){
		fread(&buf,1,1,fp);
		X[i] = (buf ==0) ? 0 : 0xFF ;
	}
}
}

/********************************************************************/

