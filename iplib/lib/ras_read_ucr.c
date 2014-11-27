#include <rasterfile.h>
#include <stdio.h>

/********************************************************************
		ras_read_ucr.c
	reads in the file fp, which is depth deep and len long,
	to the memory pointed to by X, as unsigned chars. no
	 errors are returned or set if it fails

By:  Jeff Weisberg
********************************************************************/


void
ras_read_ucr(fp,X,depth,len)
	/* read in image convert to 8bit form stored as an 8bit array */

	FILE *fp;
	unsigned char *X;
	int depth,len;
{
	int i,n;
	unsigned char buf,r,g,b;

switch (depth){
	case 1:
		for (i=0;i<len/8;i++){
			fread(&buf,1,1,fp);
			for (n=0;n<8;n++) X[i*8+n] = ((buf &(1<<7-n))==0)?0xFF:0;
		}
		return;
	case 8:
		fread(X,1,len,fp);
		return;
	case 24:
		for (i=0;i<len;i++){
			fread(&r,1,1,fp);
			fread(&g,1,1,fp);
			fread(&b,1,1,fp);
			X[i]= r/3 + g/3 + b/3 ; /* approx */
		}
	}
			
}



/*******************************************************************/

