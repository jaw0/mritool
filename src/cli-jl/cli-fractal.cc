

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

// Optimization: None

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-fractal.cc,v 1.1 1995/11/23 04:30:09 jaw Exp jaw $";
#endif
 
#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
#include <X11/Xutil.h>
#include <handy.h>

extern UserI *ui_jl;

#define CODE_SECTION SECTION_LISP
#define NUMBERP(x)      (INUMP(x) || FLOATP(x) || DOUBLEP(x) || BIGNUMP(x))


DEFUN("fractal-analyze", Ffractal_analyze, Sfractal_analyze, 1,2,1,0,
      "(fractal-analyze solid maxr) calculate localized fractal dimension",
      (Obj s, Obj m))
{
	// solid ought be a binary edge (surface) map

	// from: Hastings & Sugihara "Fractals..."
	// contour dimension (ch. 3)
	//
	// k pixels = const * scale ** D
	// ergo, slope of:
	// log k vs log s = D
	
	int maxr;
	
	if(! SOLIDP(s))
		return jlerror(Sfractal_analyze.name, s, "WTA: solidp");
	if(! INUMP(m))
		maxr = 10;
	else
		maxr = CINT(m);

	if( maxr < 4) maxr = 4;

	
	debugp( DBG_THINK )
		ui_jl->thinking();
	
	Solid &solid = *CSOLID(s);
	Fsolid *fractdim = new Fsolid(solid.width-2*maxr, solid.height-2*maxr, solid.depth-2*maxr);
	Solid &frdm = *fractdim;
	Point p, fini(solid.width-2*maxr, solid.height-2*maxr, solid.depth-2*maxr),
		begin(0,0,0), off(maxr,maxr,maxr);
	Vector nr(maxr), dim(maxr-1), den(maxr-1);
	int r, i;
	float min=1e20, max=-1e20;

	for(i=1;i<maxr;i++)
		den[i-1] = log( 2.0*i+1.0 );
	
	looppp(p, begin, fini){

		debugp( DBG_VERBOSE )
			if(!p.x && !p.y)
				cerr << "FA: " << p.z << endl;

		if( solid[p+off]==255){
			// only look at points on surface
			
			nr = 1;	// count center point...
			dim = 0;
			for(r=1; r<maxr; r++){
				// vary box size (r)
				
				nr[r] = nr[r-1];
				
				// count 'em up (6 faces)
				int x,y,z;

				z = p.z-r;
				for(y=p.y-r; y<p.y+r; y++)
					for(x=p.x-r; x<p.x+r; x++)
						if( solid(x+off.x, y+off.y, z+off.z)==255)
							nr[r]++;
				z = p.z+r;
				for(y=p.y-r; y<p.y+r; y++)
					for(x=p.x-r; x<p.x+r; x++)
						if( solid(x+off.x, y+off.y, z+off.z)==255)
							nr[r]++;
				y = p.y-r;
				for(z=p.z-r; z<p.z+r; z++)
					for(x=p.x-r; x<p.x+r; x++)
						if( solid(x+off.x, y+off.y, z+off.z)==255)
							nr[r]++;
				y = p.y+r;
				for(z=p.z-r; z<p.z+r; z++)
					for(x=p.x-r; x<p.x+r; x++)
						if( solid(x+off.x, y+off.y, z+off.z)==255)
							nr[r]++;
				x = p.x-r;
				for(z=p.z-r; z<p.z+r; z++)
					for(y=p.y-r; y<p.y+r; y++)
						if( solid(x+off.x, y+off.y, z+off.z)==255)
							nr[r]++;
				x = p.x+r;
				for(z=p.z-r; z<p.z+r; z++)
					for(y=p.y-r; y<p.y+r; y++)
						if( solid(x+off.x, y+off.y, z+off.z)==255)
							nr[r]++;
				dim[r-1] = log( nr[r] ); 
			}

			Vector foo(maxr-1);
			float slope, fd;

			foo = dim.eigenline(den);
			slope = (foo[1] - foo[0]) / (den[1] - den[0]);
			fd = slope;
			frdm[p.z].put(p, fd);
			
			if( fd < min)
				min = fd;
			if( fd > max)
				max = fd;
			if( fd == 0)
				fd = 0;

			debugp( DBG_VERBOSE )
				cerr << dim << ",\t" << foo << "\t=> " << fd << endl;
		}else{
			frdm[p.z].put(p, 0);
		}
	}

	// normalize image
	debugp( DBG_WORK )
		cerr << "FD range: " << min << " to " << max << endl;
	// for(i=0; i<solid.depth; i++)
	// 	frdm[i].punch(min, max);

	
	debugp( DBG_THINK )
		ui_jl->done_thinking();

	return maksolid( *fractdim );
}
		

		
							
DEFUN("solid-edgemap", Fsolid_edge, Ssolid_edge, 1,1,1,0,
      "(solid-edgemap solid) find the surface of a binary solid",
      (Obj s))
{

	if(! SOLIDP(s))
		return jlerror(Ssolid_edge.name, s, "WTA: solidp");

	Solid &solid = *CSOLID(s);
	Usolid *edges = new Usolid(solid.width, solid.height, solid.depth);
	Solid &edg = *edges;
	Point p, fini(solid.width, solid.height, solid.depth);
	int surface;

	debugp( DBG_THINK )
		ui_jl->thinking();
	
	looppp(p, orgin, fini){

		if( solid[p]==255){
			surface = 0;
			
			if(p.x && solid(p.x-1, p.y, p.z)==255)
				surface ++;
			if(p.x-solid.width+1 && solid(p.x+1, p.y, p.z)==255)
				surface ++;
			if(p.y && solid(p.x, p.y-1, p.z)==255)
				surface ++;
			if(p.y-solid.height+1 && solid(p.x, p.y+1, p.z)==255)
				surface ++;
			if(p.z && solid(p.x, p.y, p.z-1)==255)
				surface ++;
			if(p.z-solid.depth+1 && solid(p.x, p.y, p.z+1)==255)
				surface ++;

			if( surface != 6)
				edg[p.z].put(p, 255);
			else
				edg[p.z].put(p, 0);
			
		}else
			edg[p.z].put(p, 0);
	}

	debugp( DBG_THINK )
		ui_jl->done_thinking();
	
	return maksolid(*edges);
}

