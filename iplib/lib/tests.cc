

#include <images.h>

main(){
	Ptable p("~/.ptable");
	Bimage::static_ptable = &p;
	
	Fimage a(256, 256);
	a = 0;
	a[256*128] = 255;


	a.save("/tmp/foobar.gif");

}

