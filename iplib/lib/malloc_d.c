

#include <ras_stuff.h>


u_char **malloc_2d(xdim,ydim,so)
	int xdim, ydim, so;{
	int x,y;
	u_char **rp;

	rp = (u_char **)malloc(ydim*sizeof(u_char *));
	rp[0] = (u_char *)malloc(ydim*xdim*sizeof(u_char)*so);
	for(y=0;y<ydim;y++)
		rp[y] = rp[0] + y*xdim*so;

	return rp;
}

u_char ***malloc_3d(xdim,ydim,zdim,so)
	int xdim, ydim, zdim, so;{
	int x,y,z;
	u_char ***rp;

	rp = (u_char ***)malloc(zdim*sizeof(u_char**));
	for(z=0;z<zdim;z++){
		rp[z] = malloc_2d(xdim,ydim,so);

	}

	return rp;
}

