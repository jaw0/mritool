#include <rasterfile.h>
#include <stdio.h>

/*******************************************************************
		ras_open_ghdr.c

	opens the file name, read in the header to the struct
	pointed to by header, checks it for valid, and
	returns the file pointer to it

Date: May 30 1991
By:   Jeff Weisberg
********************************************************************/


FILE *
ras_open_ghdr(name,header) 
	char *name;
	struct rasterfile *header;
{
	FILE *fp;
if ( name[0] == '-' ) {
	fp = stdin;
}else{
	if ( (fp = fopen(name,"r")) ==NULL){
		fprintf(stderr,"\r\nCould not open file %s for input\r\n",name);
		exit(-1);
	}
}
fread(header,sizeof(struct rasterfile),1,fp);
if (header->ras_magic !=RAS_MAGIC){
	fprintf(stderr,"\r\n%s is not a valid rasterfile\r\n",name);
	exit(-1);
}
if ((header->ras_maptype !=RMT_NONE)&&(header->ras_maptype != RMT_EQUAL_RGB)&&(header->ras_maptype != RMT_RAW)){
	fprintf(stderr,"\r\n%s is not a supported type\r\n",name);
	exit(-1);
}
if (header->ras_maplength >768) {
	fprintf(stderr,"\r\n%s has a corrupt header\r\n",name);
	exit(-1);
}

return(fp);
}

/************************************************************/
