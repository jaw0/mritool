

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-lib.cc,v 1.1 1995/11/23 04:30:08 jaw Exp jaw $";
#endif
 
// Optimization: -O2

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
 
#define CODE_SECTION SECTION_LISP
#define NUMBERP(x)      (INUMP(x) || FLOATP(x) || DOUBLEP(x) || BIGNUMP(x))

extern UserI *ui_jl;


DEFUN("solidp", Fsolidp, Ssolidp, 1,1,1,0,
      "(solidp obj) is this a solid?",
      (Obj s))
{
	return SOLIDP(s) ? IC_TRUE : IC_FALSE;
}

DEFUN("imagep", Fimagep, Simagep, 1,1,1,0,
      "(imagep obj) is this an image?",
      (Obj i))
{
	return IMAGEP(i) ? IC_TRUE : IC_FALSE;
}

DEFUN("solid->vector", Fsolid_vector, Ssolid_vector, 1,1,1,0,
      "(solid->vector solid) returns a vector of images",
      (Obj s))
{
	int l, i;
	Obj v;
	Obj im;
	
	if(! SOLIDP(s))
		return jlerror(Ssolid_vector.name, s, "WTA: solidp");

	l = CSOLID(s)->depth;
	v = makvect(l);

	for(i=0; i<l; i++){
		Bimage *ni = CSOLID(s)->operator[](i).clone();
		*ni = CSOLID(s)->operator[](i);
		im = makimage( *ni );
		CVECTOR(v)[i] = im;
	}

	return v;
}

DEFUN("vector->Fsolid", Fvector_fsolid, Svector_fsolid, 1,1,1,0,
      "(vector->Fsolid vector) returns a solid of the vector of images",
      (Obj v))
{
	Solid *s;
	int i;
	
	if( !VECTORP(v) || !IMAGEP( CVECTOR(v)[0] ) )
		return jlerror(Svector_fsolid.name, v, "WTA: vectorp");

	int w = CIMAGE( CVECTOR(v)[0])->width,
		h = CIMAGE( CVECTOR(v)[0])->height,
		d = CLENGTH(v);
	
	s = new Fsolid( w,h,d);

	for(i=0; i<d; i++){
		if(! IMAGEP( CVECTOR(v)[i] ))
			return jlerror(Svector_fsolid.name, CVECTOR(v)[i], "WTA: imagep");
		s->operator[](i) = *CIMAGE( CVECTOR(v)[i] );
	}

	return maksolid( *s );
}

DEFUN("vector->Usolid", Fvector_usolid, Svector_usolid, 1,1,1,0,
      "(vector->Usolid vector) returns a solid of the vector of images",
      (Obj v))
{
	Solid *s;
	int i;
	
	if( !VECTORP(v) || !IMAGEP( CVECTOR(v)[0] ) )
		return jlerror(Svector_usolid.name, v, "WTA: vectorp");

	int w = CIMAGE( CVECTOR(v)[0])->width,
		h = CIMAGE( CVECTOR(v)[0])->height,
		d = CLENGTH(v);
	
	s = new Fsolid( w,h,d);

	for(i=0; i<d; i++){
		if(! IMAGEP( CVECTOR(v)[i] ))
			return jlerror(Svector_usolid.name, CVECTOR(v)[i], "WTA: imagep");
		s->operator[](i) = *CIMAGE( CVECTOR(v)[i] );
	}

	return maksolid( *s );
}

DEFUN("img:makeU", Fimg_makU, Simg_makU, 0,2,1,0,
      "(img:makeU width height) create an 8bit image",
      (Obj w, Obj h))
{
	int ww=0, hh=0;
	
	if( INUMP(w))
		ww = CINT(w);
	if( INUMP(h))
		hh = CINT(h);

	return makimage( *new Uimage( ww, hh));
}

