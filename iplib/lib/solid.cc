

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: solid.cc,v 1.1 1995/11/23 04:31:06 jaw Exp jaw $";
#endif

#include <solid.h>

Solid::Solid(int x, int y, int z){

	depth  = z;
	width  = x;
	height = y;
	offset = 0;
	data = 0;
	size = 0;

	uinit(x, y, z);
}
Solid::Solid(){
	data=0;
	size = 0;
	offset = 0;
	width = height = depth = 0;
}
void Solid::finit(int x, int y, int z){
	int i;
	depth = z;
	width = x;
	height= y;
	
	if(data){
		for(i=0;i<size;i++)
			delete data[i];
		delete [] data;
	}
	size = z;
	data = new Bimage* [z];
	for(i=0;i<size;i++){
		data[i] = new Fimage;
		data[i]->width  = width;
		data[i]->height = height;
		data[i]->resize();
	}
}
void Solid::uinit(int x, int y, int z){
	int i;
	depth = z;
	width = x;
	height= y;
	
	if(data){
		for(i=0;i<size;i++)
			delete data[i];
		delete [] data;
	}
	size = z;
	data = new Bimage* [z];
	for(i=0;i<size;i++){
		data[i] = new Uimage;
		data[i]->width  = width;
		data[i]->height = height;
		data[i]->resize();
	}
}
	
Solid::~Solid(){

	if(data) delete [] data;
	data = 0;
	height = width = depth = 0;
}


int Solid::fill(void){
	int i, j, a=width*height*depth;
	u_char c;
	
	for(i=0;i<depth;i++){
		a -= data[i]->fill(0,128);
		for(j=0;j<width*height;j++){
			c = u_char( data[i]->value(j));
			data[i]->put(j, ( ((c^(c>>1))&0x40) ? 255 : 0));
		}
	}
	return a;
}


Usolid::Usolid(int z){

	uinit(0, 0, z);
}
Fsolid::Fsolid(int z){

	finit(0, 0, z);
}

Usolid::Usolid(int x, int y, int z){
	uinit(x, y, z);
}
Fsolid::Fsolid(int x, int y, int z){
	finit(x, y, z);
}



