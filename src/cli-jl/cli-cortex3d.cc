
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-cortex3d.cc,v 1.1 1995/11/23 04:30:10 jaw Exp jaw $";
#endif
 
// Optimization:

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <handy.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;

static int strct_stats[4] = {0,0,0,0};

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
 

static float ext_zdim, ext_thick, ext_theta, ext_ratio;
static Solid *ext_solid, *extr_solid;
static int ext_count_t, ext_count_p;

static int ext_cx, ext_cy, ext_cz;
static int ext_xc, ext_yc, ext_zc;
static float ext_zpd2, ext_rad_h;

static void extr_counter(int x, int y, int z){
	int l;

  top:
	if(   x<0 || y<0 || z<0
	   || x>=extr_solid->width
	   || y>=extr_solid->height
	   || z>=extr_solid->depth )
		return;

	l = extr_solid->operator()(x,y,z);

	switch( l ){

	  case 1:	// been visited
		return;

	  case 0:	// not visited, do y.test
		extr_solid->operator[](z).put(x,y, 1);

		{
			int xxc = x - ext_cx;
			int yyc = y - ext_cy;
			int zzc = ( z - ext_cz ) * ext_zdim;

			// find thickness
			float rad, thk;
			rad = sqrt( xxc*xxc + yyc*yyc + zzc*zzc );    	// radius to current pt
			thk = rad - ext_rad_h;
			if( thk < 0) thk = - thk;
			
			if( thk > ext_thick)
				return;
			
			// find angle
			float top, bott;
			top =     ext_xc * xxc
				+ ext_yc * yyc
				+ ext_zc * zzc;
			bott = rad * ext_rad_h;
			float angl = acos( top / bott );
			
			if( angl > ext_theta)
				return;
			
			ext_count_t ++;
			l = ext_solid->operator()(x, y, z);

			if( l==2 || l==255 ) ext_count_p ++;
		}
	}
	extr_counter(x+1, y, z);
	extr_counter(x-1, y, z);
	extr_counter(x, y+1, z);
	extr_counter(x, y-1, z);
	extr_counter(x, y, z+1);
	// extr_counter(x, y, z-1);
	// elim tail recursion
	z--;
	goto top;
}	
		
static void extr_clear_region(int x, int y, int z){
	int l;

  top:
	if(   x<0 || y<0 || z<0
	   || x>=extr_solid->width
	   || y>=extr_solid->height
	   || z>=extr_solid->depth )
		return;

	l = extr_solid->operator()(x,y,z);

	switch( l ){

	  case 0:		// already been cleared
		return;

	  case 1:
		extr_solid->operator[](z).put(x,y, 0);
		break;
	}

	extr_clear_region(x+1, y, z);
	extr_clear_region(x-1, y, z);
	extr_clear_region(x, y+1, z);
	extr_clear_region(x, y-1, z);
	extr_clear_region(x, y, z+1);
	// extr_clear_region(x, y, z-1);
	// elim tail recursion
	z--;
	goto top;
}
	

int extract_level_strct(int x, int y, int z){
	int l, ll;

  top:
	if(   x<0 || y<0 || z<0
	   || x>=ext_solid->width
	   || y>=ext_solid->height
	   || z>=ext_solid->depth )
		return 0;

	l = ext_solid->operator()(x,y,z);

	switch( l ){
	  case 255:	// was lsoi, already visited
	  case 252:	// was cortex, passed y.test
	  case 254:	// was other,  passed y.test
		strct_stats[0] ++;
		return 1;

	  case 251:	// was cortex, failed y.test
	  case 253:	// was other,  failed y.test
		strct_stats[1] ++;
		return 0;
		
	  case 2:	// is lsoi, mark as visited
		ext_solid->operator[](z).put(x,y, 255);
		strct_stats[2] ++;
		break;

	  case 0:	// is other
	  case 1:	// is cortex
	  default:	// can't happen
		strct_stats[3] ++;
		{	//  see if it passes yamakah test

			ext_count_p = ext_count_t = 0;
			ext_xc = x - ext_cx;
			ext_yc = y - ext_cy;
			ext_zc = ( z - ext_cz ) * ext_zdim;
			
			ext_rad_h = sqrt( ext_xc*ext_xc
				+ ext_yc*ext_yc
				+ ext_zc*ext_zc );	// radius out to pt from center

			extr_counter(x, y, z);

			extr_clear_region(x,y,z);
			
			if( ext_count_p < ext_count_t * ext_ratio){
				// did not pass yamakah test
				ext_solid->operator[](z).put(x,y, l?251:253);		// mark pt as failed
				return 0;
			}
			ext_solid->operator[](z).put(x,y, l?252:254);			// mark pt as passed
		}
		break;
	}	// eo sw
	

	
	// recurse
	extract_level_strct(x+1, y, z);
	extract_level_strct(x-1, y, z);
	extract_level_strct(x, y+1, z);
	extract_level_strct(x, y-1, z);
	extract_level_strct(x, y, z+1);
	// extract_level_strct(x, y, z-1);
	// eliminate tail recursion
	z--;
	goto top;

	return 1;
}


