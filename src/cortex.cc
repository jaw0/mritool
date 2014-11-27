 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cortex.cc,v 1.1 1995/11/23 04:30:13 jaw Exp jaw $";
#endif
 
#include <mritool.h>
#include <solid.h>
#include <handy.h>

class M4 {
  public:
	float m[4][4];

	void set(int i, float a, float b, float c, float d){
		m[i][0] = a; m[i][1] = b; m[i][2] = c; m[i][3] = d;
	}
	
    friend Point operator*(M4&, Point);
};

int extract_level(Solid& solid, int x, int y, int z, float radius, float ratio){
	int l;
	int chng = 0;
	
	if(   x<0 || y<0 || z<0
	   || x>=solid.width
	   || y>=solid.height
	   || z>=solid.depth )
		return 0;

	// cerr << x << " " << y << " " << z << endl;

	l = solid(x,y,z);
	if( l!= 1 && l!= 2)
		return 0;

	solid[z].put(x,y, 256 - l);	// 255 | 254

	// roll a ball around
	// make sure it fits through holes...

	int xx,yy,zz;
	float pr;
	int t=0, p=0;
	float r2 = radius*radius;

	if( l == 1){
		// try only rolling  around the gray
		
		for(zz=-radius;zz<radius;zz++) 
			for(yy=-radius;yy<radius;yy++)
				for(xx=-radius;xx<radius;xx++){
					pr = xx*xx + yy*yy + zz*zz;
					if( pr > r2) continue;

					if(   x+xx<0 || y+yy<0 || z+zz<0
					   || x+xx>=solid.width
					   || y+yy>=solid.height
					   || z+zz>=solid.depth )
						continue;
					t++;
					if( solid(x+xx, y+yy, z+zz)) p++;
				}

		if( p < t * ratio) return 1;
	}
	// recurse
	extract_level(solid, x, y, z+1, radius, ratio);
	extract_level(solid, x, y, z-1, radius, ratio);
	extract_level(solid, x+1, y, z, radius, ratio);
	extract_level(solid, x-1, y, z, radius, ratio);
	extract_level(solid, x, y+1, z, radius, ratio);
	extract_level(solid, x, y-1, z, radius, ratio);

	return 1;
}

void create_projection(Solid& solid, int ex, int ey, int ez){
	// from 3D cortex space (a)
	// onto y'=0 plane (ap)
	
	int y;
	int v;
	M4 xform;
	float d;
	Point a, ap(0,0,0), fini(solid.width, solid.height, solid.depth);
	Fimage img (solid.width*4, solid.depth*4);	// nice big image plane
	Fimage imga(solid.width*4, solid.depth*4);	// to project on to
	Fimage imgb(solid.width*4, solid.depth*4);

	img = 0;
	imga = 0;
	imgb = 4*256*256;

	img.offset.x  = -3*solid.width/2;	img.offset.y  = -3*solid.depth/2;
	imga.offset.x = -3*solid.width/2;	imga.offset.y = -3*solid.depth/2;
	imgb.offset.x = -3*solid.width/2;	imgb.offset.y = -3*solid.depth/2;
	
	for(y=0; y<solid.height; y++){
		// build xform matrix
		xform.set(0,	y-ey, ex,   0,    -ex*y);
		xform.set(1,	0,    y-ey, 0,    0    );
		xform.set(2,	0,    ez,   y-ey, -ez*y);
		xform.set(3,	0,    1,    0,    -ey  );
     
		for(ap.z=img.offset.y;ap.z<solid.depth*5/2;ap.z++){
			for(ap.x=img.offset.x;ap.x<solid.width*5/2; ap.x++){
				// the xform
				a = xform * ap;

				if(   a.z<0		// make sure it is within our noggin
				   || a.x<0
				   || a.z>=solid.depth
				   || a.x>=solid.width
			   	) continue;

				// get the pixel value from the head
				v = (int)solid[a.z].value(a.x, y);

				// put the pixel in the image (integrate)
				img(ap.x,ap.z) = img(ap.x,ap.z) + (v?1:0);

				if( v ){
					// try looking at the distance^2 to outer surface
					d = (a.x-ex)*(a.x-ex)
						+ (y-ey)*(y-ey)
						+ (a.z-ez)*(a.z-ez);

					if( d > imga(ap.x,ap.z))
						imga(ap.x,ap.z) = d;

					// oh, lets do the inner surface as well...
					if( d < imgb(ap.x,ap.z))
						imgb(ap.x,ap.z) = d;
				}
			}
		}
	}

	img.quantize();
	img.save(".xl");
	img.save(mriname("=S/proj.ras.Z"));
	
	imga.quantize();
	imga.save(".xl");
	imga.save(mriname("=S/proja.ras.Z"));
	
	imgb.quantize();
	imgb.save(".xl");
	imgb.save(mriname("=S/projb.ras.Z"));
}


Point operator*(M4& m, Point p){
	float x,y,z,o;

	x = p.x * m.m[0][0] + p.y * m.m[0][1] + p.z * m.m[0][2] + m.m[0][3];
	y = p.x * m.m[1][0] + p.y * m.m[1][1] + p.z * m.m[1][2] + m.m[1][3];
	z = p.x * m.m[2][0] + p.y * m.m[2][1] + p.z * m.m[2][2] + m.m[2][3];
	o = p.x * m.m[3][0] + p.y * m.m[3][1] + p.z * m.m[3][2] + m.m[3][3];

	return Point( int(x/o), int(y/o), int(z/o) );
}

	
	
	
			
			
