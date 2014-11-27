

#include <fbimage.h>
#include <stdlib.h>

main(int argc, char**argv){

	Framebuffer fb;
	Uimage foo, bar;
	int k = atoi(argv[2]);
	
	foo.cmap = new Cmap;
	foo.load(argv[1]);
	
	foo.kmeans(bar, k, 1000);
	
	bar.punch(0, k);
	bar.save(".xl");

}
