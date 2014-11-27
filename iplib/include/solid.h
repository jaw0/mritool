

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifndef _solid_h
#define _solid_h

#ifdef RCSID
static const char *const rcsid_solid_h
= "@(#)$Id: solid.h,v 1.1 1995/11/23 04:31:17 jaw Exp jaw $";
#endif


#include <images.h>

class Solid {

  protected:
	Bimage**data;
	int size;
	void uinit(int x, int y, int z);
	void finit(int x, int y, int z);
	
  public:
	int depth, width, height;
	Point offset;

	Solid(int,int,int);
	Solid();
	Solid(Solid&);
	virtual ~Solid();

	Bimage& operator[](int);
	const Bimage& operator[](int) const;
	float operator[](Point) const;
	float operator()(int, int,int) const;
	
	int fill(void);
	void segment(Solid&, int, float, float, float, int, int, float);
};



class Usolid : public Solid {
  public:
	Usolid(int, int, int);
	Usolid(int);
};

class Fsolid : public Solid {
  public:
	Fsolid(int, int, int);
	Fsolid(int);
};


inline Bimage& Solid::operator[](int i){
	i -= offset.z;
	if(i>=size || i<0) i=0;

	return *data[i];
}
inline const Bimage& Solid::operator[](int i) const{
	i -= offset.z;
	// if(i>=size || i<0) i=0;

	return *data[i];
}
inline float Solid::operator[](Point p) const{
	p -= offset;
	return data[p.z]->value(p);
}
inline float Solid::operator()(int x, int y, int z) const{
	z -= offset.z;
	// if(z>=size || z<0) z=0;
	return data[z]->value(x-offset.x, y-offset.y);
}

#endif /* !_solid_h */
