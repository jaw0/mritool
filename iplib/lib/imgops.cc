

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: imgops.cc,v 1.1 1995/11/23 04:31:04 jaw Exp jaw $";
#endif

#include <images.h>
#include <math.h>
#include <vector.h>
#include <handy.h>

Bimage& Bimage::operator==(u_char v){
	int i;

	loop(i, size){
		if( u_char(value(i))==v)
			put(i, 255.0);
		else
			put(i, 0.0);
	}
	return *this;
}
Bimage& Bimage::operator<=(float f){
	int i;
	loop(i, size){
		if(value(i) < f)
			put(i, 255.0);
		else
			put(i, 0.0);
	}
	return *this;
}
Bimage& Bimage::operator>=(float f){
	int i;
	loop(i, size){
		if(value(i) > f)
			put(i, 255.0);
		else
			put(i, 0.0);
	}
	return *this;
}

Bimage& Bimage::operator*=(const Bimage& a){
	Point p, fini(width, height);

	loopp(p, orgin, fini){
		if(a.within(p))
			ncput(p, ncvalue(p) * a.cvalue(p));
	}
	return *this;
}

Bimage& Bimage::operator/=(const Bimage& a){
	Point p, fini(width, height);
	complex v;
	
	loopp(p, orgin, fini){
		if(a.within(p)){
			v = a.cvalue(p);
			if( v.real() && v.imag() )
				ncput(p, ncvalue(p) / a.cvalue(p));
		}
	}
	return *this;
}

Bimage& Bimage::operator+=(const Bimage& a){
	Point p, fini(width, height);

	loopp(p, orgin, fini){
		if(a.within(p))
		   ncput(p, ncvalue(p) + a.cvalue(p));
	}
	return *this;
}

Bimage& Bimage::operator-=(const Bimage& a){
	Point p, fini(width, height);

	loopp(p, orgin, fini){
		if(a.within(p))
		   ncput(p, ncvalue(p) - a.cvalue(p));
	}
	return *this;
}

Bimage& Bimage::operator|=(const Bimage& a){
	Point p, fini(width, height);

	loopp(p, orgin, fini){
		if(a.within(p))
			nput(p, MAX(nvalue(p), a.value(p)) );
	}
	return *this;
}

Bimage& Bimage::operator&=(const Bimage& a){
	Point p, fini(width, height);

	loopp(p, orgin, fini){
		if(a.within(p))
			nput(p, MIN(nvalue(p), a.value(p)) );
	}
	return *this;
}

Bimage& Bimage::operator^=(const Bimage& a){
	Point p, fini(width, height);

	loopp(p, orgin, fini){
		if(a.within(p))
			nput(p, (int)nvalue(p) ^ (int)a.value(p) );
	}
	return *this;
}


void Bimage::histogram(Vector& v){
	Point p, fini(width, height);
	int i;

	// we are asuming image is 0..255
	
	for(i=0; i<256; i++) v[i] = 0;
	
	loopp(p, orgin, fini){
		v[ (int)nvalue(p) ]++;
	}
}

	
