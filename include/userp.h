
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifndef _userp_h
#define _userp_h

#ifdef RCSID
static const char *const rcsid_userp_h
= "@(#)$Id: userp.h,v 1.1 1995/11/23 04:30:01 jaw Exp jaw $";
#endif

class UserI;

class UserP {

  public:
	UserI	*ui;
	Patient *c_patient;     // data describing the image we are
        Study   *c_study;       // currently looking at
        Series  *c_series;
        Image   *c_image;
        int     p_sel;

	void snakit(int a, int b, int g, int d, int n, int fr);	// snake image
	void segment(int l, int h, int k, float a, float w);
};

	









#endif /* !_userp_h */
