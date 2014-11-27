

#include <images.h>

main(){
	Uimage a;
	Cmap cmap;
	a.cmap = &cmap;
	a.load("=i/nagel.gif");

	//a.zoom(.5);
	//a.rotate(90);

	a.countcolors();
	a.ordercolors();
	a.resizecolors(256);
	a.rotate(30);
	a.save(".xl");

	
}