DEFUN("img:makeF", Fimg_makF, Simg_makF, 0,2,1,0,
      "(img:makeF width height) create an floating point image",
      (Obj w, Obj h))
{
	int ww=0, hh=0;
	
	if( INUMP(w))
		ww = CINT(w);
	if( INUMP(h))
		hh = CINT(h);

	return makimage( *new Fimage( ww, hh));
}

DEFUN("img:makeC", Fimg_makC, Simg_makC, 0,2,1,0,
      "(img:makeC width height) create a complex image",
      (Obj w, Obj h))
{
	int ww=0, hh=0;

	if( INUMP(w))
		ww = CINT(w);
	if( INUMP(h))
		hh = CINT(h);

	return makimage( *new Cimage( ww, hh));
}

DEFUN("img:cpy", Fimg_cpy, Simg_cpy, 1,2,1,0,
      "(img:cpy dst-img src-img | src-image) copy",
      (Obj d, Obj s))
{
	if(! IMAGEP(d))
		return jlerror(Simg_cpy.name, d, "WTA: imagep");

	if( IMAGEP(s))
		CIMAGE(d) ->operator=( * CIMAGE(s));
	else if( FLOATP(s))
		CIMAGE(d) ->operator=( CFLOAT(s));
	else if( INUMP(s))
		CIMAGE(d) ->operator=( CINT(s));
	else{
		Bimage *foo = CIMAGE(d)->clone();
		foo->operator=( * CIMAGE(d));
		return makimage( *foo );
	}
	return d;
}

// kmeans has img parameters backwards
// ought fix it RSN -> dstimg.kmeans( srcimg, ...)
DEFUN("img:kmeans", Fimg_kmeans, Simg_kmeans, 3,3,1,0,
      "(img:kmeans img k maxiter) perform k-means segmentation",
      (Obj img, Obj k, Obj m))
{
	if(! IMAGEP(img))
		return jlerror(Simg_kmeans.name, img, "WTA: imagep");
	if(! INUMP(k))
		return jlerror(Simg_kmeans.name, k, "WTA: intp");
	if(! INUMP(m))
		return jlerror(Simg_kmeans.name, m, "WTA: intp");

	Bimage *foo = new Uimage( CIMAGE(img)->width, CIMAGE(img)->height);

	CIMAGE(img)->kmeans( *foo, CINT(k), CINT(m));

	return makimage( *foo);
}

// some colormap support

DEFUN("img:cmap:value", Fcmap_value, Scmap_value, 2,2,1,0,
      "(img:cmap:value img n)",
      (Obj img, Obj n))
{
	Obj v;
	
	if(! IMAGEP(img))
		return jlerror(Scmap_value.name, img, "WTA: imagep");
	if(! INUMP(n))
		return jlerror(Scmap_value.name, n, "WTA: intp");

	if(! CIMAGE(img)->cmap)
		return jlerror(Scmap_value.name, img, "Image has no color map");


	v = makvect(3);

	CVECTOR(v)[0] = CIMAGE(img)->cmap->r[ CINT(n) ];
	CVECTOR(v)[1] = CIMAGE(img)->cmap->g[ CINT(n) ];
	CVECTOR(v)[2] = CIMAGE(img)->cmap->b[ CINT(n) ];

	return v;
}

DEFUN("img:cmap:put", Fcmap_put, Scmap_put, 3,3,1,0,
      "(img:cmap:put img n #(r g b))",
      (Obj img, Obj n, Obj v))
{
	
	if(! IMAGEP(img))
		return jlerror(Scmap_put.name, img, "WTA: imagep");
	if(! INUMP(n))
		return jlerror(Scmap_put.name, n, "WTA: intp");
	if(! VECTORP(v))
		return jlerror(Scmap_put.name, v, "WTA: vectorp");
	if(! CIMAGE(img)->cmap)
		return jlerror(Scmap_put.name, img, "Image has no color map");

	if( CLENGTH(v)!=3
	   || !INUMP(CVECTOR(v)[0])
	   || !INUMP(CVECTOR(v)[1])
	   || !INUMP(CVECTOR(v)[2]))
		return jlerror(Scmap_put.name, v, "WTA: vector of 3 ints p");	

	CIMAGE(img)->cmap->r[ CINT(n) ] = CINT( CVECTOR(v)[0]);
	CIMAGE(img)->cmap->g[ CINT(n) ] = CINT( CVECTOR(v)[1]);
	CIMAGE(img)->cmap->b[ CINT(n) ] = CINT( CVECTOR(v)[2]);
	return v;
}

