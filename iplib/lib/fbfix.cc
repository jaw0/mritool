
#include <fbimage.h>

int main(int argc, char**argv){
	Framebuffer fb;
	fb.cmap = new Cmap;
	fb.get_cmap();

	Uimage foo;
	foo.cmap = fb.cmap;
	foo.load("~/images/cmap.ras.gz");

	fb.install_cmap();
	
}

