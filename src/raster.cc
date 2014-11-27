
#include <mritool.h>
#include <rasterfile.h>
#include <fstream.h>
#include <fullname.h>

void ras_save(char *name, u_char *img, int xdim, int ydim){
	FILE *fp;
	struct rasterfile rast;
	int len=xdim*ydim;

	if( !(fp=fopen( fullname(name), "w") )) return;
	
	rast.ras_magic = RAS_MAGIC;
	rast.ras_type = RT_STANDARD;
	rast.ras_maptype = RMT_NONE;
	rast.ras_maplength = 0;
	rast.ras_width = xdim;
	rast.ras_height = ydim;
 
        /* save as 8bit */
	rast.ras_depth = 8;
	rast.ras_length = len;
	fwrite((char*)&rast,sizeof(struct rasterfile),1,fp); /* write out header   */
	fwrite((char*)img,1,len,fp);

	fclose(fp);

}


int ras_load(char *name, u_char *img, int xdim, int ydim){
	FILE *fp;
	struct rasterfile rast;
	int len;

	if( !(fp=fopen( fullname(name), "r") )) return 0;
	

	fread((char*)&rast, sizeof(struct rasterfile),1,fp);
	fread((char*)img, 1, xdim*ydim, fp);

	fclose(fp);

	if( xdim*ydim != rast.ras_width * rast.ras_height) return 0;
	if( rast.ras_magic !=  RAS_MAGIC) return 0;
	if( rast.ras_type != RT_STANDARD) return 0;
	if( rast.ras_maptype != RMT_NONE) return 0;

	return 1;
}



	
	
	


	
