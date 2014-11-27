

#include <spec_image.h>

main(int argc,char **argv){


	Blur blur;
	spec_image edgemap(256,256);
	spec_image segm;
	int i,j, k;

	if(argc != 2 ) exit(1);
	k = atoi(argv[1]);
	segm.fload("=w/mri/debug/segm.ras");

	//blur.LoG(atof(argv[1]), atof(argv[2]));
	
	for(j=1;j<255;j++)for(i=1;i<255;i++){
		// do lapl
		edgemap[j*256 + i] =
			4*segm[j*256 + i]
			- segm[j*256 +i +1]
			- segm[j*256 +i -1]
			- segm[j*256 +i -256]
			- segm[j*256 +i +256];
	}

	edgemap.mag();

	for(i=0;i<256*256;i++) edgemap[i] = (edgemap[i]>10)?255:0;
		
	blur.gaussian(k,k,k,k);
	edgemap.blurc(blur);
	
	
//	edgemap.quantize(0);

	edgemap.fsave("=w/mri/debug/blur.ras");
}

