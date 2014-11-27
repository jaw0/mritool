

#include <ras_stuff.h>
#include <rasterfile.h>

ras_read_3d(image,xdim,ydim,zdim,files)
	u_char ****image;
	int *xdim, *ydim, zdim;
	char **files;{	
	int x,y,z,zd=zdim,xd,yd;
	int i,n;
	u_char ***nmg;
	FILE *fp;
	struct rasterfile header;
	u_char cmap[768];


	fp = ras_open_ghdr(fullname(files[0]),&header);	/* get size info from 1st slice */
	*xdim = xd = header.ras_width;
	*ydim = yd = header.ras_height;
	*image = nmg = malloc_3d(xd,yd,zd,sizeof(u_char));

	for(z=0;z<zd;z++){
		if (z) {
			/* open the file and check it out */
			fp = ras_open_ghdr(fullname(files[z]),&header);
			if( (header.ras_width != xd)||(header.ras_height != yd) ){
				fprintf(stderr,"image read: %s is wrong size (%dx%d)\n",
					files[z],header.ras_width,header.ras_height);
				exit(-1);
			}
		}
		if (header.ras_maplength) 
			fread(cmap,header.ras_maplength,1,fp);     /* read in the color map if it exists */

		for(y=0;y<yd;y++){

			/* read in one row @ a time */

			switch (header.ras_depth){
			  case 1:
				for (i=0;i<xd/8;i++){
					u_char buf;
					fread(&buf,1,1,fp);	/* bit by bit */
					for (n=0;n<8;n++) nmg[z][y][i*8+n] = ((buf &(1<<7-n))==0)?0xFF:0;
				}
				break;
			  case 8:
				fread(nmg[z][y],1,xd,fp); /* entire row in one fell swoop */
				break;
			  case 24:
				for (i=0;i<xd;i++){
					u_char r,g,b;

					fread(&r,1,1,fp);	/* triple by triple, converting as we go */
					fread(&g,1,1,fp);
					fread(&b,1,1,fp);
					nmg[z][y][i] = 3*r/10 + 3*g/5 + b/10 ; /* approx, should be .299, .587, .114 */
				}
				break;
			}
		} /* for y */

		fclose(fp);
	} /* for z */



}
