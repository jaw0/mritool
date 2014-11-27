
/********************************************
		ras_write_ucr.c
	writes out the image in the array of 
	unsigned chars to the file fp
	no cmap is saved

Date: June 2 1991
By:   Jeff Weisberg
*******************************************/

#include <rasterfile.h>
#include <stdio.h>

int
ras_write_ucr(nimg,xdim,ydim,depth,fp)
	unsigned char *nimg;
	int xdim,ydim,depth;
	FILE *fp;
{
	struct rasterfile rast;
	int len,i,n;
	unsigned char foo;


/* write out the image */
fprintf(stderr,"Saving image...\r\n");
len = xdim*ydim;
rast.ras_magic = RAS_MAGIC;
rast.ras_type = RT_STANDARD;
rast.ras_maptype = RMT_NONE;
rast.ras_maplength = 0;
rast.ras_width = xdim;
rast.ras_height = ydim;

switch (depth){
    case 1:
  	 /* image is saved in 1bit format */
	rast.ras_depth = 1;  
       	rast.ras_length = len/8;
	fwrite(&rast,sizeof(struct rasterfile),1,fp); /* write out header   */
	for (i=0;i<len/8;i++){
		foo = 0;
		for (n=0;n<8;n++){
			foo |= ( (nimg[i*8+n]==0)?1:0 )<<(7-n);
		}
		fwrite(&foo,1,1,fp); /* write out the image */
	}
	break;
   case 8:
        /* save as 8bit */
	rast.ras_depth = 8;
	rast.ras_length = len;
	fwrite(&rast,sizeof(struct rasterfile),1,fp); /* write out header   */
	fwrite(nimg,1,len,fp);
	break;
   case 24:
	/* save as 24bit; R=G=B (gray scale) */
	rast.ras_depth = 24;
	rast.ras_length = len *3;
	fwrite(&rast,sizeof(struct rasterfile),1,fp); /* write out header   */
	for (i=0;i<len;i++){
		fwrite(&(nimg[i]),1,1,fp); /* R */
		fwrite(&(nimg[i]),1,1,fp); /* G */
		fwrite(&(nimg[i]),1,1,fp); /* B */
	}
	break;

}
return(0);
} /* end of fnc */

