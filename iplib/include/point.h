

#ifndef _point_h
#define _point_h

#ifdef RCSID
static const char *const rcsid_point_h
= "@(#)$Id: point.h,v 1.1 1995/11/23 04:31:15 jaw Exp jaw $";
#endif

#include <iostream.h>
#include <math.h>

struct xxPointxx {
	int x,y,z;
};

// impl a nice point class
class Point {
  public:
	int x,y,z;
	Point(int i,int j,int k){x=i;y=j;z=k;}
	Point(int i, int j)	{x=i;y=j;z=0;}		// a few constructors
	Point(int i)		{x=y=z=i;}
	Point(struct xxPointxx p)	{x=p.x;y=p.y;z=p.z;}
	Point()			{x=y=z=0;}
	Point& operator=(int i){x=y=z=i; return *this;}	// conversion to Point from int
	void bound(int l, int h){
		x=(x<l)?l:((x>h)?h:x);
		y=(y<l)?l:((y>h)?h:y);
	}
	Point& operator+=(Point b){
		x += b.x; y += b.y; z += b.z;
		return *this;
	}
	Point& operator-=(Point b){
		x -= b.x; y -= b.y; z -= b.z;
		return *this;
	}
	Point& operator*=(float b){
		x = int(x*b); y = int(y*b); z = int(z*b);
		return *this;
	}
	Point& operator/=(float b){
		x = int(x/b); y = int(y/b); z = int(z/b);
		return *this;
	}
};


// some simple Point arithm. ops.
inline Point operator+(Point a, Point b){
	return Point(a.x+b.x , a.y+b.y, a.z+b.z);
}
inline Point operator-(Point a, Point b){
	return Point(a.x-b.x , a.y-b.y , a.z-b.z);
}
inline int operator==(Point a, Point b){
	return( a.x==b.x && a.y==b.y && a.z==b.z);
}
inline int operator!=(Point a, Point b){
	return( a.x!=b.x || a.y!=b.y || a.z!=b.z);
}
inline int operator!(Point a){
	return( a.x==0 && a.y==0 && a.z==0);
}
inline Point operator/(Point a, float b){
	return Point(int(a.x/b), int(a.y/b), int(a.z/b));
}
inline Point operator*(Point a, float b){
	return Point(int(a.x*b), int(a.y*b), int(a.z*b));
}
inline ostream& operator<<(ostream& os, Point p){
	os << "(" << p.x << "," << p.y << ")" ;
	return os;
}

// what constitutes < , > for a Point?
// these allow a>b _AND_ b>a to both be true
inline int operator<(Point a, Point b){
	return ( (a.x<b.x) || (a.y<b.y) );
}
inline int operator>(Point a, Point b){
	return ( (a.x>b.x) || (a.y>b.y) );
}


inline Point abs(Point a){
	return Point( ((a.x<0)?-a.x:a.x)
		      , ((a.y<0)?-a.y:a.y), ((a.z<0)?-a.z:a.z));
}

inline Point sign(Point a){
	return Point( ((a.x>0)? 1: ((a.x<0)? -1 : 0))
		      , ((a.y>0)? 1: ((a.y<0)? -1 : 0))
		      , ((a.z>0)? 1: ((a.z<0)? -1 : 0)));
}

inline float angle(Point a, Point b){
        // find angle btwn. 2 points (and orgin) (0-pi)
        float cst;
 
        // A . B = |A| |B| cos theta
 
        cst = (a.x*b.x + a.y*b.y)
            / sqrt( (a.x*a.x+a.y*a.y)*(b.x*b.x+b.y*b.y) );
 
        return acos(cst);
}
inline float fullangle(Point a, Point b){
        // find angle (0-2pi)
        float cst, dnm, sst, ang;
 
        dnm = sqrt(a.x*a.x+a.y*a.y)*(b.x*b.x+b.y*b.y);
        cst = (a.x*b.x + a.y*b.y) / dnm;
        sst = (a.x*b.y - a.y*b.x) / dnm;
 
        ang = acos(cst);
        if(sst<0){
                // ang = -ang;
                ang = 2*M_PI - ang;
        }
        return ang;
}


const Point orgin(0,0,0);


#endif /* !_point_h */