static void extract_stg2(Solid &solid, int x, int y, int z, int pass){
	int l;

  top:
	if(   x<0 || y<0 || z<0
	   || x>=solid.width
	   || y>=solid.height
	   || z>=solid.depth )
		return;


	l = solid(x,y,z);


	switch( pass ){
	  case 0:
		switch( l ){
		  case 3:	// edge of region, stop here
			return;

		  case 0:	// other
			solid[z].put(x,y, 254);
			break;

		  case 4:	// possible good stuff (cortex, passed y.test)
			return;

		  case 5:	//possible good stuff (cortex, failed y.test)
			return;
			
		  case 1:	// the good stuff (cortex)
			solid[z].put(x,y, 255);	
			break;
			
		  case 2:	// lsoi
			solid[z].put(x,y, 253);
			break;

		  default:	// been visited
			return;
		}
		break;

	  case 1:
		switch( l ){
		  case 0:	// non-cortex slop
			return;

		  case 1:	// good cortex
			solid[z].put(x,y, 255);	
			break;

		  case 2:	// white matter, inner head
			solid[z].put(x,y, 254);
			break;

		  case 3:	// other inner head
			solid[z].put(x,y, 253);
			break;

		  default:	// been visited
			return;
		}
		break;
	}

	extract_stg2(solid, x+1, y, z, pass);
	extract_stg2(solid, x-1, y, z, pass);
	extract_stg2(solid, x, y+1, z, pass);
	extract_stg2(solid, x, y-1, z, pass);
	extract_stg2(solid, x, y, z+1, pass);
	// extract_stg2(solid, x, y, z-1, pass);
	// elim tail recursion
	z--;
	goto top;
}
	

