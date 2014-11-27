

/***********************************************
		vff_write_3d.c

	write out a 3D .vff format file

Date:	Jan 20, 1993
By:	Jeff weisberg

***********************************************/

#include <stdio.h>


int vff_write_3d(nimg, xdim, ydim, zdim, depth, fp)
	unsigned char ***nimg;
	int xdim, ydim, zdim, depth;
	FILE *fp;		{
	int x,y,z,i=0;

	if (depth != 8) {
		fprintf(stderr, "vff_write_3d: invalid image depth %d\n", depth);
		exit(-1);
	}

	fprintf(stderr,"Saving image...\n");

	/* first the header */
	fprintf(fp, "\
ncaa\n\
rank=3;\n\
aspect=1.0 1.0 1.0;\n\
bits=8;\n\
format=slice;\n\
orgin=0.0 0.0 0.0;\n\
type=raster;\n\
bands=1;\n\
extent=%d.0 %d.0 %d.0;\n\
title=Produced by vff_write_3d(), Jeff Weisberg;\n\
size=%d %d %d;\n\
rawsize=%d;\n\
\n", 
		xdim, ydim, zdim, 
		xdim, ydim, zdim, 
		xdim*ydim*zdim);

	for(z=0;z<zdim;z++){
		for(y=0;y<ydim;y++){
			/* write a row */
			/*for(x=0;x<xdim;x++){
				fwrite(&(nimg[z][y][x]),1,1,fp);
				i += nimg[z][y][x]?1:0;*/
			fwrite(nimg[z][y], 1, xdim, fp);
			/*}*/
		}
	}

	fprintf(stderr,"p=%d\n", i);

}