DEFUN("img:width", Fimg_width, Simg_width, 1,1,1,0,
      "(img:width img) how wide?",
      (Obj img))
{
	if(! IMAGEP(img))
		return jlerror(Simg_width.name, img, "WTA: imagep");

	return MAKINT( CIMAGE(img)->width);
}

DEFUN("img:height", Fimg_height, Simg_height, 1,1,1,0,
      "(img:height img) how high?",
      (Obj img))
{
	if(! IMAGEP(img))
		return jlerror(Simg_height.name, img, "WTA: imagep");

	return MAKINT( CIMAGE(img)->height);
}
	
DEFUN("img:ncolors", Fimg_ncolors, Simg_ncolors, 1,1,1,0,
      "(img:ncolors img) how many colors?",
      (Obj img))
{
	if(! IMAGEP(img))
		return jlerror(Simg_ncolors.name, img, "WTA: imagep");

	if( CIMAGE(img)->cmap)
		return MAKINT( CIMAGE(img)->cmap->len);
	else
		return MAKINT( 256 );
}

DEFUN("img:segment", Fimg_segm ,Simg_segm, 5,6,1,0,
      "(img:segment img k a mk mi [mw])",
      (Obj img, Obj k, Obj a, Obj mk, Obj mi, Obj mw))
{
	if(! IMAGEP(img))
		return jlerror(Simg_segm.name, img, "WTA: imagep");
	if(! NUMBERP(k))
		return jlerror(Simg_segm.name, k, "WTA: intp");
	if(! NUMBERP(a))
		return jlerror(Simg_segm.name, a, "WTA: floatp");
	if(! NUMBERP(mk))
		return jlerror(Simg_segm.name, mk, "WTA: intp");
	if(! NUMBERP(mi))
		return jlerror(Simg_segm.name, mi, "WTA: intp");
	if( NBOUNDP(mw))
		mw = makfloat(1);
	if(! NUMBERP(mw))
		return jlerror(Simg_segm.name, mw, "WTA: floatp");

	debugp( DBG_THINK )
		ui_jl->thinking();

	Fsolid o( CIMAGE(img)->width, CIMAGE(img)->height, 1);
	Usolid s( CIMAGE(img)->width, CIMAGE(img)->height, 1);
	Bimage *foo = new Uimage( CIMAGE(img)->width, CIMAGE(img)->height);
	
	o[0] = * CIMAGE(img);

	o.segment(s, intof(k), fltof(a), 1.0, 0,
		intof(mk), intof(mi), fltof(mw));

	foo->operator=( s[0] );

	debugp( DBG_THINK )
		ui_jl->done_thinking();

	return makimage( * foo );
}

DEFUN("img:histogram", Fimg_hist, Simg_hist, 1,1,1,0,
      "(img:histogram img) returns vector histogram",
      (Obj img))
{
	Vector v(256);
	int i;
	Obj vv;
	
	if(! IMAGEP(img))
		return jlerror(Simg_hist.name, img, "WTA: imagep");

	CIMAGE(img)->histogram(v);
	vv = makvect( 256 );
	
	for(i=0; i<256; i++){
		CVECTOR(vv)[i] = MAKINT( int(v[i]) );
	}
	return vv;
}

