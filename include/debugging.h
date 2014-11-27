
 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifndef _debugging_h
#define _debugging_h

#ifdef RCSID
static const char *const rcsid_debugging_h
= "@(#)$Id: debugging.h,v 1.1 1995/11/23 04:30:04 jaw Exp jaw $";
#endif

#define N_SECTIONS	32
#define N_FIELDS	32

// sections
#define SECTION_MRI	0
#define SECTION_LISP	1
#define SECTION_INIT	2
#define SECTION_SNAKE	3
#define SECTION_SEGM	4
#define SECTION_WINSYS	5
#define SECTION_
#define SECTION_


// fields
#define DBG_VERBOSE	0
#define DBG_ECHO	1
#define DBG_RESULT	2
#define DBG_SAVE	3
#define DBG_THINK	4
#define DBG_WORK	5
#define DBG_SAVE_X	6
#define DBG_STATS	7




extern boolean debug_flags[ N_SECTIONS ][ N_FIELDS ];

#endif /* !_debugging_h */

