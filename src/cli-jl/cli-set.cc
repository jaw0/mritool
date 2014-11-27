
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-set.cc,v 1.1 1995/11/23 04:30:07 jaw Exp jaw $";
#endif
 
// Optimization: -O2

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;
extern int verbosity_level;


DEFUN("set-patient", Fset_patient, Sset_patient, 1,1,1,0,
      "(set-patient PID) set the current patient",
      (Obj p))
{
	if( !ui_jl)
		return jlerror(Sset_patient.name, IC_NIL, "no ui");

	ui_jl->c_patient = 0;
	ui_jl->c_study = 0;
	ui_jl->c_series = 0;
	ui_jl->c_image = 0;
	
	if( INUMP(p))
		ui_jl->c_patient = find( CINT(p));
	if(! ui_jl->c_patient)
		return jlerror(Sset_patient.name, p, "PID not found");

	if(! ui_jl->c_patient->studies)
		ui_jl->c_patient->learn();

	return IC_UNSPEC;
}

DEFUN("set-study", Fset_study, Sset_study, 1,1,1,0,
      "(set-study studyno) set the current study",
      (Obj s))
{
	if( !ui_jl)
		return jlerror(Sset_study.name, IC_NIL, "no ui");

	ui_jl->c_study = 0;
	ui_jl->c_series = 0;
	ui_jl->c_image = 0;

	if( INUMP(s))
		ui_jl->c_study = ui_jl->c_patient->find( CINT(s));

	if(! ui_jl->c_study)
		return jlerror(Sset_study.name, s, "study not found");

	if(! ui_jl->c_study->series)
		ui_jl->c_study->learn();
	
	return IC_UNSPEC;
}

DEFUN("set-series", Fset_sries, Sset_series, 1,1,1,0,
      "(set-series seriesno) set the current series",
      (Obj s))
{

	if( !ui_jl)
		return jlerror(Sset_series.name, IC_NIL, "no ui");

	ui_jl->c_series = 0;
	ui_jl->c_image = 0;

	if( INUMP(s))
		ui_jl->c_series = ui_jl->c_study->find( CINT(s));

	if(! ui_jl->c_series)
		return jlerror(Sset_series.name, s, "series not found");

	if(! ui_jl->c_series->images)
		ui_jl->c_series->learn();

	return IC_UNSPEC;
}

DEFUN("set-image", Fset_img, Sset_img, 1,1,1,0,
      "(set-image imgno) set the current image",
      (Obj i))
{
	int n;
	Image *im = 0;

	if( !ui_jl)
		return jlerror(Sset_img.name, IC_NIL, "no ui");

       	if( Feq(i, maksym_c("prev"))==IC_TRUE){
		n = ui_jl->c_image->num - 1;
		/* if( n<0) n = 0; */
		i = MAKINT( n );
	}else if( Feq(i, maksym_c("next"))==IC_TRUE){
		n = ui_jl->c_image->num + 1;
		/* if( n >= ui_jl->c_series->many()) n --; */
		i = MAKINT( n );
	}

	if( INUMP(i))
		im = ui_jl->c_series->find( CINT(i));

	if( FALSEP(i)){
		ui_jl->c_image = 0;
		return IC_UNSPEC;
	}

	if(! im)
		return jlerror(Sset_img.name, i, "image not found");

	ui_jl->c_image = im;
	ui_jl->do_image();
	
	return IC_UNSPEC;
}

DEFUN("set-mode-view", Fmode_view, Smode_view, 0,0,1,0,
      "(set-mode-view) set the viewing mode",
      ())
{
	if( ui_jl && ui_jl->c_image){
		ui_jl->c_image->C(Image::IFB_VIEW_SEGM);
		ui_jl->c_image->update_view();
		ui_jl->c_image->refresh(*ui_jl);
	}
	return IC_UNSPEC;
}

DEFUN("set-mode-segm", Fmode_segm, Smode_segm, 0,0,1,0,
      "(set-mode-segm) set the viewing mode",
      ())
{
	if( ui_jl&& ui_jl->c_image){
		ui_jl->c_image->S(Image::IFB_VIEW_SEGM);
		ui_jl->c_image->update_view();
		ui_jl->c_image->refresh(*ui_jl);
	}
	return IC_UNSPEC;
}

DEFUN("set-loi", Fset_loi, Sset_loi, 1,1,1,1,
      "(set-loi {n | (l . n)}...) set LOI of contours",
      (Obj argl))
{
	Obj ln, foo;
	int l, n;
	
	if( ui_jl && ui_jl->c_image){

		while( CONSP(argl)){
			ln = CAR(argl);
			argl = CDR(argl);
			if( CONSP(ln)){
				if(! INUMP(CAR(ln)))
					return jlerror(Sset_loi.name, CAR(ln), "WTA: intp");
				if(! INUMP(CDR(ln)))
					return jlerror(Sset_loi.name, CDR(ln), "WTA: intp");
				l = CINT( CAR(ln));
				n = CINT( CDR(ln));
			}else{
				foo = getvalue(maksym_c("mouse:selected"));
				l = INUMP(foo) ? CINT(foo) : 0;
				if(! INUMP(ln))
					return jlerror(Sset_loi.name, ln, "WTA: intp");
				n = CINT(ln);
			}
			ui_jl->c_image->snake[ l ].loi  = n;
			ui_jl->c_image->snake[ l ].ploi = -1;
			Fset(maksym_c("mouse:selected"), MAKINT(
				(l + 1) % ui_jl->c_image->num_of_snakes));
		}
	}else{
		return jlerror(Sset_loi.name, IC_NIL, "Can't set LOI");
	}
	return IC_UNSPEC;
}
	