DEFUN("2D->3D", F2d_3d, S2d_3d, 2,2,1,0,
      "(2D->3D img zdim) returns surface as a solid",
      (Obj img, Obj zdim))
{
	int x,y,z, w,h,d, l;
	Usolid *sld;
	
	if(! IMAGEP(img))
		return jlerror(S2d_3d.name, img, "WTA: imagep");
	if(! INUMP(zdim))
		return jlerror(S2d_3d.name, zdim, "WTA: intp");
	
	sld = new Usolid( CIMAGE(img)->width, CIMAGE(img)->height, CINT(zdim));

	w = CIMAGE(img)->width;
	h = CIMAGE(img)->height;
	d = CINT(zdim);
	
	debugp( DBG_THINK )
		ui_jl->thinking();
	
	for(z=0; z<d; z++){
		sld->operator[](z).operator=( 0.0 ); 
	}
	
	for(y=0; y<h; y++) for(x=0; x<w; x++){
		l = CIMAGE(img)->value(x, y);
		
		sld->operator[](l).put(x,y, 255);
	}
	
	debugp( DBG_THINK )
		ui_jl->done_thinking();

	return maksolid( *sld );
}

DEFUN("3D->2D", F3d_2d, S3d_2d, 1,1,1,0,
      "(3D->2D solid) returns pair of range images",
      (Obj s))
{
	int x, y, z, w, h, d;
	Bimage *foo, *bar;
	int v;
	
	if(! SOLIDP(s))
		return jlerror(S3d_2d.name, s, "WTA: solidp");

	w = CSOLID(s)->width;
	h = CSOLID(s)->height;
	d = CSOLID(s)->depth;
	
	foo = new Fimage(w, h);
	bar = new Fimage(w, h);

	for(y=0; y<h; y++) for(x=0; x<w; x++){

		for(z=0; z<d; z++){
			v = CSOLID(s)->operator[](z).value(x,y);

			if( v ){
				foo->put(x,y, z);
				break;
			}
		}
		for(z=d-1; z>=0; z--){
			v = CSOLID(s)->operator[](z).value(x,y);

			if( v ){
				bar->put(x,y, d-z);
				break;
			}
		}
	}

	return Fcons( makimage( *foo), makimage( *bar));
}

DEFUN("img:yam->cnt", Fimg_yam_cnt, Simg_yam_cnt, 3,3,1,0,
      "(img:yam->cnt img l1 l2) convert yamakahed image int contour",
      (Obj img, Obj l1, Obj l2))
{
	int il1,il2, w,h, x,y;
	Bimage *bimg, *nimg;

	if(! IMAGEP(img))
		return JLERROR(Simg_yam_cnt.name, img, "WTA: imagep");
	if(! INUMP(l1))
		return JLERROR(Simg_yam_cnt.name, l1, "WTA: intp");
	if(! INUMP(l2))
		return JLERROR(Simg_yam_cnt.name, l2, "WTA: intp");
	

	il1 = CINT(l1);
	il2 = CINT(l2);
	bimg = CIMAGE(img);
	nimg = bimg->clone();
	w = bimg->width;
	h = bimg->height;

	for(y=0; y<h; y++) for(x=0; x<w; x++){
		if( !x || !y || x==w-1 || y==h-1 ){
			nimg->put(x,y, 0);
			continue;
		}
		if(    int(bimg->value(x,  y  )) == il1		/* pixel is on cortex */
		    &&(int(bimg->value(x-1,y  )) == il2		/* adjacent to bkgrnd */
		    || int(bimg->value(x+1,y  )) == il2
		    || int(bimg->value(x,  y-1)) == il2
		    || int(bimg->value(x,  y+1)) == il2)
		    &&(int(bimg->value(x-1,y  )) == il1		/* but not isolated */
		    || int(bimg->value(x+1,y  )) == il1
		    || int(bimg->value(x,  y-1)) == il1
		    || int(bimg->value(x,  y+1)) == il1))
			nimg->put(x,y, 1);
		else
			nimg->put(x,y, 0);
	}

	*bimg = *nimg;
	delete nimg;
	return img;
}


	
			
			
	
