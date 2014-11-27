

// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-lists.cc,v 1.1 1995/11/23 04:30:07 jaw Exp jaw $";
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


DEFUN("patient-list", Fpatient_l, Spatient_l, 0,0,1,0,
      "(patient-list) return list of patients",
      ())
{
	return pat_list();
}

DEFUN("study-list", Fstudy_l, Sstudy_l, 0,0,1,0,
      "(study-list) return list of studies",
      ())
{
	if( ui_jl)
		return ui_jl->c_patient->list();
	return IC_FALSE;
}

DEFUN("series-list", Fseries_l, Sseries_l, 0,0,1,0,
      "(series-list) return list of series",
      ())
{
	if( ui_jl)
		return ui_jl->c_study->list();
	return IC_FALSE;
}

DEFUN("image-list", Fimage_l, Simage_l, 0,0,1,0,
      "(image-list) return list of image",
      ())
{
	if( ui_jl)
		return ui_jl->c_series->list();
	return IC_FALSE;
}

DEFUN("loi-list", Floi_l, Sloi_l, 0,0,1,0,
      "(loi-list) return list of lois",
      ())
{
	if( ui_jl)
		return ui_jl->c_image->lois();
	return IC_FALSE;
}

DEFUN("image-info", Fimage_info, Simage_info, 0,0,1,0,
      "(image-info) return useful image info",
      ())
{
	
	if( ui_jl)
		return ui_jl->c_image->info();
	return IC_FALSE;
}

DEFUN("series-info", Fseries_info, Sseries_info, 0,0,1,0,
      "(series-info) return useful series info",
      ())
{
	
	if( ui_jl)
		return ui_jl->c_series->info();
	return IC_FALSE;
}

DEFUN("study-info", Fstudy_info, Sstudy_info, 0,0,1,0,
      "(study-info) return useful study info",
      ())
{
	
	if( ui_jl)
		return ui_jl->c_study->info();
	return IC_FALSE;
}

DEFUN("patient-info", Fpatient_info, Spatient_info, 0,0,1,0,
      "(patient-info) return useful patient info",
      ())
{
	
	if( ui_jl)
		return ui_jl->c_patient->info();
	return IC_FALSE;
}
