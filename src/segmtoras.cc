
#define SEGMFILE_MAGIC  ":jwMRs02"

#include <string.h>
#include <images.h>
#include <stdlib.h>

struct SegmFile {
        char magic[10];
        int xdim, ydim;
        int nlevs;
        u_char offline;
        int pid;
};
 
main(){
	SegmFile hdr;
	Fimage foo(256, 256);
	
	cin.read( (char*)&hdr, sizeof(hdr) );
	if (strncmp(hdr.magic, SEGMFILE_MAGIC, 8)){
		cerr << "load segment file: bad header" << endl;
		exit(-1);
	}
	if(hdr.offline){
		cerr << "Segm file is offline" << endl;
		exit(-1);
	}

	cin.read((char*)(float*)foo, 256*256*sizeof(float));

	foo.save( cout );

	exit(0);
}

	
