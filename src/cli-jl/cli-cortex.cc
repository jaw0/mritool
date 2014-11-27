 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-cortex.cc,v 1.1 1995/11/23 04:30:08 jaw Exp jaw $";
#endif
 
 
#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <handy.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;

#define CODE_SECTION SECTION_LISP

#define SLICE_ITER(lo, hi, start, action)                               \
        if(ui_jl && ui_jl->c_series){                                   \
                int sl_l = ((lo) < 1) ? 1 : (lo);                       \
                int sl_h = ((hi)>ui_jl->c_series->num_of_images)        \
                        ? (ui_jl->c_series->num_of_images) : (hi);      \
                int sl_st= ((start)>(hi)) ? (hi) :                      \
                        (((start)<(lo)) ? (lo) : (start));              \
                int sl_i;                                               \
                Image* sl_curr = ui_jl->c_image;                        \
                for(sl_i=sl_st; sl_i<=sl_h; sl_i++){                    \
                        ui_jl->c_image = ui_jl->c_series->find(sl_i);   \
                        action;                                         \
                }                                                       \
                for(sl_i=sl_st-1; sl_i>=sl_l; sl_i--){                  \
                        ui_jl->c_image = ui_jl->c_series->find(sl_i);   \
                        action;                                         \
                }                                                       \
                ui_jl->c_image = sl_curr;                               \
        }
 
			

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


DEFUN("cortex", Fcortex, Scortex, 4,6,1,0,
      "(cortex level1 level2 radius squish [from to]) extract cortex",
      (Obj l1, Obj l2, Obj rad, Obj squ, Obj from, Obj to))
{
        int level, alevel;
        int start_slice = 3;
        int end_slice = 59;
        float radius, ratio;
 
        if( !ui_jl || !ui_jl->c_image)
                return jlerror(Scortex.name, IC_NIL, "no ui");
 
        if(! INUMP(l1))
                return jlerror(Scortex.name, l1, "WTA: intp");
        if(! INUMP(l2))
                return jlerror(Scortex.name, l2, "WTA: intp");
        if( !FLOATP(rad) && !INUMP(rad))
                return jlerror(Scortex.name, rad, "WTA: number p");
        if( !FLOATP(squ) && !INUMP(squ))
                return jlerror(Scortex.name, squ, "WTA: number p");
 
        if( INUMP(from))
                start_slice = CINT(from);
        if( INUMP(to))
                end_slice = CINT(to);
 
        if( start_slice > end_slice ) start_slice = 3;
        if( end_slice > ui_jl->c_series->num_of_images)
                end_slice = ui_jl->c_series->num_of_images-1;
 
 
        debugp( DBG_THINK )
                ui_jl->thinking();
 
        level = CINT(l1);
        alevel= CINT(l2);
        radius= fltof(rad);
        ratio = fltof(squ);
        extern int extract_level(Solid&, int, int, int, float, float);
 
        int ht, wd, dp=end_slice-start_slice +1;
        int x,y, i;
        Usolid *ssolid = new Usolid(ui_jl->c_image->segm.width, ui_jl->c_image->segm.height, dp);
        Usolid &solid = *ssolid;
 
        for(i=0; i<dp; i++) solid[i] = 0;
 
        // gather our brain data 
        SLICE_ITER(start_slice, end_slice, start_slice,
 
                /* load data */
                debugp( DBG_VERBOSE )
                        cerr << "Snarfing slice " << sl_i << endl;
                ui_jl->c_image->doit();
                wd = ui_jl->c_image->segm.width;
                ht = ui_jl->c_image->segm.height;
                for(y=0;y<ht;y++) for(x=0;x<wd;x++){
                        solid[sl_i-start_slice].put(x,y,
                                        /* (y>ht/2) ? 0 : */
 
                                (ui_jl->c_image->segm(x,y) == level) ? 1
                                : ((ui_jl->c_image->segm(x,y) == alevel) ? 2
                                   : 0 )
 
                                        /* ((   (ui_jl->c_image->segm(x,y) == alevel)
                                        || (ui_jl->c_image->segm(x,y) == level)
                                        ) ? 1 : 0) */
                        );
                }
                /* de-allocate slice data
                since the machine does not have
                enough memory... */
 
                ui_jl->c_image->cleanup();
        );
        ui_jl->c_image->doit();
        debugp( DBG_SAVE )
                solid[dp/2].save( mriname("=S/foo.ras"));
 
        // find a reasonable(?) starting point
 
        for(y=ht/2; y>=0 && !solid(wd/4, y, dp/2); y--);
 
        extract_level(solid, wd/4, int(y-2*radius-1), dp/2, radius, ratio);
        debugp( DBG_SAVE ){
                solid[dp/2].save(mriname("=S/foo.e.ras"));
                solid[dp/2].save(".xl");
        }
 
        debugp( DBG_THINK )
                ui_jl->done_thinking();
 
        return maksolid( *ssolid );
}
 