DEFUN("yamakah3D", Fyamakah3d, Syamakah3d, 6,8,1,0,
      "(yamakah3D level1 level2 level3 thick theta squish [from to]) preliminary step in extracting cortex",
      (Obj l1, Obj l2, Obj l3, Obj thk, Obj tht, Obj squ, Obj from, Obj to))
{
	int level, level2, level_c;
	int start_slice = 3;
	int end_slice = 59;
	float thick, theta, ratio;
	
	if( !ui_jl || !ui_jl->c_image)
		return jlerror(Syamakah3d.name, IC_NIL, "no ui");

	if(! INUMP(l1))
		return jlerror(Syamakah3d.name, l1, "WTA: intp");
	if(! INUMP(l2))
		return jlerror(Syamakah3d.name, l2, "WTA: intp");
	if(! INUMP(l3))
		return jlerror(Syamakah3d.name, l3, "WTA: intp");
	if( !FLOATP(thk) && !INUMP(thk))
		return jlerror(Syamakah3d.name, thk, "WTA: number p");
	if( !FLOATP(tht) && !INUMP(tht))
		return jlerror(Syamakah3d.name, tht, "WTA: number p");
	if( !FLOATP(squ) && !INUMP(squ))
		return jlerror(Syamakah3d.name, squ, "WTA: number p");

	if( INUMP(from))
		start_slice = CINT(from);
	if( INUMP(to))
		end_slice = CINT(to);

	if( start_slice > end_slice ) start_slice = 3;
	if( end_slice > ui_jl->c_series->num_of_images)
		end_slice = ui_jl->c_series->num_of_images-1;


	debugp( DBG_THINK )
		ui_jl->thinking();

	level = CINT(l1);	// levels of csf, bkgrnd
	level2= CINT(l2);
	level_c = CINT(l3);	// level of cortex
	thick = fltof(thk);
	theta = fltof(tht);
	ratio = fltof(squ);
 
	int ht, wd, dp=end_slice-start_slice +1;
	int x,y,z, i;
	Usolid *ssolid = new Usolid(ui_jl->c_image->segm.width, ui_jl->c_image->segm.height, dp);
	Usolid &solid = *ssolid;
	float z_pix_dim = ui_jl->c_image->thickness / ui_jl->c_image->pixel_size;
	
	for(i=0; i<dp; i++) solid[i] = 0;
	
	// gather our brain data 
	SLICE_ITER(start_slice, end_slice, start_slice,
		
		/* load data */
		debugp( DBG_VERBOSE )
			cerr << "Snarfing slice " << sl_i << endl;
		ui_jl->c_image->doit();
		wd = ui_jl->c_image->segm.width;
		ht = ui_jl->c_image->segm.height;

		/* extract desired levels (CSF, bkgrnd, cortex) */
		for(y=0;y<ht;y++) for(x=0;x<wd;x++){
			int l = ui_jl->c_image->segm(x,y);
			
			solid[sl_i-start_slice].put(x,y,
				l==level    ? 2
				:(l==level2 ? 2
				:(l==level_c? 1
				: 0))
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
	x = 1; 
	y = 1; 
	z = solid.depth / 2;

	ext_thick = thick;
	ext_theta = theta;
	ext_ratio = ratio;
	ext_zdim = z_pix_dim;
	ext_zpd2 = z_pix_dim * z_pix_dim;
	ext_solid = &solid;
	ext_cx = solid.width/2;
	ext_cy = solid.height/2;
	ext_cz = solid.depth/2;
	extr_solid = new Usolid(solid.width, solid.height, dp);
	for(int iz=0; iz<ext_solid->depth; iz++)
		for(int iy=0; iy<ext_solid->height; iy++)
			for(int ix=0; ix<ext_solid->width; ix++)
				extr_solid->operator[](iz).put(ix,iy,  0.0);

	extract_level_strct(x,y,z);

	delete extr_solid;
	
	debugp( DBG_SAVE ){
		solid[dp/2].save(mriname("=S/foo.s.ras"));
		solid[dp/2].save(".xl");
	}

	// post-process that result
	for(z=0;z<solid.depth; z++)
		for(y=0;y<solid.height; y++)
			for(x=0;x<solid.width;x++){
				int l = solid(x,y,z);
				if( l == 251 )
					// preserve cortex that failed y.test
					solid[z].put(x,y, 5);
				else if( l == 252)
					// make note of cortex pts that passed y.test
					solid[z].put(x,y, 4);
				else if( l > 250)
					solid[z].put(x,y, 3);
			}
	extract_stg2(solid, solid.width /2, solid.height/2, solid.depth/2, 0);

	debugp( DBG_SAVE ){
		solid[dp/2].save(mriname("=S/foo.p.ras"));
		solid[dp/2].save(".xl");
	}
	
	// (still more) post-process that result
	for(z=0;z<solid.depth; z++)
		for(y=0;y<solid.height; y++)
			for(x=0;x<solid.width;x++){
				int l = solid(x,y,z);

				if( l==255 || l==5 )		// l==4 ? likely don't want to...
					solid[z].put(x,y, 1); 	// cortex
				else if( l==254 )
					solid[z].put(x,y, 2);	// also preserve white matter
				else if( l==253 )
					solid[z].put(x,y, 3);	// other inner head
				else
					solid[z].put(x,y, 0);	// other slop
			}
	extract_stg2(solid, solid.width /2, solid.height/2, solid.depth/2, 1);

	debugp( DBG_STATS ){
		cout << "Overview: " 
			<< strct_stats[0] << "\t"
			<< strct_stats[1] << "\t"
			<< strct_stats[2] << "\t"
			<< strct_stats[3] << endl;
	}
	
	debugp( DBG_SAVE ){
		solid[dp/2].save(mriname("=S/foo.c.ras"));
		solid[dp/2].save(".xl");
	}
	
	debugp( DBG_THINK )
		ui_jl->done_thinking();
	
	return maksolid( *ssolid );
	
}

