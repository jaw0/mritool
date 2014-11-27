 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-proj.cc,v 1.1 1995/11/23 04:30:09 jaw Exp jaw $";
#endif


// Optimization: None

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <handy.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;

#define CODE_SECTION SECTION_LISP


class M4 {
  public:
	float m[4][4];

	void set(int i, float a, float b, float c, float d){
		m[i][0] = a; m[i][1] = b; m[i][2] = c; m[i][3] = d;
	}
	
    friend Point operator*(M4&, Point);
};


DEFUN("solid-projection", Fsolid_project, Ssolid_project, 4,4,1,0,
      "(solid-projection solid ex ey ez) create a projection of the solid",
      (Obj sl, Obj oex, Obj oey, Obj oez))
{
	// from 3D cortex space (a)
	// onto y'=0 plane (ap)
	int ex, ey, ez;
	int y;
	int v;
	M4 xform;
	float d;

	if(! SOLIDP(sl))
		return jlerror(Ssolid_project.name, sl, "WTA: solidp");
	if(! INUMP(oex))
		return jlerror(Ssolid_project.name, oex, "WTA: intp");
	if(! INUMP(oey))
		return jlerror(Ssolid_project.name, oey, "WTA: intp");
	if(! INUMP(oez))
		return jlerror(Ssolid_project.name, oez, "WTA: intp");

	ex = CINT(oex); ey = CINT(oey); ez = CINT(oez);
	Solid &solid = *CSOLID(sl);
	Point a, ap(0,0,0), fini(solid.width, solid.height, solid.depth);
	Fimage *limg  = new Fimage(solid.width*4, solid.depth*4);	// nice big image plane
	Fimage *limga = new Fimage(solid.width*4, solid.depth*4);	// to project on to
	Fimage *limgb = new Fimage(solid.width*4, solid.depth*4);
	Fimage &img = *limg, &imga = *limga, &imgb = *limgb;
	
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

	img.offset = 0;
	imga.offset = 0;
	imgb.offset = 0;
	
	img.quantize();
	img.save(".xl");
	img.save(mriname("=S/proj.ras.Z"));
	
	imga.quantize();
	imga.save(".xl");
	imga.save(mriname("=S/proja.ras.Z"));
	
	imgb.quantize();
	imgb.save(".xl");
	imgb.save(mriname("=S/projb.ras.Z"));

	return Fcons( makimage(*limg), Fcons(makimage(*limga), Fcons(makimage(*limgb), IC_NIL)));
}


Point operator*(M4& m, Point p){
	float x,y,z,o;

	x = p.x * m.m[0][0] + p.y * m.m[0][1] + p.z * m.m[0][2] + m.m[0][3];
	y = p.x * m.m[1][0] + p.y * m.m[1][1] + p.z * m.m[1][2] + m.m[1][3];
	z = p.x * m.m[2][0] + p.y * m.m[2][1] + p.z * m.m[2][2] + m.m[2][3];
	o = p.x * m.m[3][0] + p.y * m.m[3][1] + p.z * m.m[3][2] + m.m[3][3];

	return Point( int(x/o), int(y/o), int(z/o) );
}

	
DEFUN("xyz-projections", Fxyz_proj, Sxyz_proj, 1,1,1,0,
      "(xyz_projections solid) return the 3 primary projections",
      (Obj s))
{

	if(! SOLIDP(s))
		return jlerror(Sxyz_proj.name, s, "WTA: solidp");

	debugp( DBG_THINK )
		ui_jl->thinking();
	
	Solid& solid = *CSOLID(s);
	Bimage *xprj1 = new Fimage(solid.depth, solid.height);
	Bimage *yprj1 = new Fimage(solid.width, solid.depth);
	Bimage *zprj1 = new Fimage(solid.width, solid.height);
	Bimage *xprj2 = new Fimage(solid.depth, solid.height);
	Bimage *yprj2 = new Fimage(solid.width, solid.depth);
	Bimage *zprj2 = new Fimage(solid.width, solid.height);

	int x,y,z;

	// z proj

	debugp( DBG_VERBOSE )
		cerr << "projecting Z" << endl;
	
	loop(y, solid.height){
		loop(x, solid.width){
			float v1=0,v2=0;
			int seen_front = 0;

			loop(z, solid.depth){
				float val = solid(x,y,z);
				if(val != 0.0){
					v2 = val;
					if(! seen_front){
						seen_front = 1;
						v1 = val;
					}
				}
			}
			zprj1->put(x,y, v1);
			zprj2->put(x,y, v2);
		}
	}

	// y proj

	debugp( DBG_VERBOSE )
		cerr << "projecting Y" << endl;
	
	loop(z, solid.depth){
		loop(x, solid.width){
			float v1=0, v2=0;
			int seen_front = 0;

			loop(y, solid.height){
				float val = solid(x,y,z);
				if(val != 0.0){
					v2 = val;
					if(! seen_front){
						seen_front = 1;
						v1 = val;
					}
				}
			}
			yprj1->put(x,z, v1);
			yprj2->put(x,z, v2);
		}
	}

	// x proj

	debugp( DBG_VERBOSE )
		cerr << "projecting X" << endl;
	
	loop(z, solid.depth){
		loop(y, solid.height){
			float v1=0, v2=0;
			int seen_front = 0;

			loop(x, solid.width){
				float val = solid(x,y,z);
				if(val != 0.0){
					v2 = val;
					if(! seen_front){
						seen_front = 1;
						v1 = val;
					}
				}
			}
			xprj1->put(z,y, v1);
			xprj2->put(z,y, v2);
		}
	}

	debugp( DBG_SAVE ){
		xprj1->save("=d/xprj1.ras");
		xprj2->save("=d/xprj2.ras");
		yprj1->save("=d/yprj1.ras");
		yprj2->save("=d/yprj2.ras");
		zprj1->save("=d/zprj1.ras");
		zprj2->save("=d/zprj2.ras");
	}

	Obj v = makvect(6);

	CVECTOR(v)[0] = makimage( *xprj1 );
	CVECTOR(v)[1] = makimage( *xprj2 );
	CVECTOR(v)[2] = makimage( *yprj1 );
	CVECTOR(v)[3] = makimage( *yprj2 );
	CVECTOR(v)[4] = makimage( *zprj1 );
	CVECTOR(v)[5] = makimage( *zprj2 );

	debugp( DBG_THINK )
		ui_jl->done_thinking();

	return v;
}


	
	
