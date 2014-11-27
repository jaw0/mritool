
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifndef _conf_h
#define _conf_h

#ifdef RCSID
static const char *const rcsid_conf_h
= "@(#)$Id: conf.h,v 1.1 1995/11/23 04:30:03 jaw Exp jaw $";
#endif


#define NEW_YACC	// or OLD_YACC
#define NEW_LEX		// or OLD_LEX




// only one of each pair
#ifdef NEW_YACC
#	undef OLD_YACC
#endif

#ifdef NEW_LEX
#	undef OLD_LEX
#endif



#endif // !_conf_h

